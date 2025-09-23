/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "m5_unit_fingerprint2.hpp"

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
// 静态成员变量定义
SemaphoreHandle_t M5UnitFingerprint2::mutexLock         = nullptr;
SemaphoreHandle_t M5UnitFingerprint2::responseSemaphore = nullptr;
M5UnitFingerprint2* M5UnitFingerprint2::instance        = nullptr;

// 队列相关静态变量
QueueHandle_t M5UnitFingerprint2::dataQueue      = nullptr;
TaskHandle_t M5UnitFingerprint2::parseTaskHandle = nullptr;
#endif

void M5UnitFingerprint2::acquireMutex()
{
    if (mutexLock == nullptr) {
        mutexLock = xSemaphoreCreateMutex();
    }
    if (mutexLock != nullptr) {
        xSemaphoreTake(mutexLock, portMAX_DELAY);
    }
}

void M5UnitFingerprint2::releaseMutex()
{
    if (mutexLock != nullptr) {
        xSemaphoreGive(mutexLock);
    }
}

// 构造函数 - Arduino 版本
M5UnitFingerprint2::M5UnitFingerprint2(uint32_t password, uint32_t address, HardwareSerial* serialPort, int txPin,
                                       int rxPin)
{
    _serialPort   = serialPort;
    _fp2_password = password;
    _fp2_address  = address;
    _txPin        = txPin;
    _rxPin        = rxPin;
    instance      = this;
}

// 析构函数，ESP 平台上删除互斥锁
M5UnitFingerprint2::~M5UnitFingerprint2()
{
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    // 停止解析任务
    if (parseTaskHandle != nullptr) {
        vTaskDelete(parseTaskHandle);
        parseTaskHandle = nullptr;
    }

    // 删除队列
    if (dataQueue != nullptr) {
        vQueueDelete(dataQueue);
        dataQueue = nullptr;
    }

    // 删除响应信号量
    if (responseSemaphore != nullptr) {
        vSemaphoreDelete(responseSemaphore);
        responseSemaphore = nullptr;
    }
#endif

    // 清理静态实例指针
    if (instance == this) {
        instance = nullptr;
    }

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    if (mutexLock != NULL) {
        vSemaphoreDelete(mutexLock);
        mutexLock = NULL;
    }
#endif
}

// 读取串口数据
size_t M5UnitFingerprint2::readSerial(uint8_t* buffer, size_t length)
{
    if (buffer == nullptr || length == 0) {
        return 0;
    }

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    // Arduino 平台
    if (_serialPort != nullptr) {
        size_t bytesRead = 0;
        while (bytesRead < length && _serialPort->available()) {
            buffer[bytesRead] = _serialPort->read();
            bytesRead++;
        }
        return bytesRead;
    }
#endif
    return 0;
}

// 写入串口数据
size_t M5UnitFingerprint2::writeSerial(const uint8_t* buffer, size_t length)
{
    if (buffer == nullptr || length == 0) {
        return 0;
    }

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    // Arduino 平台
    if (_serialPort != nullptr) {
        return _serialPort->write(buffer, length);
    }
#endif
    return 0;
}

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
// Arduino平台数据解析任务 - 使用中断方式接收
void M5UnitFingerprint2::parseDataTask(void* parameter)
{
    M5UnitFingerprint2* instance = static_cast<M5UnitFingerprint2*>(parameter);
    uint8_t buffer[256];

    while (true) {
        // 检查串口中断接收的数据
        if (instance->_serialPort != nullptr && instance->_serialPort->available()) {
            size_t len = instance->readSerial(buffer, sizeof(buffer));
            if (len > 0) {
                // 直接处理接收到的数据，更新时间戳
                instance->processReceivedData(buffer, len);
            }
        }
        
        // 检查是否可以开始解析数据包（基于包间隔）
        instance->checkAndParsePackets();
        
        // 清理过期的解析包
        instance->cleanupExpiredPackets();
        
        vTaskDelay(pdMS_TO_TICKS(1)); // 1ms检查间隔
    }
}
#endif

// 处理接收到的数据
void M5UnitFingerprint2::processReceivedData(uint8_t* data, size_t length)
{
    // 获取互斥锁
    acquireMutex();

#if defined M5_MODULE_DEBUG_SERIAL
    serialPrintf("RX: %d bytes -> ", length);
    //打印全部数据
    for (size_t i = 0; i < length; i++) {
        M5_MODULE_DEBUG_SERIAL.printf("%02X ", data[i]);
    }
    M5_MODULE_DEBUG_SERIAL.println("");
#endif

    // 将接收到的数据添加到缓冲区
    for (size_t i = 0; i < length; i++) {
        if (_recvIndex < sizeof(_recvBuffer)) {
            _recvBuffer[_recvIndex++] = data[i];
        } else {
            // 缓冲区溢出，重置
            serialPrintln("Receive buffer overflow, resetting");
            _recvIndex = 0;
            _recvBuffer[_recvIndex++] = data[i];
        }
    }

    // 更新最后接收时间和新数据标志
    _lastReceiveTime = millis();
    _hasNewData = true;

    // 释放互斥锁
    releaseMutex();
}

// 检查是否可以开始解析数据包（基于包间隔）
void M5UnitFingerprint2::checkAndParsePackets()
{
    // 获取互斥锁
    acquireMutex();

    // 检查是否有新数据且已超过包间隔时间
    if (_hasNewData && (millis() - _lastReceiveTime >= PACKET_INTERVAL_MS)) {
        _hasNewData = false; // 清除新数据标志
        
        // 开始解析缓冲区中的所有完整数据包
        size_t processedBytes = 0;
        while (processedBytes < _recvIndex) {
            size_t packetLength = 0;
            uint8_t* bufferStart = &_recvBuffer[processedBytes];
            size_t remainingBytes = _recvIndex - processedBytes;
            
            // 尝试从当前位置提取一个完整的数据包
            if (extractSinglePacket(bufferStart, remainingBytes, packetLength)) {
                // 检查是否为唤醒包，如果是则调用回调函数而不添加到解析队列
                if (handleWakeupPacket(bufferStart, packetLength)) {
                    // 是唤醒包，已经处理完毕，跳过addParsedPacket
                    processedBytes += packetLength;
                } else {
                    // 不是唤醒包，正常添加到解析队列
                    addParsedPacket(bufferStart, packetLength);
                    processedBytes += packetLength;
                }
                
// #if defined M5_MODULE_DEBUG_SERIAL
//                 serialPrintf("Extracted packet: %d bytes, total processed: %d/%d\r\n", 
//                            packetLength, processedBytes, _recvIndex);
// #endif
            } else {
                // 无法提取完整包，查找下一个可能的起始码
                bool foundNextStart = false;
                for (size_t i = 1; i < remainingBytes - 1; i++) {
                    uint16_t startCode = (bufferStart[i] << 8) | bufferStart[i + 1];
                    if (startCode == FINGERPRINT_STARTCODE) {
                        processedBytes += i;
                        foundNextStart = true;
                        break;
                    }
                }
                
                if (!foundNextStart) {
                    // 没有找到更多起始码，保留剩余数据等待下次接收
                    if (processedBytes < _recvIndex) {
                        memmove(_recvBuffer, &_recvBuffer[processedBytes], _recvIndex - processedBytes);
                        _recvIndex -= processedBytes;
                    } else {
                        _recvIndex = 0;
                    }
                    break;
                }
            }
        }
        
        // 如果所有数据都被处理完，清空缓冲区
        if (processedBytes >= _recvIndex) {
            _recvIndex = 0;
        }
    }

    // 如果正在等待响应，检查解析队列中是否有匹配的包
    if (_waitingForResponse && _expectedPacket != nullptr) {
        if (getMatchingPacket(*_expectedPacket)) {
            _packetReceived = true;
            _waitingForResponse = false;

            // 通知等待的线程
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
            if (responseSemaphore != nullptr) {
                xSemaphoreGive(responseSemaphore);
            }
#endif
        }
    }

    // 释放互斥锁
    releaseMutex();
}

// 从缓冲区中精确解析单个数据包
bool M5UnitFingerprint2::extractSinglePacket(uint8_t* buffer, size_t bufferSize, size_t& packetLength)
{
    packetLength = 0;
    
    // 检查是否有足够的数据用于最小包头
    if (bufferSize < 9) {
        return false;
    }
    
    // 检查起始码
    uint16_t startCode = (buffer[0] << 8) | buffer[1];
    if (startCode != FINGERPRINT_STARTCODE) {
        return false;
    }
    
    // 解析地址（4字节）
    uint32_t address = (buffer[2] << 24) | (buffer[3] << 16) | (buffer[4] << 8) | buffer[5];
    
    // 解析包类型（1字节）
    uint8_t packetType = buffer[6];
    
    // 解析包长度（2字节）
    uint16_t dataLength = (buffer[7] << 8) | buffer[8];
    
    // 计算完整包的长度：起始码(2) + 地址(4) + 类型(1) + 长度(2) + 数据 + 校验和(2)
    size_t totalPacketLength = 9 + dataLength;
    
    // 检查缓冲区是否包含完整的包
    if (bufferSize < totalPacketLength) {
        return false;
    }
    
    // 验证校验和
    uint16_t receivedChecksum = (buffer[totalPacketLength - 2] << 8) | buffer[totalPacketLength - 1];
    uint16_t calculatedChecksum = packetType + dataLength;
    for (size_t i = 9; i < totalPacketLength - 2; i++) {
        calculatedChecksum += buffer[i];
    }
    calculatedChecksum &= 0xFFFF;
    
    if (receivedChecksum != calculatedChecksum) {
#if defined M5_MODULE_DEBUG_SERIAL
        serialPrintf("Checksum mismatch: expected 0x%04X, got 0x%04X\r\n", 
                   calculatedChecksum, receivedChecksum);
#endif
        return false;
    }
    
    // 验证地址是否匹配（可选，根据需要启用）
    if (address != _fp2_address) {
#if defined M5_MODULE_DEBUG_SERIAL
        serialPrintf("Address mismatch: expected 0x%08X, got 0x%08X\r\n", _fp2_address, address);
#endif
        // 对于某些情况可能需要接受不同地址的包，这里先接受
    }
    
    packetLength = totalPacketLength;
    
#if defined M5_MODULE_DEBUG_SERIAL
    // 获取确认码（数据部分的第一个字节，如果有数据的话）
    uint8_t confirmationCode = (dataLength > 2) ? buffer[9] : 0x00;
    
    // 获取校验和
    uint16_t checksum = (buffer[totalPacketLength - 2] << 8) | buffer[totalPacketLength - 1];
    
    serialPrintf("Valid packet found: type=0x%02X, addr=0x%08X, len=%d, total=%d, confirm=0x%02X, checksum=0x%04X\r\n", 
               packetType, address, dataLength, totalPacketLength, confirmationCode, checksum);
    
    // // 打印前6字节数据（如果有足够的数据）
    // if (dataLength > 2) {  // 减去2字节校验和
    //     uint16_t actualDataLength = dataLength - 2;
    //     M5_MODULE_DEBUG_SERIAL.print("Data: ");
    //     for (int i = 0; i < 8 && i < actualDataLength; i++) {
    //         M5_MODULE_DEBUG_SERIAL.printf("%02X ", buffer[9 + i]);
    //     }
    //     if (actualDataLength > 8) {
    //         M5_MODULE_DEBUG_SERIAL.print("...");
    //     }
    //     M5_MODULE_DEBUG_SERIAL.println("");
    // }
#endif
    
    return true;
}

// 将解析好的包添加到解析队列
void M5UnitFingerprint2::addParsedPacket(const uint8_t* packetData, size_t packetLength)
{
    if (packetLength > sizeof(_parsedPackets[0].data)) {
        serialPrintf("Packet too large: %d bytes\r\n", packetLength);
        return;
    }
    
    // 如果队列已满，移除最老的包
    if (_parsedPacketCount >= MAX_PARSED_PACKETS) {
        // 移动数组，删除最老的包
        for (size_t i = 0; i < MAX_PARSED_PACKETS - 1; i++) {
            _parsedPackets[i] = _parsedPackets[i + 1];
        }
        _parsedPacketIndex = MAX_PARSED_PACKETS - 1;
        _parsedPacketCount = MAX_PARSED_PACKETS - 1;
    }
    
    // 添加新包到队列
    memcpy(_parsedPackets[_parsedPacketIndex].data, packetData, packetLength);
    _parsedPackets[_parsedPacketIndex].length = packetLength;
    _parsedPackets[_parsedPacketIndex].timestamp = millis();
    
    _parsedPacketIndex = (_parsedPacketIndex + 1) % MAX_PARSED_PACKETS;
    if (_parsedPacketCount < MAX_PARSED_PACKETS) {
        _parsedPacketCount++;
    }
    
// #if defined M5_MODULE_DEBUG_SERIAL
//     serialPrintf("Added parsed packet to queue: %d bytes, queue size: %d\r\n", 
//                packetLength, _parsedPacketCount);
// #endif
}

// 从解析队列中获取匹配的包
bool M5UnitFingerprint2::getMatchingPacket(Fingerprint_Packet& packet)
{
    for (size_t i = 0; i < _parsedPacketCount; i++) {
        const ParsedPacket& parsedPkt = _parsedPackets[i];
        
        // 解析包头信息
        if (parsedPkt.length < 9) continue;
        
        uint16_t startCode = (parsedPkt.data[0] << 8) | parsedPkt.data[1];
        uint32_t address = (parsedPkt.data[2] << 24) | (parsedPkt.data[3] << 16) | 
                          (parsedPkt.data[4] << 8) | parsedPkt.data[5];
        uint8_t packetType = parsedPkt.data[6];
        uint16_t dataLength = (parsedPkt.data[7] << 8) | parsedPkt.data[8];
        
        // 检查是否是我们期望的包类型（通常是ACK包或数据包）
        if (startCode == FINGERPRINT_STARTCODE && 
            (packetType == FINGERPRINT_PACKET_ACKPACKET || 
             packetType == FINGERPRINT_PACKET_DATAPACKET ||
             packetType == FINGERPRINT_PACKET_ENDDATAPACKET)) {
            
            // 构建Fingerprint_Packet对象
            uint16_t actualDataLength = dataLength - 2; // 减去校验和长度
            packet = Fingerprint_Packet(startCode, address, packetType, 
                                       &parsedPkt.data[9], actualDataLength);
            
            // 从队列中移除这个包
            for (size_t j = i; j < _parsedPacketCount - 1; j++) {
                _parsedPackets[j] = _parsedPackets[j + 1];
            }
            _parsedPacketCount--;
            if (_parsedPacketIndex > 0) {
                _parsedPacketIndex--;
            } else {
                _parsedPacketIndex = MAX_PARSED_PACKETS - 1;
            }
            
// #if defined M5_MODULE_DEBUG_SERIAL
//             serialPrintf("Found matching packet: type=0x%02X, data_len=%d\r\n", 
//                        packetType, actualDataLength);
// #endif
            return true;
        }
    }
    
    return false;
}

// 清理过期的解析包
void M5UnitFingerprint2::cleanupExpiredPackets()
{
    const unsigned long PACKET_TIMEOUT_MS = 5000; // 5秒超时
    unsigned long currentTime = millis();
    
    acquireMutex();
    
    size_t validPackets = 0;
    for (size_t i = 0; i < _parsedPacketCount; i++) {
        if (currentTime - _parsedPackets[i].timestamp < PACKET_TIMEOUT_MS) {
            if (validPackets != i) {
                _parsedPackets[validPackets] = _parsedPackets[i];
            }
            validPackets++;
        }
    }
    
    if (validPackets != _parsedPacketCount) {
        _parsedPacketCount = validPackets;
        _parsedPacketIndex = validPackets % MAX_PARSED_PACKETS;
#if defined M5_MODULE_DEBUG_SERIAL
        serialPrintf("Cleaned up expired packets, remaining: %d\r\n", _parsedPacketCount);
#endif
    }
    
    releaseMutex();
}

// 尝试解析完整的数据包
bool M5UnitFingerprint2::tryParsePacket()
{
    if (_recvIndex < 9) {  // 最小包头长度
        return false;
    }

    // 查找起始码
    size_t startPos = 0;
    bool foundStart = false;
    for (size_t i = 0; i <= _recvIndex - 2; i++) {
        uint16_t startCode = (_recvBuffer[i] << 8) | _recvBuffer[i + 1];
        if (startCode == FINGERPRINT_STARTCODE) {
            startPos   = i;
            foundStart = true;
            break;
        }
    }

    if (!foundStart) {
        // 没有找到起始码，清空缓冲区
        _recvIndex = 0;
        return false;
    }

    // 如果起始码不在开头，移动数据
    if (startPos > 0) {
        memmove(_recvBuffer, _recvBuffer + startPos, _recvIndex - startPos);
        _recvIndex -= startPos;
    }

    // 检查是否有足够的数据解析包头
    if (_recvIndex < 9) {
        return false;
    }

    // 解析包长度
    uint16_t dataLength    = (_recvBuffer[7] << 8) | _recvBuffer[8];
    size_t totalPacketSize = 9 + dataLength;

    // 检查是否接收到完整的数据包
    if (_recvIndex < totalPacketSize) {
        return false;
    }

    // 构建数据包
    uint32_t address    = (_recvBuffer[2] << 24) | (_recvBuffer[3] << 16) | (_recvBuffer[4] << 8) | _recvBuffer[5];
    uint8_t type        = _recvBuffer[6];
    uint8_t* packetData = &_recvBuffer[9];
    uint16_t actualDataLength = dataLength - 2;  // 减去校验和长度

    // 验证校验和
    uint16_t receivedChecksum   = (_recvBuffer[totalPacketSize - 2] << 8) | _recvBuffer[totalPacketSize - 1];
    uint16_t calculatedChecksum = type + dataLength;
    for (uint16_t i = 0; i < actualDataLength; i++) {
        calculatedChecksum += packetData[i];
    }
    calculatedChecksum &= 0xFFFF;

    if (receivedChecksum != calculatedChecksum) {
        serialPrintln("Checksum mismatch");
        _recvIndex = 0;
        return false;
    }

    // 构建响应包
    *_expectedPacket = Fingerprint_Packet(FINGERPRINT_STARTCODE, address, type, packetData, actualDataLength);

    // serialPrintf("Parsed complete packet, type: 0x%02X, data length: %d\r\n",
    //             type, actualDataLength);

    // 移除已处理的数据包
    if (_recvIndex > totalPacketSize) {
        memmove(_recvBuffer, _recvBuffer + totalPacketSize, _recvIndex - totalPacketSize);
        _recvIndex -= totalPacketSize;
    } else {
        _recvIndex = 0;
    }

    return true;
}

// 初始化模块
bool M5UnitFingerprint2::begin()
{
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    // 创建互斥锁
    if (mutexLock == NULL) {
        mutexLock = xSemaphoreCreateMutex();
        if (mutexLock == NULL) {
            serialPrintln("Failed to create mutex lock.");
            return false;
        } else {
            serialPrintln("Mutex lock created successfully.");
        }
    }

    // 获取互斥锁
    acquireMutex();

    // Arduino 平台串口初始化
    if (_serialPort != nullptr) {
        // 检查串口是否已经初始化且波特率为115200
        bool needsInitialization = true;

        // 尝试检查当前波特率（ESP32平台支持）
#if defined(ESP32)
        // ESP32可以检查串口状态
        if (_serialPort->baudRate() == 115200) {
            needsInitialization = false;
            serialPrintln("Serial port already initialized at 115200 baud rate, skipping initialization.");
        }
#endif

        if (needsInitialization) {
            // 配置UART引脚（如果提供了有效的引脚号）
            if (_txPin != -1 && _rxPin != -1) {
                _serialPort->begin(115200, SERIAL_8N1, _rxPin, _txPin);  // 设置波特率和引脚
                serialPrintf("Arduino serial port initialized with TX pin %d, RX pin %d, baud rate 115200.\r\n", _txPin,
                             _rxPin);
            } else {
                _serialPort->begin(115200);  // 使用默认引脚设置波特率为115200
                serialPrintln("Arduino serial port initialized with default pins, baud rate 115200.");
            }
        } else {
            // 串口已初始化，但仍需要检查引脚配置
            if (_txPin != -1 && _rxPin != -1) {
                // 如果需要特定引脚，重新配置
                _serialPort->end();                                      // 先关闭当前配置
                _serialPort->begin(115200, SERIAL_8N1, _rxPin, _txPin);  // 重新设置引脚
                serialPrintf("Serial port reconfigured with TX pin %d, RX pin %d.\r\n", _txPin, _rxPin);
            } else {
                serialPrintln("Serial port already configured, using existing pin configuration.");
            }
        }

        _serialPort->setTimeout(1000);

        // 创建数据队列
        if (dataQueue == nullptr) {
            dataQueue = xQueueCreate(10, sizeof(uint8_t) * 256);
            if (dataQueue == nullptr) {
                serialPrintln("Failed to create data queue.");
                releaseMutex();
                return false;
            }
        }

        // 创建数据解析任务
        if (parseTaskHandle == nullptr) {
            xTaskCreate(parseDataTask, "ParseDataTask", 8096, this, 5, &parseTaskHandle);
            if (parseTaskHandle == nullptr) {
                serialPrintln("Failed to create parse task.");
                releaseMutex();
                return false;
            }
        }
    }

    // 释放互斥锁
    releaseMutex();

#endif

    serialPrintln("M5UnitFingerprint2 initialized successfully.");
    return true;
}

// 发送数据包
bool M5UnitFingerprint2::sendPacketData(const Fingerprint_Packet& packet)
{
    acquireMutex();

    // 构建要发送的数据
    uint8_t sendBuffer[FINGERPRINT_MAX_PACKET_SIZE + 10];  // 额外空间用于包头

    // 使用 Fingerprint_Packet 的序列化方法
    size_t packetSize = packet.serialize(sendBuffer, sizeof(sendBuffer));
    if (packetSize == 0) {
        releaseMutex();
        serialPrintln("Failed to serialize packet");
        return false;
    }

#if defined M5_MODULE_DEBUG_SERIAL
// 打印发送包的调试信息
#if M5_MODULE_DEBUG_SERIAL_ENABLED
    FingerprintDebugUtils::printPacketHex("TX: ", sendBuffer, packetSize);
#endif
#if M5_MODULE_COMMAND_PARSING_ENABLED
    serialPrintln("=== SENDING PACKET ===");
    FingerprintDebugUtils::printPacketStructure("TX: ", packet);
#endif
#endif

    // 发送数据
    size_t bytesWritten = writeSerial(sendBuffer, packetSize);

    releaseMutex();

#if defined M5_MODULE_DEBUG_SERIAL
    serialPrintf("Sent %d bytes", bytesWritten);
#if !defined(M5_MODULE_DEBUG_SERIAL_ENABLED) && !defined(M5_MODULE_COMMAND_PARSING_ENABLED)
    M5_MODULE_DEBUG_SERIAL.print(": ");
    if (packetSize > 16) {
        // 如果数据超过16字节，显示包头（前9字节）+ 数据前4字节 + ... + 校验和（最后2字节）
        // 显示包头（前9字节：起始码+地址+包类型+包长度）
        for (size_t i = 0; i < 9 && i < packetSize; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X ", sendBuffer[i]);
        }
        // 显示数据部分的前4字节
        for (size_t i = 9; i < 13 && i < packetSize - 2; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X ", sendBuffer[i]);
        }
        // 如果还有更多数据，显示省略号
        if (packetSize > 15) {
            M5_MODULE_DEBUG_SERIAL.printf("... ");
        }
        // 显示最后2字节（校验和）
        for (size_t i = packetSize - 2; i < packetSize; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X ", sendBuffer[i]);
        }
    } else {
        // 数据不超过16字节，正常显示所有数据
        for (size_t i = 0; i < packetSize; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X ", sendBuffer[i]);
        }
    }
#endif
    M5_MODULE_DEBUG_SERIAL.println("");
#endif
    return (bytesWritten == packetSize);
}

// 接收数据包 - 使用新的解析队列机制
bool M5UnitFingerprint2::receivePacketData(Fingerprint_Packet& packet, uint32_t timeout_ms)
{
    acquireMutex();

    // 设置等待状态
    _waitingForResponse = true;
    _packetReceived = false;
    _expectedPacket = &packet;

    releaseMutex();

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    // 创建响应信号量（如果还没有创建）
    if (responseSemaphore == nullptr) {
        responseSemaphore = xSemaphoreCreateBinary();
        if (responseSemaphore == nullptr) {
            serialPrintln("Failed to create response semaphore");
            _waitingForResponse = false;
            return false;
        }
    }

    // 等待响应信号量
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    if (xSemaphoreTake(responseSemaphore, timeout_ticks) == pdTRUE) {
        // 成功接收到响应
        acquireMutex();
        bool success = _packetReceived;
        
        _waitingForResponse = false;
        _expectedPacket = nullptr;
        releaseMutex();

        if (success) {
#if defined M5_MODULE_DEBUG_SERIAL
#if M5_MODULE_COMMAND_PARSING_ENABLED
            serialPrintln("=== RECEIVED PACKET ===");
            FingerprintDebugUtils::printPacketStructure("RX ", packet);
            serialPrintln("Successfully received response packet");
#endif
#endif
            return true;
        } else {
            serialPrintln("Response received but parsing failed");
            return false;
        }
    } else {
        // 超时
        acquireMutex();
        _waitingForResponse = false;
        _expectedPacket = nullptr;
        releaseMutex();

        serialPrintln("Timeout waiting for response packet");
        return false;
    }
#else
    // 非 FreeRTOS 平台，使用简单的轮询
    unsigned long startTime = millis();
    while (millis() - startTime < timeout_ms) {
        // 主动检查解析队列
        acquireMutex();
        if (getMatchingPacket(packet)) {
            _waitingForResponse = false;
            _expectedPacket = nullptr;
            _packetReceived = true;
            releaseMutex();
            
#if defined M5_MODULE_DEBUG_SERIAL
            serialPrintln("=== RECEIVED PACKET ===");
#if M5_MODULE_COMMAND_PARSING_ENABLED
            FingerprintDebugUtils::printPacketStructure("RX ", packet);
#endif
            serialPrintln("Successfully received response packet");
#endif
            return true;
        }
        releaseMutex();
        
        delay(1);
    }

    _waitingForResponse = false;
    _expectedPacket = nullptr;
    serialPrintln("Timeout waiting for response packet");
    return false;
#endif
}

// 设置唤醒回调函数
void M5UnitFingerprint2::setWakeupCallback(PS_WakeupCallback_t callback)
{
    acquireMutex();
    _wakeupCallback = callback;
    releaseMutex();
}

// 获取当前设置的唤醒回调函数
PS_WakeupCallback_t M5UnitFingerprint2::getWakeupCallback() const
{
    return _wakeupCallback;
}

// 检查是否为唤醒包并处理
bool M5UnitFingerprint2::handleWakeupPacket(const uint8_t* packetData, size_t packetLength)
{
    // 唤醒包的固定格式：EF 01 FF FF FF FF 07 00 03 FF 01 09
    // 总长度应该是12字节
    if (packetLength != 12) {
        return false;
    }
    
    // 检查唤醒包的特征：起始码(EF 01) + 地址(FF FF FF FF) + 包类型(07) + 长度(00 03) + 数据(FF 01) + 校验和(09)
    const uint8_t wakeupPattern[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x03, 0xFF, 0x01, 0x09};
    
    // 逐字节比较
    for (size_t i = 0; i < 12; i++) {
        if (packetData[i] != wakeupPattern[i]) {
            return false;
        }
    }
    
    // 确认是唤醒包，调用回调函数
#if defined M5_MODULE_DEBUG_SERIAL
    serialPrintln("Wakeup packet detected, calling callback function");
#endif
    
    // 如果用户设置了自定义回调，调用用户回调；否则调用默认回调
    if (_wakeupCallback != nullptr) {
        _wakeupCallback(packetData, packetLength);
    } else {
        defaultWakeupCallback(packetData, packetLength);
    }
    
    return true;
}

// 默认唤醒回调函数
void M5UnitFingerprint2::defaultWakeupCallback(const uint8_t* wakeupPacket, size_t packetLength)
{
#if defined M5_MODULE_DEBUG_SERIAL
    serialPrintf("=== WAKEUP PACKET RECEIVED ===\r\n");
    serialPrintf("Fingerprint module wakeup detected, packet length: %d bytes\r\n", (int)packetLength);
    serialPrintf("Wakeup packet data: ");
    for (size_t i = 0; i < packetLength; i++) {
        M5_MODULE_DEBUG_SERIAL.printf("%02X ", wakeupPacket[i]);
    }
    M5_MODULE_DEBUG_SERIAL.println("");
    serialPrintf("==============================\r\n");
#else
    // 如果没有调试串口，仍然可以通过默认串口打印基本信息
    Serial.println("Fingerprint module wakeup detected!");
#endif
}

/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "m5_unit_fingerprint2.hpp"
#include <string.h>  // 为了使用 memcpy 和 memset

//指纹模块操作

// 验证用获取图像
fingerprint_status_t M5UnitFingerprint2::PS_GetImage(void) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_IMAGE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetImage command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetImage response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetImage");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_GetImage");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Get image result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 注册用获取图像
fingerprint_status_t M5UnitFingerprint2::PS_GetEnrollImage(void) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_ENROLL_IMAGE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetEnrollImage command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetEnrollImage response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetEnrollImage");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_GetEnrollImage");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Get enroll image result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 生成特征文件
fingerprint_status_t M5UnitFingerprint2::PS_GenChar(uint8_t BufferID) const
{
    // 创建命令数据包
    uint8_t params[] = {BufferID};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GENERATE_CHARACTER, params, 1);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GenChar command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GenChar response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GenChar");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_GenChar");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Generate character file in BufferID %d result: %s [confirmation: %s]\r\n", BufferID,
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 精确比对两枚指纹特征
fingerprint_status_t M5UnitFingerprint2::PS_Match(uint16_t& compareScores) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_MATCH, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_Match command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_Match response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_Match");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 3) {
        serialPrintln("Invalid response data length for PS_Match");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];
    compareScores               = (responseData[1] << 8) | responseData[2];

    serialPrintf("Match fingerprint result: %s, score: %d [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed", compareScores,
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 搜索指纹
fingerprint_status_t M5UnitFingerprint2::PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, uint16_t& PageID, uint16_t& MatchScore) const
{
    // 验证BufferID范围 (通常为1或2)
    if (BufferID < 1 || BufferID > 2) {
        serialPrintln("Invalid BufferID. Range: 1-2");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 创建命令数据包
    uint8_t params[] = {BufferID, 
                        static_cast<uint8_t>((StartPage >> 8) & 0xFF), static_cast<uint8_t>(StartPage & 0xFF),
                        static_cast<uint8_t>((PageNum >> 8) & 0xFF), static_cast<uint8_t>(PageNum & 0xFF)};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_SEARCH, params, 5);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_Search command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_Search response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_Search");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_Search");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];
    
    if (confirmationCode == FINGERPRINT_OK && responsePacket.get_actual_data_length() >= 5) {
        PageID = (responseData[1] << 8) | responseData[2];
        MatchScore = (responseData[3] << 8) | responseData[4];
        serialPrintf("Search fingerprint result: Success, PageID: %d, Score: %d [confirmation: %s]\r\n", 
                     PageID, MatchScore, FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
    } else {
        PageID = 0;
        MatchScore = 0;
        serialPrintf("Search fingerprint result: Failed [confirmation: %s]\r\n",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
    }

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 合并特征文件生成模板
fingerprint_status_t M5UnitFingerprint2::PS_RegModel(void) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_REG_MODEL, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_RegModel command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_RegModel response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_RegModel");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_RegModel");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];

    serialPrintf("Register model result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 储存模板
fingerprint_status_t M5UnitFingerprint2::PS_StoreChar(uint8_t BufferID, uint16_t PageID) const
{
    // 验证BufferID范围 (通常为1或2)
    if (BufferID < 1 || BufferID > 2) {
        serialPrintln("Invalid BufferID. Range: 1-2");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 创建命令数据包
    uint8_t params[] = {BufferID, static_cast<uint8_t>((PageID >> 8) & 0xFF), static_cast<uint8_t>(PageID & 0xFF)};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_STORE, params, 3);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_StoreChar command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_StoreChar response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_StoreChar");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_StoreChar");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];

    serialPrintf("Store character in BufferID %d to PageID %d result: %s [confirmation: %s]\r\n", 
                 BufferID, PageID,
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 读取模板
fingerprint_status_t M5UnitFingerprint2::PS_LoadChar(uint8_t BufferID, uint16_t PageID) const
{
    // 验证BufferID范围 (通常为1或2)
    if (BufferID < 1 || BufferID > 2) {
        serialPrintln("Invalid BufferID. Range: 1-2");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 创建命令数据包
    uint8_t params[] = {BufferID, static_cast<uint8_t>((PageID >> 8) & 0xFF), static_cast<uint8_t>(PageID & 0xFF)};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_LOAD_MODEL, params, 3);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_LoadChar command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_LoadChar response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_LoadChar");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_LoadChar");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];

    serialPrintf("Load character from PageID %d to BufferID %d result: %s [confirmation: %s]\r\n", 
                 PageID, BufferID,
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 上传图像（带图像数据返回）- 从指纹模块上传原始图像数据到主控并返回给用户
fingerprint_status_t M5UnitFingerprint2::PS_UpImage(uint8_t* imageBuffer, uint32_t bufferSize, uint32_t& actualImageSize) const
{
    // 参数检查
    if (imageBuffer == nullptr || bufferSize == 0) {
        serialPrintln("Invalid parameters for PS_UpImage");
        return FINGERPRINT_PARAM_ERROR;
    }

    actualImageSize = 0; // 初始化实际图像大小

    // 创建上传图像命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_UPLOAD_IMAGE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_UpImage command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收ACK响应包
    Fingerprint_Packet ackPacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(ackPacket, 10000)) {
        serialPrintln("Failed to receive PS_UpImage ACK response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查ACK响应包类型
    if (ackPacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid ACK packet type for PS_UpImage");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查ACK数据长度
    if (ackPacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid ACK data length for PS_UpImage");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取ACK响应数据
    const uint8_t* ackData = ackPacket.get_data();
    uint8_t confirmationCode = ackData[0];

    // 检查命令执行状态
    if (confirmationCode != FINGERPRINT_OK) {
        serialPrintf("PS_UpImage command failed [confirmation: %s]\r\n",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
        return static_cast<fingerprint_status_t>(confirmationCode);
    }

    serialPrintln("PS_UpImage ACK received, starting to receive image data packets...");

    // 开始接收图像数据包
    uint32_t totalPacketsReceived = 0;
    uint32_t totalBytesReceived = 0;
    bool receivingComplete = false;

    while (!receivingComplete) {
        // 接收数据包（可能是DATAPACKET或ENDDATAPACKET）
        Fingerprint_Packet dataPacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_DATAPACKET, nullptr, 0);
        if (!nonConstThis->receivePacketData(dataPacket, 10000)) {
            serialPrintf("Failed to receive image data packet #%d\r\n", totalPacketsReceived + 1);
            return FINGERPRINT_PACKET_TIMEOUT;
        }

        // 检查数据包类型
        uint8_t packetType = dataPacket.get_type();
        if (packetType == FINGERPRINT_PACKET_DATAPACKET) {
            // 普通数据包，继续接收
            serialPrintf("Received data packet #%d, size: %d bytes\r\n", 
                        totalPacketsReceived + 1, dataPacket.get_actual_data_length());
        } else if (packetType == FINGERPRINT_PACKET_ENDDATAPACKET) {
            // 结束数据包，这是最后一个包
            serialPrintf("Received end data packet #%d, size: %d bytes\r\n", 
                        totalPacketsReceived + 1, dataPacket.get_actual_data_length());
            receivingComplete = true;
        } else {
            serialPrintf("Invalid data packet type: 0x%02X\r\n", packetType);
            return FINGERPRINT_PACKET_BADPACKET;
        }

        // 将当前包的数据添加到图像数据中
        const uint8_t* packetData = dataPacket.get_data();
        uint16_t packetDataLength = dataPacket.get_actual_data_length();
        
        if (packetDataLength > 0) {
            // 检查用户提供的缓冲区是否有足够空间
            if (totalBytesReceived + packetDataLength > bufferSize) {
                serialPrintf("Image data exceeds provided buffer size (%d bytes)\r\n", bufferSize);
                return FINGERPRINT_PACKET_OVERFLOW;
            }
            
            // 复制数据到用户提供的图像缓冲区
            memcpy(&imageBuffer[totalBytesReceived], packetData, packetDataLength);
            totalBytesReceived += packetDataLength;
        }

        totalPacketsReceived++;

        // 防止无限循环，设置最大包数限制
        if (totalPacketsReceived > 1000) {
            serialPrintln("Warning: Received too many packets, stopping...");
            break;
        }
    }

    // 设置实际接收的图像大小
    actualImageSize = totalBytesReceived;

    serialPrintf("Image upload completed successfully!\r\n");
    serialPrintf("Total packets received: %d\r\n", totalPacketsReceived);
    serialPrintf("Total image data size: %d bytes\r\n", actualImageSize);

// #if defined M5_MODULE_DEBUG_SERIAL
//     // 可选：打印图像数据的前几个字节作为验证
//     if (actualImageSize > 0) {
//         serialPrint("Image data header (first 16 bytes): ");
//         for (int i = 0; i < 16 && i < actualImageSize; i++) {
//             M5_MODULE_DEBUG_SERIAL.printf("%02X ", imageBuffer[i]);
//         }
//         M5_MODULE_DEBUG_SERIAL.println("");
//     }
// #endif

    serialPrintln("PS_UpImage operation with data return completed successfully");
    return FINGERPRINT_OK;
}


// 删除模板 - 删除flash指纹库中的一个或多个模板文件
fingerprint_status_t M5UnitFingerprint2::PS_DeletChar(uint16_t PageID, uint16_t Num) const
{
    // 参数检查
    if (PageID > 999 || Num == 0 || Num > 1000) {
        serialPrintln("Invalid parameters for PS_DeletChar");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 准备命令参数
    uint8_t params[4];
    params[0] = (PageID >> 8) & 0xFF;  // PageID 高字节
    params[1] = PageID & 0xFF;         // PageID 低字节
    params[2] = (Num >> 8) & 0xFF;     // Num 高字节
    params[3] = Num & 0xFF;            // Num 低字节

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_DELETE_MODEL, params, 4);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_DeletChar command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 2000)) {  // 删除可能需要更长时间
        serialPrintln("Failed to receive PS_DeletChar response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_DeletChar");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_DeletChar");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Delete fingerprint templates result: %s [PageID: %d, Count: %d, confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed", PageID, Num,
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 清空指纹库 - 清空flash指纹库
fingerprint_status_t M5UnitFingerprint2::PS_Empty(void) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_EMPTY, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_Empty command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包 (清空操作可能需要更长时间)
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 5000)) {
        serialPrintln("Failed to receive PS_Empty response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_Empty");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_Empty");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Empty fingerprint library result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 写寄存器 - 写SOC系统寄存器
fingerprint_status_t M5UnitFingerprint2::PS_WriteReg(fingerprint_register_id_t RegID, uint8_t Value) const
{
    // 参数检查 - 根据文档，RegID范围为0-3, 5-13，禁用4
    if (RegID == 4 || RegID > 13) {
        serialPrintln("Invalid register ID for PS_WriteReg");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 准备命令参数
    uint8_t params[2];
    // RegID特殊转换：当RegID大于9时，转换为十进制表示（10->0x10, 11->0x11, etc.）
    if (RegID > 9) {
        params[0] = 0x10 + (RegID - 10);  // 10->0x10, 11->0x11, 12->0x12, 13->0x13
    } else {
        params[0] = static_cast<uint8_t>(RegID);  // 0-9直接转换
    }
    params[1] = Value;                        // 寄存器数据值

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_WRITE_REG, params, 2);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_WriteReg command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_WriteReg response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_WriteReg");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_WriteReg");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    // 计算实际发送的RegID值（用于调试输出）
    uint8_t actualRegIDValue = (RegID > 9) ? (0x10 + (RegID - 10)) : static_cast<uint8_t>(RegID);
    
    serialPrintf("Write register result: %s [RegID: %d->0x%02X, Value: 0x%02X, confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed", static_cast<uint8_t>(RegID), actualRegIDValue, Value,
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 读取系统参数 - 读取系统基本参数
fingerprint_status_t M5UnitFingerprint2::PS_ReadSysPara(PS_ReadSysPara_BasicParams &RawData) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_READ_SYSTEM_PARAM, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_ReadSysPara command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_ReadSysPara response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_ReadSysPara");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (1字节确认码 + 16字节系统参数)
    if (responsePacket.get_actual_data_length() < 17) {
        serialPrintln("Invalid response data length for PS_ReadSysPara");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

#ifdef M5_MODULE_DEBUG_SERIAL_ENABLED
    // 如果命令执行成功，解析16字节的系统参数
    if (confirmationCode == FINGERPRINT_OK) {
        // 确保有足够的数据长度
        if (responsePacket.get_actual_data_length() >= 17) {
            // 解析16字节系统参数 (从第1字节开始，跳过确认码)
            const uint8_t* paramData = &responseData[1];
            
            // 按照大端字节序解析各个参数
            RawData.status_register = (paramData[0] << 8) | paramData[1];           // 状态寄存器 (2字节)
            RawData.temp_size = (paramData[2] << 8) | paramData[3];                 // 指纹模板大小 (2字节)
            RawData.data_size = (paramData[4] << 8) | paramData[5];                 // 指纹库大小 (2字节)
            RawData.score_level = (paramData[6] << 8) | paramData[7];               // 安全等级 (2字节)
            RawData.device_addr = (paramData[8] << 24) | (paramData[9] << 16) |     // 设备地址 (4字节)
                                  (paramData[10] << 8) | paramData[11];
            RawData.packet_size = (paramData[12] << 8) | paramData[13];             // 数据包大小 (2字节)
            RawData.baud_rate = (paramData[14] << 8) | paramData[15];               // 串口波特率 (2字节)

            serialPrintf("Read system parameters successfully:\r\n");
            serialPrintf("  Status Register: 0x%04X\r\n", RawData.status_register);
            serialPrintf("  Template Size: %d\r\n", RawData.temp_size);
            serialPrintf("  Database Size: %d\r\n", RawData.data_size);
            serialPrintf("  Security Level: %d\r\n", RawData.score_level);
            serialPrintf("  Device Address: 0x%08X\r\n", RawData.device_addr);
            // 0:32 bytes, 1:64 bytes, 2:128 bytes, 3:256 bytes
            serialPrintf("  Packet Size: ");
            switch (RawData.packet_size) {
                case 0:
                    M5_MODULE_DEBUG_SERIAL.print("32 bytes");
                    break;
                case 1:
                    M5_MODULE_DEBUG_SERIAL.print("64 bytes");
                    break;
                case 2:
                    M5_MODULE_DEBUG_SERIAL.print("128 bytes");
                    break;
                case 3:
                    M5_MODULE_DEBUG_SERIAL.print("256 bytes");
                    break;
                default:
                    M5_MODULE_DEBUG_SERIAL.printf("Unknown (%d)\r\n", RawData.packet_size * 32);
            }
            M5_MODULE_DEBUG_SERIAL.println("");

            serialPrintf("  Baud Rate: %d\r\n", RawData.baud_rate * 9600);
        } else {
            serialPrintln("Insufficient data length for system parameters");
            return FINGERPRINT_PACKET_OVERFLOW;
        }
    }

#endif

    serialPrintf("Read system parameters result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 采样随机数 - 生成4字节随机数
fingerprint_status_t M5UnitFingerprint2::PS_GetRandomCode(uint32_t &RandomCode) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_RANDOM_CODE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetRandomCode command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetRandomCode response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetRandomCode");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (1字节确认码 + 4字节随机数)
    if (responsePacket.get_actual_data_length() < 5) {
        serialPrintln("Invalid response data length for PS_GetRandomCode");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    // 如果命令执行成功，解析4字节的随机数
    if (confirmationCode == FINGERPRINT_OK) {
        // 确保有足够的数据长度
        if (responsePacket.get_actual_data_length() >= 5) {
            // 解析4字节随机数 (从第1字节开始，跳过确认码)，大端字节序
            RandomCode = (responseData[1] << 24) | (responseData[2] << 16) | 
                         (responseData[3] << 8) | responseData[4];

            serialPrintf("Random code generated successfully: 0x%08X\r\n", RandomCode);
        } else {
            serialPrintln("Insufficient data length for random code");
            return FINGERPRINT_PACKET_OVERFLOW;
        }
    }

    serialPrintf("Get random code result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 读取INF页 - 读取FLASH Information Page内容（512字节）
fingerprint_status_t M5UnitFingerprint2::PS_ReadINFpage(uint8_t* INFData) const
{
    // 参数检查
    if (INFData == nullptr) {
        serialPrintln("Invalid parameter for PS_ReadINFpage: INFData is null");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_READ_INFO_PAGE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_ReadINFpage command");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 接收第一个响应包（ACK包）
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 2000)) {
        serialPrintln("Failed to receive PS_ReadINFpage ACK response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_ReadINFpage ACK");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查ACK包数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid ACK response data length for PS_ReadINFpage");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取确认码
    const uint8_t* ackData = responsePacket.get_data();
    uint8_t confirmationCode = ackData[0];

    if (confirmationCode != FINGERPRINT_OK) {
        serialPrintf("PS_ReadINFpage command failed [confirmation: %s]\r\n",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
        return static_cast<fingerprint_status_t>(confirmationCode);
    }

    // 命令执行成功，开始接收后续的数据包
    uint32_t totalBytesReceived = 0;
    uint32_t totalPacketsReceived = 0;
    const uint32_t expectedDataSize = 512; // INF页固定为512字节

    serialPrintln("Starting to receive INF page data packets...");

    while (totalBytesReceived < expectedDataSize) {
        // 接收数据包
        Fingerprint_Packet dataPacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_DATAPACKET, nullptr, 0);
        if (!nonConstThis->receivePacketData(dataPacket, 3000)) {
            serialPrintf("Failed to receive data packet %d for PS_ReadINFpage\r\n", totalPacketsReceived + 1);
            return FINGERPRINT_PACKET_TIMEOUT;
        }

        // 检查包类型（可能是数据包或结束数据包）
        uint8_t packetType = dataPacket.get_type();
        if (packetType != FINGERPRINT_PACKET_DATAPACKET && packetType != FINGERPRINT_PACKET_ENDDATAPACKET) {
            serialPrintf("Invalid data packet type for PS_ReadINFpage: 0x%02X\r\n", packetType);
            return FINGERPRINT_PACKET_BADPACKET;
        }

        // 获取数据内容
        const uint8_t* packetData = dataPacket.get_data();
        uint16_t packetDataLength = dataPacket.get_actual_data_length();

        if (packetDataLength > 0) {
            // 检查用户提供的缓冲区是否有足够空间
            if (totalBytesReceived + packetDataLength > expectedDataSize) {
                serialPrintf("INF data exceeds expected size (%d bytes)\r\n", expectedDataSize);
                return FINGERPRINT_PACKET_OVERFLOW;
            }

            // 复制数据到用户提供的缓冲区
            memcpy(&INFData[totalBytesReceived], packetData, packetDataLength);
            totalBytesReceived += packetDataLength;
        }

        totalPacketsReceived++;

        // 如果是结束数据包，停止接收
        if (packetType == FINGERPRINT_PACKET_ENDDATAPACKET) {
            serialPrintln("Received end data packet, stopping...");
            break;
        }

        // 防止无限循环，设置最大包数限制
        if (totalPacketsReceived > 50) {
            serialPrintln("Warning: Received too many packets, stopping...");
            break;
        }
    }

    serialPrintf("INF page read completed successfully!\r\n");
    serialPrintf("Total packets received: %d\r\n", totalPacketsReceived);
    serialPrintf("Total INF data size: %d bytes\r\n", totalBytesReceived);

#if defined M5_MODULE_DEBUG_SERIAL
    if (totalBytesReceived > 0) {
        serialPrint("INF data (128 bytes):");
        for (int i = 0; i < totalBytesReceived && i < 128; i++) {
            if (i % 16 == 0) {
                M5_MODULE_DEBUG_SERIAL.printf("\r\n%04X: ", i); // Print address offset
            }
            M5_MODULE_DEBUG_SERIAL.printf("%02X ", INFData[i]);
        }
        M5_MODULE_DEBUG_SERIAL.println("");
    }
#endif

    serialPrintln("PS_ReadINFpage operation completed successfully");
    return FINGERPRINT_OK;
}

// 写记事本 - 写入便笺数据到指定的便笺页
fingerprint_status_t M5UnitFingerprint2::PS_WriteNotepad(uint8_t NotepadID, const uint8_t* NotepadData, uint16_t NotepadLength) const
{
    // 参数检查
    if (NotepadData == nullptr) {
        serialPrintln("Invalid parameter for PS_WriteNotepad: NotepadData is null");
        return FINGERPRINT_PACKET_OVERFLOW;
    }
    
    if (NotepadID > 7) {
        serialPrintf("Invalid NotepadID for PS_WriteNotepad: %d (valid range: 0-7)\r\n", NotepadID);
        return FINGERPRINT_PACKET_OVERFLOW;
    }
    
    if (NotepadLength > 32) {
        serialPrintf("Invalid NotepadLength for PS_WriteNotepad: %d (max: 32 bytes)\r\n", NotepadLength);
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 准备命令参数：便笺号(1字节) + 便笺数据(最多32字节，不足补0)
    uint8_t commandParams[33] = {0}; // 1字节便笺号 + 32字节数据
    commandParams[0] = NotepadID;
    
    // 复制便笺数据，不足32字节的部分自动补0
    if (NotepadLength > 0) {
        memcpy(&commandParams[1], NotepadData, NotepadLength);
    }

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_WRITE_NOTEPAD, commandParams, 33);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_WriteNotepad command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_WriteNotepad response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_WriteNotepad");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (至少1字节确认码)
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_WriteNotepad");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];

    serialPrintf("Write notepad result: %s [confirmation: %s] NotepadID: %d\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str(),
                 NotepadID);

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 读记事本 - 读取指定便笺页的数据
fingerprint_status_t M5UnitFingerprint2::PS_ReadNotepad(uint8_t NotepadID, uint8_t* NotepadData) const
{
    // 参数检查
    if (NotepadData == nullptr) {
        serialPrintln("Invalid parameter for PS_ReadNotepad: NotepadData is null");
        return FINGERPRINT_PACKET_OVERFLOW;
    }
    
    if (NotepadID > 7) {
        serialPrintf("Invalid NotepadID for PS_ReadNotepad: %d (valid range: 0-7)\r\n", NotepadID);
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 准备命令参数：便笺号(1字节)
    uint8_t commandParams[1] = {NotepadID};

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_READ_NOTEPAD, commandParams, 1);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_ReadNotepad command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_ReadNotepad response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_ReadNotepad");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (1字节确认码 + 32字节便笺数据)
    if (responsePacket.get_actual_data_length() < 33) {
        serialPrintln("Invalid response data length for PS_ReadNotepad");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];

    // 如果命令执行成功，复制32字节的便笺数据
    if (confirmationCode == FINGERPRINT_OK) {
        memcpy(NotepadData, &responseData[1], 32);
        
        serialPrintf("Read notepad successful: NotepadID: %d\r\n", NotepadID);
#ifdef M5_MODULE_DEBUG_SERIAL_ENABLED
        serialPrint("Notepad data: ");
        for (int i = 0; i < 32; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X ", NotepadData[i]);
        }
        M5_MODULE_DEBUG_SERIAL.println("");
#endif
    }

    serialPrintf("Read notepad result: %s [confirmation: %s] NotepadID: %d\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str(),
                 NotepadID);

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 读有效模板个数 - 获取指纹库中已存储的有效模板数量
fingerprint_status_t M5UnitFingerprint2::PS_ValidTempleteNum(uint16_t &ValidNum) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_VALID_MODEL_COUNT, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_ValidTempleteNum command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_ValidTempleteNum response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_ValidTempleteNum");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (1字节确认码 + 2字节有效模板数量)
    if (responsePacket.get_actual_data_length() < 3) {
        serialPrintln("Invalid response data length for PS_ValidTempleteNum");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];

    // 如果命令执行成功，解析2字节的有效模板数量
    if (confirmationCode == FINGERPRINT_OK) {
        // 确保有足够的数据长度
        if (responsePacket.get_actual_data_length() >= 3) {
            // 解析2字节有效模板数量 (从第1字节开始，跳过确认码)，大端字节序
            ValidNum = (responseData[1] << 8) | responseData[2];

            serialPrintf("Valid template count: %d\r\n", ValidNum);
        } else {
            serialPrintln("Insufficient data length for valid template count");
            return FINGERPRINT_PACKET_OVERFLOW;
        }
    }

    serialPrintf("Get valid template count result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 读索引表 - 读取指纹库索引表，每1bit代表一个模板的状态
fingerprint_status_t M5UnitFingerprint2::PS_ReadIndexTable(uint8_t* IndexTableData) const
{
    // 参数检查
    if (IndexTableData == nullptr) {
        serialPrintln("Invalid parameter for PS_ReadIndexTable: IndexTableData is null");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // IndexTableID 固定为0
    uint8_t IndexTableID = 0;

    // 准备命令参数：索引表号(1字节)
    uint8_t commandParams[1] = {IndexTableID};

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_READ_INDEX_TABLE, commandParams, 1);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_ReadIndexTable command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_ReadIndexTable response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_ReadIndexTable");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (1字节确认码 + 32字节索引表数据)
    if (responsePacket.get_actual_data_length() < 33) {
        serialPrintln("Invalid response data length for PS_ReadIndexTable");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode = responseData[0];

    // 如果命令执行成功，复制32字节的索引表数据
    if (confirmationCode == FINGERPRINT_OK) {
        memcpy(IndexTableData, &responseData[1], 32);
        
        // 清零第100位后面的数据（从第100位开始，即第12字节的第4位开始）
        // 第100位是第12字节（索引11）的第4位（从高位开始计算）
        // 100位 = 12字节 + 4位，所以第12字节需要保留高4位，清零低4位
        IndexTableData[12] &= 0xF0;  // 保留高4位，清零低4位
        
        // 清零第13-31字节（索引13-31）
        for (int i = 13; i < 32; i++) {
            IndexTableData[i] = 0;
        }
#ifdef M5_MODULE_DEBUG_SERIAL_ENABLED
        serialPrintf("Read index table successful: IndexTableID: %d (fixed)\r\n", IndexTableID);
        
        // 计算模板范围（固定为0-99，只有100个有效位）
        uint16_t startTemplate = 0;
        uint16_t endTemplate = 99;
        
        serialPrintf("Template range: %d-%d (only 100 templates supported)\r\n", startTemplate, endTemplate);
        serialPrintln("Index table (1=enrolled, 0=empty):");
        
        // 显示有效的100位数据
        // 每行显示20位，共5行显示100位
        for (int row = 0; row < 5; row++) {
            uint16_t rowStartTemplate = row * 20;
            uint16_t rowEndTemplate = rowStartTemplate + 19;
            if (rowEndTemplate > 99) rowEndTemplate = 99;
            
            serialPrintf("Templates %02d-%02d: ", rowStartTemplate, rowEndTemplate);
            
            // 每行显示20位
            for (int bit = 0; bit < 20 && (row * 20 + bit) < 100; bit++) {
                int globalBitIndex = row * 20 + bit;
                int byteIndex = globalBitIndex / 8;
                int bitIndex = globalBitIndex % 8;  // 从最低位开始
                
                M5_MODULE_DEBUG_SERIAL.print((IndexTableData[byteIndex] & (1 << bitIndex)) ? "1" : "0");
                
                // 每5位加一个空格，方便阅读
                if ((bit + 1) % 5 == 0 && bit < 19) {
                    M5_MODULE_DEBUG_SERIAL.print(" ");
                }
            }
            M5_MODULE_DEBUG_SERIAL.println("");
        }
        
#endif
    }


    serialPrintf("Read index table result: %s [confirmation: %s] (IndexTableID fixed to 0)\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 获取芯片序列号 - 获取芯片唯一序列号
fingerprint_status_t M5UnitFingerprint2::PS_GetChipSN(uint8_t* ChipSN) const
{
    // 准备命令参数：预留(1字节)
    uint8_t commandParams[1] = {0};

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_CHIP_SN, commandParams, 1);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetChipSN command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetChipSN response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetChipSN");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (确认码1字节 + 芯片序列号32字节)
    if (responsePacket.get_actual_data_length() < 33) {
        serialPrintln("Invalid response data length for PS_GetChipSN");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    if (confirmationCode == FINGERPRINT_OK) {
        // 复制32字节芯片序列号
        if (ChipSN != nullptr) {
            memcpy(ChipSN, &responseData[1], 32);
        }
#ifdef M5_MODULE_DEBUG_SERIAL_ENABLED
        else {
            serialPrintln("Warning: ChipSN buffer is null, cannot copy chip serial number");
        }
        serialPrint("Get chip SN successful: ");
        for (int i = 0; i < 32; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X", responseData[1 + i]);
        }
        M5_MODULE_DEBUG_SERIAL.println("");
#endif
    }

    serialPrintf("Get chip SN result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 握手指令 - 检查模块是否正常工作
fingerprint_status_t M5UnitFingerprint2::PS_HandShake(void) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_HAND_SHAKE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_HandShake command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_HandShake response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_HandShake");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (只需要确认码1字节)
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_HandShake");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("HandShake result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 校验传感器 - 检查传感器是否正常工作
fingerprint_status_t M5UnitFingerprint2::PS_CheckSensor(void) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_CHECK_SENSOR, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_CheckSensor command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_CheckSensor response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_CheckSensor");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (至少要有确认码1字节)
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_CheckSensor");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Check sensor result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// LED控制灯指令 - 控制指纹模块的LED灯
fingerprint_status_t M5UnitFingerprint2::PS_ControlBLN(fingerprint_led_control_mode_t mode, fingerprint_led_color_t startColor, fingerprint_led_color_t endColor, uint8_t loopCount) const
{
    // 参数验证
    if (mode < FINGERPRINT_LED_BREATHING || mode > FINGERPRINT_LED_FADE_OUT) {
        serialPrintf("Invalid LED mode for PS_ControlBLN: %d (valid range: %d-%d)\r\n", 
                     mode, FINGERPRINT_LED_BREATHING, FINGERPRINT_LED_FADE_OUT);
        return FINGERPRINT_PACKET_OVERFLOW;
    }
    
    if (startColor > FINGERPRINT_LED_COLOR_WHITE) {
        serialPrintf("Invalid start color for PS_ControlBLN: %d (valid range: 0x00-0x07)\r\n", startColor);
        return FINGERPRINT_PACKET_OVERFLOW;
    }
    
    if (endColor > FINGERPRINT_LED_COLOR_WHITE) {
        serialPrintf("Invalid end color for PS_ControlBLN: %d (valid range: 0x00-0x07)\r\n", endColor);
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 准备命令参数：功能码(1字节) + 起始颜色(1字节) + 结束颜色(1字节) + 循环次数(1字节)
    uint8_t commandParams[4] = {
        static_cast<uint8_t>(mode),        // 功能码
        static_cast<uint8_t>(startColor),  // 起始颜色
        static_cast<uint8_t>(endColor),    // 结束颜色
        loopCount                          // 循环次数
    };

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_CONTROL_LED, commandParams, 4);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_ControlBLN command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_ControlBLN response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_ControlBLN");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (至少要有确认码1字节)
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_ControlBLN");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("LED control result: %s [confirmation: %s] Mode: %d, StartColor: 0x%02X, EndColor: 0x%02X, LoopCount: %d\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str(),
                 mode, startColor, endColor, loopCount);

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 获取图像信息指令 - 获取当前采集图像的面积和质量信息
fingerprint_status_t M5UnitFingerprint2::PS_GetImageInfo(uint8_t &imageArea, uint8_t &imageQuality) const
{
    // 初始化输出参数
    imageArea = 0;
    imageQuality = 0xFF; // 默认设为不合格

    // 创建命令数据包（无参数）
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_IMAGE_INFO, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetImageInfo command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetImageInfo response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetImageInfo");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (至少要有确认码1字节)
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_GetImageInfo");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    // 如果操作成功且有足够的数据，解析图像信息
    if (confirmationCode == FINGERPRINT_OK && responsePacket.get_actual_data_length() >= 3) {
        imageArea = responseData[1];    // 图像面积（百分比）
        imageQuality = responseData[2]; // 图像质量（0:合格，其他：不合格）
        
        serialPrintf("Get image info result: Success, Area: %d%%, Quality: %s [confirmation: %s]\r\n",
                     imageArea,
                     (imageQuality == 0) ? "Good" : "Poor",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
    } else {
        serialPrintf("Get image info result: Failed [confirmation: %s]\r\n",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
    }

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 搜索当前指纹指令 - 搜索当前采集的指纹并返回匹配的模板信息
fingerprint_status_t M5UnitFingerprint2::PS_SearchNow(uint16_t StartPage, uint16_t PageNum, uint16_t &PageID, uint16_t &MatchScore) const
{
    // 初始化输出参数
    PageID = 0;
    MatchScore = 0;

    // 参数验证
    if (PageNum == 0) {
        serialPrintln("Invalid PageNum for PS_SearchNow: must be greater than 0");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 创建命令数据包：起始页面(2字节) + 页数(2字节)
    uint8_t commandParams[4] = {
        static_cast<uint8_t>((StartPage >> 8) & 0xFF), static_cast<uint8_t>(StartPage & 0xFF),  // 起始页面
        static_cast<uint8_t>((PageNum >> 8) & 0xFF), static_cast<uint8_t>(PageNum & 0xFF)       // 搜索页数
    };

    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_SEARCH_NOW, commandParams, 4);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_SearchNow command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_SearchNow response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_SearchNow");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度 (至少要有确认码1字节)
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_SearchNow");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    // 如果搜索成功且有足够的数据，解析匹配信息
    if (confirmationCode == FINGERPRINT_OK && responsePacket.get_actual_data_length() >= 5) {
        PageID = (responseData[1] << 8) | responseData[2];     // 匹配的页码ID
        MatchScore = (responseData[3] << 8) | responseData[4]; // 匹配得分
        
        serialPrintf("Search current fingerprint result: Success, PageID: %d, Score: %d [confirmation: %s]\r\n",
                     PageID, MatchScore, FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
    } else {
        serialPrintf("Search current fingerprint result: Failed [confirmation: %s]\r\n",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
    }

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 取消指令 - 取消当前正在执行的自动注册或自动验证操作
fingerprint_status_t M5UnitFingerprint2::PS_Cancel(void) const
{
    // 创建命令数据包 - 使用命令代码 0x30
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_CANCEL_AUTO_FLOW, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_Cancel command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_Cancel response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_Cancel");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_Cancel");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Cancel auto operation result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}


// 特殊上传模板 - 从指纹模块上传模板数据到主控
fingerprint_status_t M5UnitFingerprint2::PS_UploadTemplate(uint16_t offset, uint16_t uploadSize, uint16_t &actualSize, uint8_t* templateData) const
{
    // 参数检查
    if (templateData == nullptr || uploadSize == 0) { 
        serialPrintln("Invalid parameters for PS_UploadTemplate");
        return FINGERPRINT_PARAM_ERROR;
    }

    actualSize = 0; // 初始化实际模板大小

    // 创建命令数据包 - 参数：偏移地址(2字节) + 上传大小(2字节)
    uint8_t commandData[4];
    commandData[0] = (offset >> 8) & 0xFF;
    commandData[1] = offset & 0xFF;
    commandData[2] = (uploadSize >> 8) & 0xFF;
    commandData[3] = uploadSize & 0xFF;

    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_UP_TEMPLATE, commandData, 4);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_UploadTemplate command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收ACK响应包
    Fingerprint_Packet ackPacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(ackPacket, 10000)) {
        serialPrintln("Failed to receive PS_UploadTemplate ACK response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查ACK响应包类型
    if (ackPacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid ACK packet type for PS_UploadTemplate");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查ACK数据长度（至少要有确认码1字节 + 实际大小2字节）
    if (ackPacket.get_actual_data_length() < 3) {
        serialPrintln("Invalid ACK data length for PS_UploadTemplate");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取ACK响应数据
    const uint8_t* ackData = ackPacket.get_data();
    uint8_t confirmationCode = ackData[0];
    actualSize = (ackData[1] << 8) | ackData[2]; // 实际的上传模板大小

    // 检查命令执行状态
    if (confirmationCode != FINGERPRINT_OK) {
        serialPrintf("PS_UploadTemplate command failed [confirmation: %s]\r\n",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
        // return static_cast<fingerprint_status_t>(confirmationCode);
    }

    // 如果实际大小为0，表示没有更多数据
    if (actualSize == 0) {
        serialPrintln("PS_UploadTemplate: No template data available");
        return FINGERPRINT_OK;
    }

    // 检查ACK包中是否包含模板数据（从第4个字节开始）
    uint16_t availableDataLength = ackPacket.get_actual_data_length() - 3; // 减去确认码和大小字段
    if (availableDataLength > 0) {
        // ACK包中包含模板数据
        serialPrintf("PS_UploadTemplate: Found %d bytes of template data in ACK packet\r\n", availableDataLength);
        
        // 检查数据长度是否与声明的大小一致
        if (availableDataLength > actualSize) {
            serialPrintf("Template data size mismatch: declared %d bytes, available %d bytes\r\n", actualSize, availableDataLength);
            availableDataLength = actualSize; // 只复制声明的大小
        }
        
        // 复制模板数据到用户提供的缓冲区
        memcpy(templateData, &ackData[3], availableDataLength);
        actualSize = availableDataLength; // 更新实际接收的大小
        
        serialPrintf("Template upload completed successfully!\r\n");
        serialPrintf("Template data size: %d bytes\r\n", actualSize);
        
#if defined M5_MODULE_DEBUG_SERIAL
        // 可选：打印模板数据的前几个字节作为验证
        if (actualSize > 0) {
            serialPrint("Template data header (first 16 bytes): ");
            for (int i = 0; i < 16 && i < actualSize; i++) {
                M5_MODULE_DEBUG_SERIAL.printf("%02X ", templateData[i]);
            }
            M5_MODULE_DEBUG_SERIAL.println("");
        }
#endif
    } else {
        serialPrintln("PS_UploadTemplate: No template data found in ACK packet");
        actualSize = 0;
    }

    serialPrintln("PS_UploadTemplate operation completed successfully");
    return FINGERPRINT_OK;
}

// 特殊下载模板 - 从主控下载模板数据到指纹模块
fingerprint_status_t M5UnitFingerprint2::PS_DownloadTemplate(uint16_t offset, uint16_t downloadSize, const uint8_t* templateData) const
{
    // 参数检查
    if (templateData == nullptr || downloadSize == 0) {
        serialPrintln("Invalid parameters for PS_DownloadTemplate");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 创建命令数据包 - 参数：偏移地址(2字节) + 下载大小(2字节) + 模板数据
    uint8_t* commandData = new uint8_t[4 + downloadSize];
    commandData[0] = (offset >> 8) & 0xFF;
    commandData[1] = offset & 0xFF;
    commandData[2] = (downloadSize >> 8) & 0xFF;
    commandData[3] = downloadSize & 0xFF;
    
    // 将模板数据复制到命令数据包中
    memcpy(&commandData[4], templateData, downloadSize);

    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_DOWN_TEMPLATE, commandData, 4 + downloadSize);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_DownloadTemplate command");
        delete[] commandData;
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 释放内存
    delete[] commandData;

    // 接收ACK响应包
    Fingerprint_Packet ackPacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(ackPacket, 10000)) {
        serialPrintln("Failed to receive PS_DownloadTemplate ACK response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查ACK响应包类型
    if (ackPacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid ACK packet type for PS_DownloadTemplate");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查ACK数据长度（至少要有确认码1字节）
    if (ackPacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid ACK data length for PS_DownloadTemplate");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取ACK响应数据
    const uint8_t* ackData = ackPacket.get_data();
    uint8_t confirmationCode = ackData[0];

    // 检查命令执行状态
    if (confirmationCode != FINGERPRINT_OK) {
        serialPrintf("PS_DownloadTemplate command failed [confirmation: %s]\r\n",
                     FingerprintDebugUtils::getStatusName(confirmationCode).c_str());
        return static_cast<fingerprint_status_t>(confirmationCode);
    }

    serialPrintf("PS_DownloadTemplate completed successfully!\r\n");
    serialPrintf("Template data size: %d bytes\r\n", downloadSize);

#if defined M5_MODULE_DEBUG_SERIAL
    // 可选：打印模板数据的前几个字节作为验证
    if (downloadSize > 0) {
        serialPrint("Template data header (first 16 bytes): ");
        for (int i = 0; i < 16 && i < downloadSize; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X ", templateData[i]);
        }
        M5_MODULE_DEBUG_SERIAL.println("");
    }
#endif

    serialPrintln("PS_DownloadTemplate operation completed successfully");
    return FINGERPRINT_OK;
}

// 特殊下载模板自动版本 - 自动循环下载完整模板数据
fingerprint_status_t M5UnitFingerprint2::PS_DownloadTemplateAuto(
    const uint8_t* templateBuffer,  // 模板缓冲区
    uint32_t actualSize,            // 实际下载的模板大小
    PS_DownloadTemplateCallback_t callback // 每发送一段数据时的回调
) const
{
    // 参数检查
    if (templateBuffer == nullptr || actualSize == 0) {
        serialPrintln("Invalid parameters for PS_DownloadTemplateAuto");
        return FINGERPRINT_PARAM_ERROR;
    }

    uint16_t offset = 0;               // 起始偏移地址
    uint16_t downloadSize = 100;       // 每次发送的下载大小
    uint32_t totalSize = 0;            // 累计发送的总大小
    int packetCount = 0;               // 数据包计数器
    bool downloadComplete = false;     // 下载完成标志
    bool callbackAborted = false;      // 回调中止标志
    
    serialPrintln("PS_DownloadTemplateAuto: Starting automatic template download...");
    serialPrintf("Template size: %d bytes, download packet size: %d bytes\r\n", actualSize, downloadSize);
    
    // 循环发送所有模板数据包
    while (!downloadComplete && !callbackAborted) {
        packetCount++;
        
        // 计算当前包的实际发送大小
        uint16_t currentDownloadSize = downloadSize;
        if (totalSize + downloadSize > actualSize) {
            // 最后一个包，调整大小
            currentDownloadSize = actualSize - totalSize;
        }
        
        // 检查是否是最后一个数据包
        if (totalSize + currentDownloadSize >= actualSize) {
            downloadComplete = true;
        }
        
        // 调用原始的PS_DownloadTemplate函数发送一个数据包
        fingerprint_status_t result = PS_DownloadTemplate(offset, currentDownloadSize, 
                                                          templateBuffer + totalSize);
        
        if (result != FINGERPRINT_OK) {
            serialPrintf("PS_DownloadTemplateAuto: Download failed at packet #%d (offset %d), error: %s\r\n", 
                        packetCount, offset, 
                        FingerprintDebugUtils::getStatusName(result).c_str());
            return result;
        }
        
        // 打印当前包信息
        serialPrintf("PS_DownloadTemplateAuto: Packet #%d - offset=%d, sent=%d bytes\r\n",
                    packetCount, offset, currentDownloadSize);
        
        // 更新偏移量和总大小
        offset += currentDownloadSize;
        totalSize += currentDownloadSize;
        
        // 调用进度回调函数（如果提供）
        if (callback != nullptr) {
            bool continueDownload = callback(currentDownloadSize, totalSize, packetCount, downloadComplete);
            if (!continueDownload) {
                serialPrintln("PS_DownloadTemplateAuto: Download aborted by callback function");
                callbackAborted = true;
                return FINGERPRINT_OPERATION_BLOCKED;
            }
        }
        
        // 防止无限循环的安全检查
        if (packetCount > 1000) {
            serialPrintln("PS_DownloadTemplateAuto: Too many packets sent, stopping for safety");
            return FINGERPRINT_PACKET_OVERFLOW;
        }
    }
    
    if (downloadComplete && !callbackAborted) {
        serialPrintf("PS_DownloadTemplateAuto: Download completed successfully!\r\n");
        serialPrintf("Total packets sent: %d\r\n", packetCount);
        serialPrintf("Total template data size: %d bytes\r\n", totalSize);
        
        // 最后一次调用回调函数通知完成
        if (callback != nullptr) {
            callback(0, totalSize, packetCount, true);
        }
        
#if defined M5_MODULE_DEBUG_SERIAL
        // 显示模板数据预览
        if (totalSize > 0) {
            serialPrint("Template data preview (first 16 bytes): ");
            for (int i = 0; i < 16 && i < totalSize; i++) {
                M5_MODULE_DEBUG_SERIAL.printf("%02X ", templateBuffer[i]);
            }
            M5_MODULE_DEBUG_SERIAL.println("");
        }
#endif
        
        return FINGERPRINT_OK;
    } else if (callbackAborted) {
        serialPrintf("PS_DownloadTemplateAuto: Download aborted by callback after %d packets, %d bytes sent\r\n",
                    packetCount, totalSize);
        return FINGERPRINT_OPERATION_BLOCKED;
    } else {
        serialPrintf("PS_DownloadTemplateAuto: Download incomplete, %d bytes sent\r\n", totalSize);
        return FINGERPRINT_TIMEOUT;
    }
}

// 特殊上传模板自动版本 - 自动循环上传完整模板数据
fingerprint_status_t M5UnitFingerprint2::PS_UploadTemplateAuto(
    uint8_t* templateBuffer,        // 上传缓冲区
    uint32_t bufferSize,            // 缓冲区大小
    uint32_t& actualSize,           // 返回的实际上传大小
    PS_UploadTemplateCallback_t callback // 每接收一段数据时的回调
) const
{
    // 参数检查
    if (templateBuffer == nullptr || bufferSize == 0) {
        serialPrintln("Invalid parameters for PS_UploadTemplateAuto");
        return FINGERPRINT_PARAM_ERROR;
    }

    actualSize = 0; // 初始化实际上传大小
    
    uint16_t offset = 0;               // 起始偏移地址
    uint16_t uploadSize = 100;         // 每次请求的上传大小
    uint16_t currentActualSize = 0;    // 当前包的实际大小
    uint32_t totalSize = 0;            // 累计接收的总大小
    int packetCount = 0;               // 数据包计数器
    bool uploadComplete = false;       // 上传完成标志
    bool callbackAborted = false;      // 回调中止标志
    
    serialPrintln("PS_UploadTemplateAuto: Starting automatic template upload...");
    serialPrintf("Buffer size: %d bytes, upload packet size: %d bytes\r\n", bufferSize, uploadSize);
    
    // 清空缓冲区
    memset(templateBuffer, 0, bufferSize);
    
    // 循环接收所有模板数据包
    while (!uploadComplete && !callbackAborted) {
        // 检查缓冲区是否足够容纳下一个数据包
        if (totalSize + uploadSize > bufferSize) {
            serialPrintf("PS_UploadTemplateAuto: Buffer size insufficient. Required: %d, Available: %d\r\n", 
                        totalSize + uploadSize, bufferSize);
            return FINGERPRINT_PACKET_OVERFLOW;
        }
        
        packetCount++;
        
        // 调用原始的PS_UploadTemplate函数获取一个数据包
        fingerprint_status_t result = PS_UploadTemplate(offset, uploadSize, currentActualSize, 
                                                        templateBuffer + totalSize);
        
        if (result != FINGERPRINT_OK) {
            serialPrintf("PS_UploadTemplateAuto: Upload failed at packet #%d (offset %d), error: %s\r\n", 
                        packetCount, offset, 
                        FingerprintDebugUtils::getStatusName(result).c_str());
            actualSize = totalSize; // 返回已经接收到的数据大小
            return result;
        }
        
        // 打印当前包信息
        serialPrintf("PS_UploadTemplateAuto: Packet #%d - offset=%d, requested=%d, received=%d bytes\r\n",
                    packetCount, offset, uploadSize, currentActualSize);
        
        // 检查是否是最后一个数据包
        if (currentActualSize == 0) {
            // 实际接收大小为0，表示模板数据传输完成
            serialPrintln("PS_UploadTemplateAuto: Template upload completed (actualSize = 0)");
            uploadComplete = true;
        } else {
            // 更新偏移量和总大小
            offset += currentActualSize;
            totalSize += currentActualSize;
            
            // 调用进度回调函数（如果提供）
            if (callback != nullptr) {
                bool continueUpload = callback(currentActualSize, totalSize, packetCount, uploadComplete);
                if (!continueUpload) {
                    serialPrintln("PS_UploadTemplateAuto: Upload aborted by callback function");
                    callbackAborted = true;
                    actualSize = totalSize;
                    return FINGERPRINT_OPERATION_BLOCKED;
                }
            }
        }
        
        // 防止无限循环的安全检查
        if (packetCount > 1000) {
            serialPrintln("PS_UploadTemplateAuto: Too many packets received, stopping for safety");
            actualSize = totalSize;
            return FINGERPRINT_PACKET_OVERFLOW;
        }
        
        // 如果缓冲区即将满，提前结束以避免溢出
        if (totalSize >= bufferSize - uploadSize) {
            serialPrintln("PS_UploadTemplateAuto: Buffer nearly full, ending upload");
            uploadComplete = true;
        }
    }
    
    // 设置最终的实际大小
    actualSize = totalSize;
    
    if (uploadComplete && !callbackAborted) {
        serialPrintf("PS_UploadTemplateAuto: Upload completed successfully!\r\n");
        serialPrintf("Total packets received: %d\r\n", packetCount);
        serialPrintf("Total template data size: %d bytes\r\n", actualSize);
        
        // 最后一次调用回调函数通知完成
        if (callback != nullptr) {
            callback(0, actualSize, packetCount, true);
        }
        
#if defined M5_MODULE_DEBUG_SERIAL
        // 显示模板数据预览
        if (actualSize > 0) {
            serialPrint("Template data preview (first 16 bytes): ");
            for (int i = 0; i < 16 && i < actualSize; i++) {
                M5_MODULE_DEBUG_SERIAL.printf("%02X ", templateBuffer[i]);
            }
            M5_MODULE_DEBUG_SERIAL.println("");
        }
#endif
        
        return FINGERPRINT_OK;
    } else if (callbackAborted) {
        serialPrintf("PS_UploadTemplateAuto: Upload aborted by callback after %d packets, %d bytes received\r\n",
                    packetCount, actualSize);
        return FINGERPRINT_OPERATION_BLOCKED;
    } else {
        serialPrintf("PS_UploadTemplateAuto: Upload incomplete, %d bytes received\r\n", actualSize);
        return FINGERPRINT_TIMEOUT;
    }
}

// 自动注册模板 - 自动注册指纹模板到指定ID
fingerprint_status_t M5UnitFingerprint2::PS_AutoEnroll(uint16_t ID, 
                                                       uint8_t enrollCount, 
                                                       fingerprint_auto_enroll_flags_t flags, 
                                                       uint8_t* param1, 
                                                       uint8_t* param2,
                                                       PS_AutoEnrollCallback_t callback) const
{
    // 参数检查
    if (enrollCount == 0 || enrollCount > 6) {  // 通常最多6次采集
        serialPrintln("Invalid enroll count for PS_AutoEnroll (1-6)");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 添加ID范围检查 (0-99)
    if (ID > 99) {
        serialPrintf("Invalid ID for PS_AutoEnroll: %d (valid range: 0-99)\r\n", ID);
        return FINGERPRINT_PARAM_ERROR;
    }

    // 创建命令参数 - 总共 5 字节 (ID: 2字节, enrollCount: 1字节, flags: 2字节)
    uint8_t params[5];
    params[0] = (ID >> 8) & 0xFF;           // ID 高字节
    params[1] = ID & 0xFF;                  // ID 低字节  
    params[2] = enrollCount;                // 注册次数（录入次数）
    params[3] = (static_cast<uint16_t>(flags) >> 8) & 0xFF;  // 参数高字节
    params[4] = static_cast<uint16_t>(flags) & 0xFF;         // 参数低字节

    // 创建命令数据包 - 使用命令代码 0x31
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_AUTO_ENROLL, params, 5);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_AutoEnroll command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    serialPrintf("PS_AutoEnroll: Starting enrollment [ID: %d, Count: %d, Flags: 0x%04X]\r\n",
                 ID, enrollCount, static_cast<uint16_t>(flags));
    
    // PS_AutoEnroll命令特殊处理 - 需要接收多个响应包
    fingerprint_status_t finalResult = FINGERPRINT_OK;
    uint8_t lastParam1 = 0, lastParam2 = 0;
    int packetCount = 0;
    unsigned long startTime = millis();
    const unsigned long totalTimeout = 60000; // 总超时时间60秒（增加时间以适应多次采集）
    bool enrollmentComplete = false;
    bool callbackAborted = false; // 标记是否被回调函数中止
    
    while (millis() - startTime < totalTimeout && !enrollmentComplete && !callbackAborted) {
        // 接收响应包
        Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
        if (!nonConstThis->receivePacketData(responsePacket, 10000)) {
            // 如果这是第一个包，则认为是通信错误
            if (packetCount == 0) {
                serialPrintln("Failed to receive PS_AutoEnroll initial response");
                return FINGERPRINT_PACKET_TIMEOUT;
            }
            // 否则可能是注册过程结束，退出循环
            break;
        }

        packetCount++;
        
        // 检查响应包类型
        if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
            serialPrintf("PS_AutoEnroll packet %d: Invalid response packet type\r\n", packetCount);
            continue;
        }

        // 检查数据长度
        if (responsePacket.get_actual_data_length() < 1) {
            serialPrintf("PS_AutoEnroll packet %d: Invalid response data length\r\n", packetCount);
            continue;
        }

        // 获取响应数据
        const uint8_t* responseData = responsePacket.get_data();
        uint8_t confirmationCode = responseData[0];
        uint8_t currentParam1 = (responsePacket.get_actual_data_length() >= 2) ? responseData[1] : 0;
        uint8_t currentParam2 = (responsePacket.get_actual_data_length() >= 3) ? responseData[2] : 0;
        
        // 保存最后的参数值
        lastParam1 = currentParam1;
        lastParam2 = currentParam2;
        
        // 显示过程信息
        if (responsePacket.get_actual_data_length() >= 3) {
            serialPrintf("PS_AutoEnroll process packet #%d: Status=%s(0x%02X), Param1=%s(0x%02X), Param2=%s(0x%02X)\r\n",
                         packetCount,
                         FingerprintDebugUtils::getStatusName(confirmationCode).c_str(), confirmationCode,
                         FingerprintDebugUtils::getAutoEnrollParam1Name(currentParam1).c_str(), currentParam1,
                         FingerprintDebugUtils::getAutoEnrollParam2Name(currentParam2).c_str(), currentParam2);
        } else {
            serialPrintf("PS_AutoEnroll process packet #%d: Status=%s(0x%02X)\r\n",
                         packetCount,
                         FingerprintDebugUtils::getStatusName(confirmationCode).c_str(), confirmationCode);
        }
        
        // 调用回调函数（如果提供了回调函数）
        if (callback != nullptr) {
            fingerprint_status_t currentStatus = static_cast<fingerprint_status_t>(confirmationCode);
            
            // 调用用户提供的回调函数
            bool continueProcessing = callback(ID, currentStatus, currentParam1, currentParam2, packetCount);
            
            if (!continueProcessing) {
                serialPrintln("PS_AutoEnroll: Process aborted by callback function");
                callbackAborted = true;
                finalResult = FINGERPRINT_OPERATION_BLOCKED; // 使用适当的状态码表示操作被用户中止
                break;
            }
        }
        
        // 记录最终结果状态
        finalResult = static_cast<fingerprint_status_t>(confirmationCode);
        
        // 检查是否是终止状态
        if (currentParam1 == PS_AUTO_ENROLL_PARAM1_STORE) {
            // 成功完成注册
            serialPrintln("PS_AutoEnroll: Registration completed successfully!");
            enrollmentComplete = true;
            break;
        } 
        
        // 如果设置了不返回状态标志，可能只会收到最终结果
        if ((flags & FINGERPRINT_AUTO_ENROLL_NO_STATUS_RETURN) && confirmationCode == FINGERPRINT_OK) {
            serialPrintln("PS_AutoEnroll: Completed (no status return mode)");
            enrollmentComplete = true;
            break;
        }
        
        // 继续等待下一个包
        delay(100); // 短暂延迟避免过度占用CPU
    }
    
    // 超时检查
    if (millis() - startTime >= totalTimeout) {
        serialPrintln("PS_AutoEnroll: Process timeout after 60 seconds");
        finalResult = FINGERPRINT_TIMEOUT;
    }
    
    // 设置返回参数
    if (param1 != nullptr) {
        *param1 = lastParam1;
    }
    if (param2 != nullptr) {
        *param2 = lastParam2;
    }
    
    // 输出最终结果总结
    if (callbackAborted) {
        serialPrintf("PS_AutoEnroll aborted by callback: %s [ID: %d, Packets received: %d, Final param1: %s(0x%02X), Final param2: %s(0x%02X)]\r\n",
                     FingerprintDebugUtils::getStatusName(finalResult).c_str(), 
                     ID, packetCount,
                     FingerprintDebugUtils::getAutoEnrollParam1Name(lastParam1).c_str(), lastParam1,
                     FingerprintDebugUtils::getAutoEnrollParam2Name(lastParam2).c_str(), lastParam2);
    } else {
        serialPrintf("PS_AutoEnroll final result: %s [ID: %d, Packets received: %d, Final param1: %s(0x%02X), Final param2: %s(0x%02X)]\r\n",
                     FingerprintDebugUtils::getStatusName(finalResult).c_str(), 
                     ID, packetCount,
                     FingerprintDebugUtils::getAutoEnrollParam1Name(lastParam1).c_str(), lastParam1,
                     FingerprintDebugUtils::getAutoEnrollParam2Name(lastParam2).c_str(), lastParam2);
    }

    return finalResult;
}

// 自动验证指纹 - 自动验证当前指纹并返回匹配结果
fingerprint_status_t M5UnitFingerprint2::PS_AutoIdentify(uint8_t securityLevel, 
                                                         uint16_t ID, 
                                                         fingerprint_auto_verify_flags_t flags, 
                                                         uint16_t& PageID,
                                                         PS_AutoIdentifyCallback_t callback) const
{
    // 参数检查 - 安全等级通常在0-1范围内
    if (securityLevel != 0 && securityLevel != 1) {
        serialPrintln("Invalid security level for PS_AutoIdentify");
        return FINGERPRINT_PARAM_ERROR;
    }

    // 创建命令参数 - 总共 5 字节 (根据协议文档调整)
    uint8_t params[5];
    params[0] = securityLevel;              // 安全等级
    params[1] = (ID >> 8) & 0xFF;           // ID 高字节 (作为输入参数)
    params[2] = ID & 0xFF;                  // ID 低字节
    params[3] = (static_cast<uint16_t>(flags) >> 8) & 0xFF;  // 标志位高字节
    params[4] = static_cast<uint16_t>(flags) & 0xFF;         // 标志位低字节

    // 创建命令数据包 - 使用命令代码 0x32
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_AUTO_IDENTIFY, params, 5);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_AutoIdentify command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    serialPrintf("PS_AutoIdentify: Starting identification [Security Level: %d, Input ID: %d, Flags: 0x%02X]\r\n",
                 securityLevel, ID, static_cast<uint8_t>(flags));
    
    // PS_AutoIdentify命令特殊处理 - 类似PS_AutoEnroll，需要接收多个响应包，但只有一个参数
    fingerprint_status_t finalResult = FINGERPRINT_OK;
    uint8_t lastParam = 0;
    int packetCount = 0;
    unsigned long startTime = millis();
    const unsigned long totalTimeout = 30000; // 总超时时间30秒
    bool identificationComplete = false;
    bool callbackAborted = false; // 标记是否被回调函数中止
    
    while (millis() - startTime < totalTimeout && !identificationComplete && !callbackAborted) {
        // 接收响应包
        Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
        if (!nonConstThis->receivePacketData(responsePacket, 10000)) {
            // 如果这是第一个包，则认为是通信错误
            if (packetCount == 0) {
                serialPrintln("Failed to receive PS_AutoIdentify initial response");
                return FINGERPRINT_PACKET_TIMEOUT;
            }
            // 否则可能是验证过程结束，退出循环
            break;
        }

        packetCount++;
        
        // 检查响应包类型
        if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
            serialPrintf("PS_AutoIdentify packet %d: Invalid response packet type\r\n", packetCount);
            continue;
        }

        // 检查数据长度
        if (responsePacket.get_actual_data_length() < 1) {
            serialPrintf("PS_AutoIdentify packet %d: Invalid response data length\r\n", packetCount);
            continue;
        }

        // 获取响应数据 - 注意：PS_AutoIdentify只有一个参数，不像PS_AutoEnroll有两个参数
        const uint8_t* responseData = responsePacket.get_data();
        uint8_t confirmationCode = responseData[0];
        uint8_t currentParam = (responsePacket.get_actual_data_length() >= 2) ? responseData[1] : 0;
        uint16_t recv_ID = 0;
        uint16_t recv_score = 0;
        
        // 如果数据长度足够，解析ID和分数
        if (responsePacket.get_actual_data_length() >= 4) {
            recv_ID = (responseData[2] << 8) | responseData[3]; // 返回的ID (大端序)
        }
        if (responsePacket.get_actual_data_length() >= 6) {
            recv_score = (responseData[4] << 8) | responseData[5]; // 返回的匹配分数 (大端序)
        }
        
        // 保存最后的参数值
        lastParam = currentParam;
        
        // 显示过程信息
        if (responsePacket.get_actual_data_length() >= 2) {
            if (responsePacket.get_actual_data_length() >= 6) {
                // 有完整的ID和分数信息
                serialPrintf("PS_AutoIdentify process packet #%d: Status=%s(0x%02X), Param=%s(0x%02X), ID=%d, Score=%d\r\n",
                             packetCount,
                             FingerprintDebugUtils::getStatusName(confirmationCode).c_str(), confirmationCode,
                             FingerprintDebugUtils::getAutoIdentifyParamName(static_cast<ps_auto_identify_param_t>(currentParam)).c_str(), currentParam,
                             recv_ID, recv_score);
            } else if (responsePacket.get_actual_data_length() >= 4) {
                // 只有ID信息
                serialPrintf("PS_AutoIdentify process packet #%d: Status=%s(0x%02X), Param=%s(0x%02X), ID=%d\r\n",
                             packetCount,
                             FingerprintDebugUtils::getStatusName(confirmationCode).c_str(), confirmationCode,
                             FingerprintDebugUtils::getAutoIdentifyParamName(static_cast<ps_auto_identify_param_t>(currentParam)).c_str(), currentParam,
                             recv_ID);
            } else {
                // 只有基本参数信息
                serialPrintf("PS_AutoIdentify process packet #%d: Status=%s(0x%02X), Param=%s(0x%02X)\r\n",
                             packetCount,
                             FingerprintDebugUtils::getStatusName(confirmationCode).c_str(), confirmationCode,
                             FingerprintDebugUtils::getAutoIdentifyParamName(static_cast<ps_auto_identify_param_t>(currentParam)).c_str(), currentParam);
            }
        } else {
            serialPrintf("PS_AutoIdentify process packet #%d: Status=%s(0x%02X)\r\n",
                         packetCount,
                         FingerprintDebugUtils::getStatusName(confirmationCode).c_str(), confirmationCode);
        }
        
        // 调用回调函数（如果提供了回调函数）
        if (callback != nullptr) {
            fingerprint_status_t currentStatus = static_cast<fingerprint_status_t>(confirmationCode);
            
            // 调用用户提供的回调函数
            bool continueProcessing = callback(securityLevel, currentStatus, currentParam, recv_ID, recv_score, packetCount);
            
            if (!continueProcessing) {
                serialPrintln("PS_AutoIdentify: Process aborted by callback function");
                callbackAborted = true;
                finalResult = FINGERPRINT_OPERATION_BLOCKED; // 使用适当的状态码表示操作被用户中止
                PageID = 0;
                break;
            }
        }
        
        // 记录最终结果状态
        finalResult = static_cast<fingerprint_status_t>(confirmationCode);
        
        // 检查是否是终止状态
        if (currentParam == PS_AUTO_IDENTIFY_PARAM_VERIFY) {
            if (confirmationCode == FINGERPRINT_OK) {
                // 成功完成验证，尝试获取匹配的PageID
                if (responsePacket.get_actual_data_length() >= 4) {
                    PageID = (responseData[2] << 8) | responseData[3]; // 匹配的页码ID
                    serialPrintln("PS_AutoIdentify: Identification completed successfully!");
                } else {
                    PageID = 0; // 没有返回PageID
                    serialPrintln("PS_AutoIdentify: Identification completed but no PageID returned");
                }
                identificationComplete = true;
                break;
            } else {
                // 验证失败
                PageID = 0;
                serialPrintln("PS_AutoIdentify: Identification failed at verification stage");
                identificationComplete = true;
                break;
            }
        }
        
        // 如果设置了不返回状态标志，可能只会收到最终结果
        if ((flags & FINGERPRINT_AUTO_VERIFY_NO_STATUS_RETURN) && confirmationCode == FINGERPRINT_OK) {
            if (responsePacket.get_actual_data_length() >= 4) {
                PageID = (responseData[2] << 8) | responseData[3]; // 匹配的页码ID
            } else {
                PageID = 0;
            }
            serialPrintln("PS_AutoIdentify: Completed (no status return mode)");
            identificationComplete = true;
            break;
        }
        
        // 继续等待下一个包
        delay(100); // 短暂延迟避免过度占用CPU
    }
    
    // 超时检查
    if (millis() - startTime >= totalTimeout) {
        serialPrintln("PS_AutoIdentify: Process timeout after 30 seconds");
        PageID = 0;
        finalResult = FINGERPRINT_TIMEOUT;
    }
    
    // 输出最终结果总结
    if (callbackAborted) {
        serialPrintf("PS_AutoIdentify aborted by callback: %s [Security Level: %d, Input ID: %d, Packets received: %d, Final param: %s(0x%02X)]\r\n",
                     FingerprintDebugUtils::getStatusName(finalResult).c_str(), 
                     securityLevel, ID, packetCount,
                     FingerprintDebugUtils::getAutoIdentifyParamName(static_cast<ps_auto_identify_param_t>(lastParam)).c_str(), lastParam);
    } else if (finalResult == FINGERPRINT_OK) {
        serialPrintf("PS_AutoIdentify final result: %s [Security Level: %d, Input ID: %d, Matched PageID: %d, Flags: 0x%02X, Packets received: %d, Final param: %s(0x%02X)]\r\n",
                     FingerprintDebugUtils::getStatusName(finalResult).c_str(), 
                     securityLevel, ID, PageID, static_cast<uint8_t>(flags), packetCount,
                     FingerprintDebugUtils::getAutoIdentifyParamName(static_cast<ps_auto_identify_param_t>(lastParam)).c_str(), lastParam);
    } else {
        serialPrintf("PS_AutoIdentify final result: %s [Security Level: %d, Input ID: %d, Flags: 0x%02X, Packets received: %d, Final param: %s(0x%02X)]\r\n",
                     FingerprintDebugUtils::getStatusName(finalResult).c_str(), 
                     securityLevel, ID, static_cast<uint8_t>(flags), packetCount,
                     FingerprintDebugUtils::getAutoIdentifyParamName(static_cast<ps_auto_identify_param_t>(lastParam)).c_str(), lastParam);
    }

    return finalResult;
}
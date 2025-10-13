/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __M5_UNIT_FINGERPRINT2_H
#define __M5_UNIT_FINGERPRINT2_H

#include "Arduino.h"
#include <cstring>
#include "M5UnitFingerprint2_defs.hpp"
#include "M5UnitFingerprint2_debug.hpp"

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#endif

class Fingerprint_Packet {
public:
    Fingerprint_Packet(uint16_t start_code = FINGERPRINT_STARTCODE, uint32_t address = 0, uint8_t type = FINGERPRINT_PACKET_DATAPACKET, const uint8_t* data = nullptr, uint16_t data_length = 0) {
        this->start_code = start_code;
        this->set_address(address);
        this->type = type;
        this->actual_data_length = data_length; // 存储实际数据长度 / Store actual data length
        this->data_length = data_length + 2; // 包长度 = 实际数据长度 + 校验和长度(2字节) / Packet length = actual data length + checksum length (2 bytes)
        this->packet_size = sizeof(start_code) + sizeof(address) + sizeof(type) + sizeof(data_length) + this->data_length; // 计算整个包的大小 / Calculate total packet size
        if (data != nullptr && data_length <= FINGERPRINT_MAX_PACKET_SIZE) {
            memcpy(this->data, data, data_length);
        }
        this->checksum = calculate_checksum();
    }

    static Fingerprint_Packet new_command_packet(uint32_t address, uint8_t command, const uint8_t* params, uint16_t param_length) {
        uint8_t full_data[FINGERPRINT_MAX_PACKET_SIZE] = {0};
        full_data[0] = command; // 第一个数据是命令 / First data is command
        if (params != nullptr && param_length > 0 && param_length <= FINGERPRINT_MAX_PACKET_SIZE - 1) {
            memcpy(&full_data[1], params, param_length); // 其他数据是命令的参数 / Other data are command parameters
        }
        return Fingerprint_Packet(FINGERPRINT_STARTCODE, address, FINGERPRINT_PACKET_COMMANDPACKET, full_data, param_length + 1);
    }

    static Fingerprint_Packet new_data_packet(uint32_t address, const uint8_t* data, uint16_t data_length) {
        return Fingerprint_Packet(FINGERPRINT_STARTCODE, address, FINGERPRINT_PACKET_DATAPACKET, data, data_length);
    }

    static Fingerprint_Packet new_end_packet(uint32_t address, const uint8_t* data, uint16_t data_length) {
        return Fingerprint_Packet(FINGERPRINT_STARTCODE, address, FINGERPRINT_PACKET_ENDDATAPACKET, data, data_length);
    }

    static Fingerprint_Packet new_ack_packet(uint32_t address, const uint8_t* data, uint16_t data_length) {
        return Fingerprint_Packet(FINGERPRINT_STARTCODE, address, FINGERPRINT_PACKET_ACKPACKET, data, data_length);
    }

    uint16_t get_start_code() const { return start_code; }
    uint32_t get_address() const {
        return (static_cast<uint32_t>(address[0]) << 24) |
               (static_cast<uint32_t>(address[1]) << 16) |
               (static_cast<uint32_t>(address[2]) << 8) |
               static_cast<uint32_t>(address[3]);
    }
    uint8_t get_type() const { return type; }
    uint16_t get_data_length() const { return data_length; }
    uint16_t get_actual_data_length() const { return actual_data_length; } // 获取实际数据长度 / Get actual data length
    uint16_t get_packet_size() const { return packet_size; } // 获取整个数据包的大小（用于发送） / Get total packet size (for sending)
    const uint8_t* get_data() const { return data; }
    uint16_t get_checksum() const { return checksum; }

    void set_address(uint32_t addr) {
        address[0] = (addr >> 24) & 0xFF;
        address[1] = (addr >> 16) & 0xFF;
        address[2] = (addr >> 8) & 0xFF;
        address[3] = addr & 0xFF;
    }

    /**
     * @brief 将数据包序列化为字节数组
     * @param buffer 输出缓冲区
     * @param buffer_size 缓冲区大小
     * @return 序列化后的字节数，失败返回0
     */
    size_t serialize(uint8_t* buffer, size_t buffer_size) const {
        if (buffer == nullptr || buffer_size < get_packet_size()) {
            return 0;
        }
        
        size_t index = 0;
        
        // 添加起始码 (2字节) / Add start code (2 bytes)
        buffer[index++] = (start_code >> 8) & 0xFF;
        buffer[index++] = start_code & 0xFF;
        
        // 添加地址 (4字节) / Add address (4 bytes)
        buffer[index++] = address[0];
        buffer[index++] = address[1];
        buffer[index++] = address[2];
        buffer[index++] = address[3];
        
        // 添加包标识 (1字节) / Add packet identifier (1 byte)
        buffer[index++] = type;
        
        // 添加包长度 (2字节) / Add packet length (2 bytes)
        buffer[index++] = (data_length >> 8) & 0xFF;
        buffer[index++] = data_length & 0xFF;
        
        // 添加数据内容 / Add data content
        for (uint16_t i = 0; i < actual_data_length; i++) {
            buffer[index++] = data[i];
        }
        
        // 添加校验和 (2字节) / Add checksum (2 bytes)
        buffer[index++] = (checksum >> 8) & 0xFF;
        buffer[index++] = checksum & 0xFF;
        
        return index;
    }

private:
    uint16_t calculate_checksum() const {
        // 校验和计算：包标识 + 包长度 + 实际数据内容 / Checksum calculation: packet identifier + packet length + actual data content
        uint16_t sum = type + data_length;
        for (uint16_t i = 0; i < actual_data_length; ++i) {
            sum += data[i];
        }
        return sum & 0xFFFF; // 忽略超出 2 字节的进位 / Ignore carry beyond 2 bytes
    }

    uint16_t start_code;
    uint8_t address[4];
    uint8_t type;
    uint16_t data_length;        // 包长度（实际数据长度 + 校验和长度2字节） / Packet length (actual data length + checksum length 2 bytes)
    uint16_t actual_data_length; // 实际数据长度 / Actual data length
    uint16_t packet_size;        // 整个数据包的大小（用于发送） / Total packet size (for sending)
    uint8_t data[FINGERPRINT_MAX_PACKET_SIZE];
    uint16_t checksum;
};


typedef enum {
    FINGERPRINT_LED_BREATHING = 1,  // 普通呼吸灯 / Normal breathing light
    FINGERPRINT_LED_FLASHING  = 2,  // 闪烁灯 / Flashing light
    FINGERPRINT_LED_ON        = 3,  // 常开灯 / Always on
    FINGERPRINT_LED_OFF       = 4,  // 常闭灯 / Always off
    FINGERPRINT_LED_FADE_IN   = 5,  // 渐开灯 / Fade in
    FINGERPRINT_LED_FADE_OUT  = 6   // 渐闭灯 / Fade out
} fingerprint_led_control_mode_t;

typedef enum {
    FINGERPRINT_LED_COLOR_OFF    = 0x00,  // 全灭 / All off
    FINGERPRINT_LED_COLOR_BLUE   = 0x01,  // 蓝灯 / Blue
    FINGERPRINT_LED_COLOR_GREEN  = 0x02,  // 绿灯 / Green
    FINGERPRINT_LED_COLOR_CYAN   = 0x03,  // 青色灯 (蓝+绿) / Cyan (Blue + Green)
    FINGERPRINT_LED_COLOR_RED    = 0x04,  // 红灯 / Red
    FINGERPRINT_LED_COLOR_PURPLE = 0x05,  // 紫色灯 (蓝+红) / Purple (Blue + Red)
    FINGERPRINT_LED_COLOR_YELLOW = 0x06,  // 黄色灯 (绿+红) / Yellow (Green + Red)
    FINGERPRINT_LED_COLOR_WHITE  = 0x07   // 白色灯 (蓝+绿+红) / White (Blue + Green + Red)
} fingerprint_led_color_t;

typedef enum {
    FINGERPRINT_AUTO_ENROLL_DEFAULT              = 0x0000, // 默认设置 / Default setting
    FINGERPRINT_AUTO_ENROLL_NO_STATUS_RETURN     = (1 << 2), // 注册过程中不返回当前状态 / Don't return current status during enrollment
    FINGERPRINT_AUTO_ENROLL_ALLOW_OVERWRITE_ID   = (1 << 3), // 允许覆盖已存在的ID号 / Allow overwriting existing ID
    FINGERPRINT_AUTO_ENROLL_FORBID_DUPLICATES    = (1 << 4), // 禁止指纹重复注册 / Forbid duplicate fingerprint enrollment
    FINGERPRINT_AUTO_ENROLL_NO_LIFT_REQUIRED     = (1 << 5), // 采集间不要求手指离开 / No finger lift required between captures
} fingerprint_auto_enroll_flags_t;

typedef enum {
    FINGERPRINT_AUTO_ENROLL_LEGAL_CHECK     = 0x00, // 指纹合法性检测 / Fingerprint legality check
    FINGERPRINT_AUTO_ENROLL_GET_IMAGE       = 0x01, // 获取图像 / Get image
    FINGERPRINT_AUTO_ENROLL_GEN_CHAR        = 0x02, // 生成特征 / Generate characteristics
    FINGERPRINT_AUTO_ENROLL_CHECK_LIFT      = 0x03, // 判断手指离开 / Check finger lift
    FINGERPRINT_AUTO_ENROLL_MERGE_TEMPLATE  = 0x04, // 合并模板 / Merge template
    FINGERPRINT_AUTO_ENROLL_VERIFY          = 0x05, // 注册检验 / Enrollment verification
    FINGERPRINT_AUTO_ENROLL_STORE_TEMPLATE  = 0x06, // 存储模板 / Store template
} fingerprint_auto_enroll_stage_t;

// PS_AutoEnroll 参数1 (阶段码) / PS_AutoEnroll Parameter 1 (Stage code)
typedef enum : uint8_t {
    PS_AUTO_ENROLL_PARAM1_LEGAL_CHECK   = 0x00, // 指纹合法性检测 / Fingerprint legality check
    PS_AUTO_ENROLL_PARAM1_GET_IMAGE     = 0x01, // 获取图像 / Get image
    PS_AUTO_ENROLL_PARAM1_GEN_CHAR      = 0x02, // 生成特征 / Generate characteristics
    PS_AUTO_ENROLL_PARAM1_LIFT_FINGER   = 0x03, // 判断手指离开 / Check finger lift
    PS_AUTO_ENROLL_PARAM1_MERGE_TEMPLATE= 0x04, // 合并模板 / Merge template
    PS_AUTO_ENROLL_PARAM1_VERIFY        = 0x05, // 注册验证 / Enrollment verification
    PS_AUTO_ENROLL_PARAM1_STORE         = 0x06  // 存储模板 / Store template
} ps_auto_enroll_param1_t;

// PS_AutoEnroll 参数2 (状态/错误码) / PS_AutoEnroll Parameter 2 (Status/Error code)
typedef enum : uint8_t {
    PS_AUTO_ENROLL_PARAM2_OK                = 0x00, // 指纹合法性检测 / Fingerprint legality check
    PS_AUTO_ENROLL_PARAM2_MERGE_TEMPLATE    = 0xF0, // 合并模板 / Merge template
    PS_AUTO_ENROLL_PARAM2_FINGER_DUPLICATED = 0xF1, // 检验该手指是否已注册 / Check if finger is already registered
    PS_AUTO_ENROLL_PARAM2_STORE_TEMPLATE    = 0xF2, // 存储模板 / Store template
} ps_auto_enroll_param2_t;

// PS_AutoIdentify 标志位 / PS_AutoIdentify flags
typedef enum {
    FINGERPRINT_AUTO_VERIFY_DEFAULT          = 0x0000, // 默认设置 / Default setting
    FINGERPRINT_AUTO_VERIFY_NO_STATUS_RETURN = (1 << 2), // 验证过程中不返回当前状态 / Don't return current status during verification
} fingerprint_auto_verify_flags_t;

// PS_AutoIdentify 参数 / PS_AutoIdentify parameters
typedef enum {
    PS_AUTO_IDENTIFY_PARAM_LEGAL_CHECK   = 0x00, // 指纹合法性检测 / Fingerprint legality check
    PS_AUTO_IDENTIFY_PARAM_GET_IMAGE     = 0x01, // 获取图像 / Get image
    PS_AUTO_IDENTIFY_PARAM_VERIFY        = 0x05, // 已注册指纹比对 / Registered fingerprint matching
} ps_auto_identify_param_t;

// 唤醒回调函数类型定义 / Wakeup callback function type definition
typedef void (*PS_WakeupCallback_t)(const uint8_t* wakeupPacket, size_t packetLength);

//PS_WriteReg 寄存器序号 枚举类型 / PS_WriteReg register number enumeration type
typedef enum : uint8_t {
    // FP_REG_DELAY_TIME     = 0x00, // 延迟时间寄存器（DelayTime） / Delay time register (DelayTime)
    // FP_REG_ENROLL_TIMES   = 0x01, // 注册次数寄存器（EnrollTimes） / Enrollment times register (EnrollTimes)
    // FP_REG_IMAGE_FORMAT   = 0x02, // 图像格式寄存器（ImageFormat），固定格式 0 / Image format register (ImageFormat), fixed format 0
    // FP_REG_ENROLL_MODE    = 0x03, // 注册逻辑寄存器（EnrollMode），mode 0 / Enrollment logic register (EnrollMode), mode 0
    // FP_REG_BAUD_RATE      = 0x04, // 波特率控制寄存器（BaudRate = 9600 * (N + 1)） / Baud rate control register (BaudRate = 9600 * (N + 1))
    // FP_REG_MATCH_LEVEL    = 0x05, // 比对阈值寄存器（MatchLevel，level 1–5） / Match threshold register (MatchLevel, level 1–5)
    FP_REG_PACKET_SIZE    = 0x06, // 包大小寄存器（PacketSize：0=32B 1=64B 2=128B 3=256B） / Packet size register (PacketSize: 0=32B 1=64B 2=128B 3=256B)
    // FP_REG_SECURITY_LEVEL = 0x07, // 加密等级寄存器（SecurityLevel：0=level0 1=保留 2=level2 3=level3） / Encryption level register (SecurityLevel: 0=level0 1=reserved 2=level2 3=level3)
    // FP_REG_RESERVED_08    = 0x08, // 保留 / Reserved
    // FP_REG_RESERVED_09    = 0x09, // 保留 / Reserved
    // FP_REG_PRODUCT_SN     = 0x0A, // 产品型号寄存器（ProductSN） / Product model register (ProductSN)
    // FP_REG_LED_CTRL       = 0x0B  // 自动背光灯寄存器（LedCtr） / Auto backlight register (LedCtr)
} fingerprint_register_id_t;

//PS_ReadSysPara 应答包 - 16字节系统参数 / PS_ReadSysPara response packet - 16-byte system parameters
struct PS_ReadSysPara_BasicParams {
    uint16_t status_register;     // 1) 状态寄存器(2 bytes) - 系统工作状态 / Status register (2 bytes) - System working status
    uint16_t temp_size;           // 2) 指纹模板大小(2 bytes) - 指纹模板大小指示 / Fingerprint template size (2 bytes) - Template size indicator
    uint16_t data_size;           // 3) 指纹库大小(2 bytes) - 指纹库容量指示 / Fingerprint library size (2 bytes) - Library capacity indicator  
    uint16_t score_level;         // 4) 安全等级(2 bytes) - 比对阈值设定 / Security level (2 bytes) - Match threshold setting
    uint32_t device_addr;         // 5) 设备地址(4 bytes) - 模块通信地址 / Device address (4 bytes) - Module communication address
    uint16_t packet_size;         // 6) 数据包大小(2 bytes) - CFG_PktSize / Packet size (2 bytes) - CFG_PktSize
    uint16_t baud_rate;           // 7) 串口波特率(2 bytes) - CFG_BaudRate / Serial baud rate (2 bytes) - CFG_BaudRate
} __attribute__((packed));

class M5UnitFingerprint2 {
public:
    /** 构造和析构函数 / Constructor and destructor */
    /**
     * @brief Constructs a new M5UnitFingerprint2 object for Arduino.
     *
     * This constructor initializes the M5UnitFingerprint2 instance for Arduino platforms.
     * Creates mutex lock for thread-safe operations on ESP32 or ESP8266 platforms.
     * @param serialPort The HardwareSerial port to use for communication (default: nullptr).
     * @param txPin The TX pin number for serial communication (default: -1, use default pin).
     * @param rxPin The RX pin number for serial communication (default: -1, use default pin).
     * @param address The address for the fingerprint module (default: 0xFFFFFFFF).
     */
    M5UnitFingerprint2(HardwareSerial* serialPort = nullptr, int txPin = -1, int rxPin = -1, uint32_t address = 0xFFFFFFFF);



    ~M5UnitFingerprint2();

    /**
     * @brief Initializes the M5UnitFingerprint2 module.
     *
     * This function initializes the M5UnitFingerprint2 module by setting up the serial communication,
     * creating necessary tasks and queues for data processing, and configuring mutex locks for 
     * thread-safe operations. It configures the serial port at 115200 baud rate and creates
     * background tasks for packet parsing on ESP32/ESP8266 platforms.
     * 
     * For Arduino platforms: Initializes HardwareSerial with specified or default pins.
     * 
     * @return true if initialization succeeded, false if any step failed.
     * @note This function should be called before any other operations on the module.
     */
    bool begin();

    /**
     * @brief Reads data from the serial port.
     *
     * This function reads data from the configured serial port (Arduino HardwareSerial)
     * and stores it in the provided buffer. The function is platform-aware and uses the appropriate
     * serial interface based on the platform configuration.
     * @param buffer The buffer to store the read data.
     * @param length The maximum number of bytes to read.
     * @return The number of bytes actually read (0 if no data available or error).
     */
    size_t readSerial(uint8_t* buffer, size_t length);

    /**
     * @brief Writes data to the serial port.
     *
     * This function writes data to the configured serial port (Arduino HardwareSerial).
     * The function is platform-aware and uses the appropriate serial interface based on the platform configuration.
     * @param buffer The buffer containing the data to write.
     * @param length The number of bytes to write.
     * @return The number of bytes actually written (0 if error occurred).
     */
    size_t writeSerial(const uint8_t* buffer, size_t length);

    // Wakeup callback related interface
    /**
     * @brief Set the wakeup callback function.
     *
     * Allows the user to register a custom callback that will be invoked when a
     * wakeup packet from the fingerprint module is detected.
     * If no custom callback is provided (callback == nullptr) a built‑in
     * default logging callback will be used.
     *
     * Thread-safety: The callback is stored as a raw function pointer and is
     * expected to be lightweight and non-blocking. Avoid long delays or
     * blocking operations inside the callback.
     *
     * @param callback Function pointer of type PS_WakeupCallback_t. Pass nullptr
     *                 to restore/use the default callback.
     */
    void setWakeupCallback(PS_WakeupCallback_t callback);

    /**
     * @brief Get the currently registered wakeup callback.
     *
     * Returns the user-provided callback if one was set. If this returns
     * nullptr the internal default callback will be used when a wakeup packet
     * arrives.
     *
     * @return PS_WakeupCallback_t Currently active wakeup callback pointer, or
     *                             nullptr meaning the default will be used.
     */
    PS_WakeupCallback_t getWakeupCallback() const;
    
    //MCU命令 / MCU commands
    fingerprint_status_t PS_SetSleepTime(uint8_t SleepTime) const;                      //D0H 设置休眠时间 10-254范围 单位：秒 / Set sleep time, range 10-254, unit: seconds
    fingerprint_status_t PS_GeTSleepTime(uint8_t &SleepTime) const;                     //D1H 获取休眠时间 / Get sleep time
    fingerprint_status_t PS_SetWorkMode(uint8_t WorkMode) const;                        //D2H 设置工作模式 -> 0:定时休眠模式 1:开启模式 / Set work mode -> 0: timed sleep mode 1: on mode
    fingerprint_status_t PS_GetWorkMode(uint8_t &WorkMode) const;                       //D3H 获取工作模式 / Get work mode
    fingerprint_status_t PS_ActivateFingerprintModule(void)  const;                     //D4H 激活 Unit-Fingerprint 内部指纹模组 / Activate Unit-Fingerprint internal fingerprint module
    fingerprint_status_t PS_GetFingerprintModuleStatus(uint8_t &ModuleStatus) const;    //D5H 获取指纹模组工作状态 / Get fingerprint module working status
    fingerprint_status_t PS_SaveConfigurationToFlash(uint8_t SaveOptions) const;        //D6H 保存配置到 Flash -> 0:保存休眠时间 1:保存工作模式 / Save configuration to Flash -> 0: save sleep time 1: save work mode
    fingerprint_status_t PS_GetFirmwareVersion(uint8_t &FwVersion) const;               //D7H 获取固件版本 / Get firmware version

    //指纹模块操作 / Fingerprint module operations
    fingerprint_status_t PS_GetImage(void) const;                                       //01H 验证用获取图像 / Get image for verification
    fingerprint_status_t PS_GetEnrollImage(void) const;                                 //29H 注册用获取图像 / Get image for enrollment
    fingerprint_status_t PS_GenChar(uint8_t BufferID) const;                            //02H 生成特征文件 输入：BufferID 该命令中标识第几次按手指 / Generate characteristic file, Input: BufferID identifies which finger press
    fingerprint_status_t PS_Match(uint16_t &compareScores) const;                       //03H 精确比对两枚指纹特征 返回：比对得分 / Precisely match two fingerprint characteristics, Return: match score
    // ↓ 04H 搜索指纹 输入：BufferID:默认为1，起始页，搜索页数 返回：页码ID，匹配得分 / Search fingerprint, Input: BufferID: default 1, start page, page count, Return: page ID, match score
    fingerprint_status_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, uint16_t &PageID, uint16_t &MatchScore) const;
    fingerprint_status_t PS_RegModel(void) const;                                       //05H 合并特征（生成模板） / Merge characteristics (generate template)
    fingerprint_status_t PS_StoreChar(uint8_t BufferID, uint16_t PageID) const;         //06H 储存模板 输入：BufferID 默认为1，PageID 指纹库的位置号 / Store template, Input: BufferID default 1, PageID fingerprint library location number
    fingerprint_status_t PS_LoadChar(uint8_t BufferID, uint16_t PageID) const;          //07H 读取模板 输入：BufferID 默认为2，PageID:指纹库的模板号 / Read template, Input: BufferID default 2, PageID: fingerprint library template number
    // ↓ 0AH 上传图像 输入：imageBuffer:图像数据缓冲区，bufferSize:缓冲区大小，actualImageSize:实际上传的图像大小 / Upload image, Input: imageBuffer: image data buffer, bufferSize: buffer size, actualImageSize: actual uploaded image size
    fingerprint_status_t PS_UpImage(uint8_t* imageBuffer, uint32_t bufferSize, uint32_t& actualImageSize) const; 
    fingerprint_status_t PS_DeletChar(uint16_t PageID, uint16_t Num) const;             //0CH 删除模板 输入：PageID 指纹库的模板号，Num 删除数量 / Delete template, Input: PageID fingerprint library template number, Num delete quantity
    fingerprint_status_t PS_Empty(void) const;                                          //0DH 清空指纹库 / Empty fingerprint library
    fingerprint_status_t PS_WriteReg(fingerprint_register_id_t RegID, uint8_t Value) const; //0EH 写寄存器 输入：RegID 寄存器枚举类型，Value 寄存器值 / Write register, Input: RegID register enumeration type, Value register value
    fingerprint_status_t PS_ReadSysPara(PS_ReadSysPara_BasicParams &RawData) const;                        //0FH 读取系统参数 返回：16字节数据 / Read system parameters, Return: 16-byte data
    fingerprint_status_t PS_GetRandomCode(uint32_t &RandomCode) const;                  //14H 采样随机数 返回：4字节随机数 / Sample random number, Return: 4-byte random number
    fingerprint_status_t PS_ReadINFpage(uint8_t* INFData) const;                        //16H 读取INF页 返回：512字节数据 (应答后有后续包) / Read INF page, Return: 512-byte data (followed by subsequent packets)
    // ↓ 18H 写记事本 输入：NotepadID:便笺号 0-3，NotepadData:便笺数据，NotepadLength:便笺数据长度(不足32字节会自动补0) / Write notepad, Input: NotepadID: notepad number 0-3, NotepadData: notepad data, NotepadLength: notepad data length (auto-filled to 32 bytes)
    fingerprint_status_t PS_WriteNotepad(uint8_t NotepadID, const uint8_t* NotepadData, uint16_t NotepadLength) const;
    fingerprint_status_t PS_ReadNotepad(uint8_t NotepadID, uint8_t* NotepadData) const; // 19H 读记事本 输入：NotepadID 便笺号 0-3，NotepadData 便笺数据(返回32字节数据) / Read notepad, Input: NotepadID notepad number 0-3, NotepadData notepad data (returns 32-byte data)
    fingerprint_status_t PS_ValidTemplateNum(uint16_t &ValidNum) const;                 // 1DH 有效模板数量 返回：有效模板数量 / Valid template count, Return: valid template count
    // ↓ 1FH 读取索引表 返回：IndexTableData:索引表数据(返回32字节数据),每1bit代表一个模板的状态 1:已录入 0:未录入 ，只有前100位有效 / Read index table, Return: IndexTableData: index table data (returns 32-byte data), each 1bit represents template status 1: enrolled 0: not enrolled, only first 100 bits valid
    fingerprint_status_t PS_ReadIndexTable(uint8_t* IndexTableData) const; 
    fingerprint_status_t PS_GetChipSN(uint8_t* ChipSN) const;                          // 34H 获取芯片序列号 返回：32字节芯片序列号 / Get chip serial number, Return: 32-byte chip serial number
    fingerprint_status_t PS_HandShake(void) const;                                     // 35H 握手指令 / Handshake command
    fingerprint_status_t PS_CheckSensor(void) const;                                   // 36H 校验传感器 / Check sensor
    // ↓ 3CH LED 控制灯指令 输入：功能码，起始颜色（仅限于普通呼吸灯），结束颜色，循环次数（仅适用于呼吸、闪烁） / LED control command, Input: function code, start color (only for normal breathing light), end color, loop count (only for breathing, flashing)
    fingerprint_status_t PS_ControlBLN(fingerprint_led_control_mode_t mode, fingerprint_led_color_t startColor, fingerprint_led_color_t endColor, uint8_t loopCount = 0) const;
    // ↓ 3DH 获取图像信息指令 输入：返回图像面积（百分比），图像质量（0:合格，其他：不合格） / Get image info command, Input: return image area (percentage), image quality (0: qualified, other: unqualified)
    fingerprint_status_t PS_GetImageInfo(uint8_t &imageArea, uint8_t &imageQuality) const;
    // ↓ 3EH 搜索当前指纹指令 输入：起始页面，页数 返回：页码（相配指纹模板），得分（MatchScore） / Search current fingerprint command, Input: start page, page count, Return: page ID (matching fingerprint template), score (MatchScore)
    fingerprint_status_t PS_SearchNow(uint16_t StartPage, uint16_t PageNum, uint16_t &PageID, uint16_t &MatchScore) const;
    // ↓ 7AH 特殊上传模板 输入：模板偏移地址，指定的上传模板大小 返回：实际的上传模板大小，模板数据  （实际的上传模板大小 小于 指定的上传模板大小 或为0时，即为最后一个包） / Special upload template, Input: template offset address, specified upload template size, Return: actual upload template size, template data (when actual upload template size is less than specified upload template size or 0, it is the last packet)
    fingerprint_status_t PS_UploadTemplate(uint16_t offset, uint16_t uploadSize, uint16_t &actualSize, uint8_t* templateData) const;
    // ↓ 7AH 特殊上传模板 自动上传函数 输入：模板上传缓冲区，回调函数 返回：模板实际大小 / Special upload template auto upload function, Input: template upload buffer, callback function, Return: template actual size
    fingerprint_status_t PS_UploadTemplateAuto(
        uint8_t* templateBuffer,                                            // 上传缓冲区 / Upload buffer
        uint32_t bufferSize,                                                // 缓冲区大小 / Buffer size
        uint32_t& actualSize,                                               // 返回的实际上传大小 / Returned actual upload size
        PS_UploadTemplateCallback_t callback = nullptr                      // 每接收一段数据时的回调 / Callback when receiving each data segment
    ) const;
    // ↓ 7BH 特殊下载模板 输入：模板偏移地址，指定的下载模板大小，模板数据 / Special download template, Input: template offset address, specified download template size, template data
    fingerprint_status_t PS_DownloadTemplate(uint16_t offset, uint16_t downloadSize, const uint8_t* templateData) const;
    // ↓ 7BH 特殊下载模板 自动下载函数 输入：模板缓冲区，实际下载的模板大小，回调函数 返回：无 / Special download template auto download function, Input: template buffer, actual download template size, callback function, Return: none
    fingerprint_status_t PS_DownloadTemplateAuto(
        const uint8_t* templateBuffer,                                      // 模板缓冲区 / Template buffer
        uint32_t actualSize,                                                // 实际下载的模板大小 / Actual download template size
        PS_DownloadTemplateCallback_t callback = nullptr                    // 每发送一段数据时的回调 / Callback when sending each data segment
    ) const;
    // 模块化指令 / Modular commands
    fingerprint_status_t PS_Cancel(void) const;                             // 30H 取消指令 / Cancel command
    // ↓ 31H 自动注册模板 输入：ID号，录入次数，输入参数（2byte） 返回：返回参数1（1byte） 返回参数2（1byte） / Auto enroll template, Input: ID number, enrollment count, input parameters (2 bytes), Return: return parameter 1 (1 byte), return parameter 2 (1 byte)
    fingerprint_status_t PS_AutoEnroll(uint16_t ID, uint8_t enrollCount, fingerprint_auto_enroll_flags_t flags,
                                       uint8_t* param1 = nullptr, uint8_t* param2 = nullptr,
                                       PS_AutoEnrollCallback_t callback = nullptr) const;
    // ↓ 32H 自动验证指纹 输入：安全等级，ID号，输入参数 返回：页码（相配指纹模板） / Auto verify fingerprint, Input: security level, ID number, input parameters, Return: page ID (matching fingerprint template)
    fingerprint_status_t PS_AutoIdentify(uint8_t securityLevel, uint16_t ID, fingerprint_auto_verify_flags_t flags,
                                         uint16_t& PageID, PS_AutoIdentifyCallback_t callback = nullptr) const;


private:
    // 静态实例指针，用于在回调函数中访问类成员 / Static instance pointer for accessing class members in callbacks
    static M5UnitFingerprint2* instance;

    // 串口对象 (Arduino) / Serial port object (Arduino)
    HardwareSerial* _serialPort = nullptr;

    // 默认地址 / Default address
    uint32_t _fp2_address = 0xFFFFFFFF;

    int _txPin = -1; // TX引脚编号 / TX pin number
    int _rxPin = -1; // RX引脚编号 / RX pin number

    // 数据包接收相关 / Packet reception related
    uint8_t _recvBuffer[FINGERPRINT_RECV_BUFFER_SIZE]; // 接收缓冲区，使用更大的缓冲区 / Receive buffer, using larger buffer
    size_t _recvIndex = 0; // 当前接收索引 / Current receive index
    bool _waitingForResponse = false; // 是否在等待响应 / Whether waiting for response
    Fingerprint_Packet* _expectedPacket = nullptr; // 期望的响应包 / Expected response packet
    bool _packetReceived = false; // 是否已收到完整包 / Whether complete packet received
    
    // 新的包间隔检测和解析机制 / New packet interval detection and parsing mechanism
    unsigned long _lastReceiveTime = 0; // 最后接收到数据的时间 / Last data receive time
    bool _hasNewData = false; // 是否有新数据需要解析 / Whether there's new data to parse
    
    // 解析队列 - 存储完整的数据包 / Parse queue - stores complete packets
    struct ParsedPacket {
        uint8_t data[FINGERPRINT_MAX_PACKET_SIZE]; // 完整包数据 / Complete packet data
        size_t length; // 包长度 / Packet length
        unsigned long timestamp; // 接收时间戳 / Receive timestamp
    };
    ParsedPacket _parsedPackets[MAX_PARSED_PACKETS];
    size_t _parsedPacketIndex = 0; // 当前解析包索引 / Current parsed packet index
    size_t _parsedPacketCount = 0; // 已解析包数量 / Parsed packet count
    
    // 唤醒回调函数相关 / Wakeup callback related
    PS_WakeupCallback_t _wakeupCallback = nullptr; // 用户设置的唤醒回调函数 / User-set wakeup callback function

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
    /** FreeRTOS 互斥锁句柄 / FreeRTOS mutex lock handle */
    static SemaphoreHandle_t mutexLock;
    
    /** 响应信号量 / Response semaphore */
    static SemaphoreHandle_t responseSemaphore;

    /** 队列相关静态变量 / Queue-related static variables */
    static QueueHandle_t dataQueue;
    static TaskHandle_t parseTaskHandle;

    /** 队列相关方法 / Queue-related methods */
    static void parseDataTask(void* parameter);
#endif

    /**
     * @brief Acquires the mutex lock for thread-safe operations.
     * 
     * This function acquires the FreeRTOS mutex lock to ensure thread-safe access to shared resources.
     * Creates the mutex if it doesn't exist. Blocks until the lock is acquired.
     * Only effective on ESP32/ESP8266 platforms with FreeRTOS support.
     */
    void acquireMutex();

    /**
     * @brief Releases the mutex lock.
     * 
     * This function releases the previously acquired FreeRTOS mutex lock, allowing other tasks
     * to access the shared resources. Should be called after acquireMutex() and corresponding operations.
     * Only effective on ESP32/ESP8266 platforms with FreeRTOS support.
     */
    void releaseMutex();
    
    /**
     * @brief Processes received serial data and adds it to the receive buffer.
     * 
     * This function handles incoming serial data by adding it to the internal receive buffer,
     * updating timestamps for packet interval detection, and setting flags for packet parsing.
     * Includes debug output when enabled and handles buffer overflow protection.
     * 
     * @param data Pointer to the received data buffer.
     * @param length Number of bytes received.
     */
    void processReceivedData(uint8_t* data, size_t length);
    
    /**
     * @brief Checks for packet interval timeout and parses complete packets.
     * 
     * This function implements packet interval-based parsing by checking if enough time has passed
     * since the last data reception. When the interval timeout is reached, it attempts to parse
     * all complete packets from the receive buffer and adds them to the parsed packet queue.
     * Also handles response matching for waiting operations.
     */
    void checkAndParsePackets();
    
    /**
     * @brief Extracts a single complete packet from the buffer.
     * 
     * This function precisely parses one complete fingerprint packet from the given buffer,
     * validating the start code, packet structure, checksum, and address. Returns the exact
     * packet length if a valid packet is found.
     * 
     * @param buffer Pointer to the data buffer to parse.
     * @param bufferSize Size of the data buffer.
     * @param packetLength Reference to store the length of the extracted packet.
     * @return true if a valid packet was extracted, false otherwise.
     */
    bool extractSinglePacket(uint8_t* buffer, size_t bufferSize, size_t& packetLength);
    
    /**
     * @brief Adds a parsed packet to the internal packet queue.
     * 
     * This function adds a complete, validated packet to the internal queue for later retrieval.
     * Manages queue overflow by removing the oldest packet when the queue is full.
     * Each packet is timestamped for expiration tracking.
     * 
     * @param packetData Pointer to the complete packet data.
     * @param packetLength Length of the packet data.
     */
    void addParsedPacket(const uint8_t* packetData, size_t packetLength);
    
    /**
     * @brief Retrieves a matching packet from the parsed packet queue.
     * 
     * This function searches the internal packet queue for a packet matching the expected
     * criteria (typically ACK, DATA, or END_DATA packets). When found, constructs a
     * Fingerprint_Packet object and removes the packet from the queue.
     * 
     * @param packet Reference to a Fingerprint_Packet object to store the found packet.
     * @return true if a matching packet was found and retrieved, false otherwise.
     */
    bool getMatchingPacket(Fingerprint_Packet& packet);
    
    /**
     * @brief Removes expired packets from the internal queue.
     * 
     * This function performs housekeeping by removing packets that have exceeded the
     * timeout threshold (typically 5 seconds) from the internal packet queue.
     * Prevents memory leaks and ensures the queue doesn't grow indefinitely.
     */
    void cleanupExpiredPackets();
    
    /**
     * @brief Checks if a packet is a wakeup packet and handles it accordingly.
     * 
     * This function checks if the given packet matches the wakeup packet format:
     * EF 01 FF FF FF FF 07 00 03 FF 01 09
     * If it is a wakeup packet, calls the wakeup callback function instead of 
     * adding it to the parsed packet queue.
     * 
     * @param packetData Pointer to the complete packet data.
     * @param packetLength Length of the packet data.
     * @return true if this is a wakeup packet (and callback was called), false otherwise.
     */
    bool handleWakeupPacket(const uint8_t* packetData, size_t packetLength);
    
    /**
     * @brief Default wakeup callback function that logs the wakeup event.
     * 
     * This function serves as the default callback when no custom callback is set.
     * It prints debug information about the wakeup packet reception.
     * 
     * @param wakeupPacket Pointer to the wakeup packet data.
     * @param packetLength Length of the wakeup packet.
     */
    static void defaultWakeupCallback(const uint8_t* wakeupPacket, size_t packetLength);
    
    /**
     * @brief Legacy packet parsing method (simplified version).
     * 
     * This function provides a simpler packet parsing approach for compatibility.
     * Searches for start codes, validates packet structure, and constructs response packets.
     * Less robust than the interval-based parsing but useful for fallback scenarios.
     * 
     * @return true if a complete packet was parsed successfully, false otherwise.
     */
    bool tryParsePacket();
    
    /**
     * @brief Sends a packet through the serial interface.
     * 
     * This function serializes a Fingerprint_Packet object and transmits it via the
     * configured serial port (Arduino HardwareSerial). Includes
     * debug output when enabled and mutex protection for thread safety.
     * 
     * @param packet The Fingerprint_Packet object to send.
     * @return true if the packet was sent successfully, false otherwise.
     */
    bool sendPacketData(const Fingerprint_Packet& packet);

    /**
     * @brief Receives a packet from the serial interface with timeout.
     * 
     * This function waits for a response packet using the parsed packet queue mechanism.
     * On FreeRTOS platforms, uses semaphores for efficient waiting. On other platforms,
     * uses polling with the specified timeout. Handles response matching and cleanup.
     * 
     * @param packet Reference to a Fingerprint_Packet object to store the received packet.
     * @param timeout_ms Timeout in milliseconds to wait for the packet (default: 1000ms).
     * @return true if a packet was received within the timeout, false otherwise.
     */
    bool receivePacketData(Fingerprint_Packet& packet, uint32_t timeout_ms = 1000);
};




#endif // __M5_UNIT_FINGERPRINT2_H
/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef __M5_UNIT_FINGERPRINT2_DEFS_H
#define __M5_UNIT_FINGERPRINT2_DEFS_H

#include "Arduino.h"

#define M5_DEVICE_NAME "M5UnitFingerprint2"

#define M5_MODULE_DEBUG_SERIAL Serial0 // The corresponding serial port must be initialized before use
// This macro definition can be annotated without sending and receiving data prints
// Define the serial port you want to use, e.g., Serial1 or Serial2
// 是否启用原始数据分割打印 printPacketHex
// #define M5_MODULE_DEBUG_SERIAL_ENABLED true
// 是否启用命令解析
// #define M5_MODULE_COMMAND_PARSING_ENABLED true

#define FORMAT_TIMESTAMP() ([]() -> String { \
    unsigned long ms = millis(); \
    unsigned long total_seconds = ms / 1000; \
    unsigned long hours = total_seconds / 3600; \
    unsigned long minutes = (total_seconds % 3600) / 60; \
    unsigned long seconds = total_seconds % 60; \
    float sec_frac = seconds + (ms % 1000) / 1000.0f; \
    char buf[20]; \
    snprintf(buf, sizeof(buf), "%02luh:%02lum:%.3lfs\0", hours, minutes, sec_frac); \
    return String(buf); \
})()

#if defined M5_MODULE_DEBUG_SERIAL
#define serialPrint(...)   do { \
    char _msg_buf[256]; \
    String _timestamp = FORMAT_TIMESTAMP(); \
    snprintf(_msg_buf, sizeof(_msg_buf), "[at %s] %s: ", _timestamp.c_str(), M5_DEVICE_NAME); \
    M5_MODULE_DEBUG_SERIAL.print(_msg_buf); \
    M5_MODULE_DEBUG_SERIAL.print(__VA_ARGS__); \
    M5_MODULE_DEBUG_SERIAL.flush(); \
} while(0)

#define serialPrintln(...) do { \
    char _msg_buf[256]; \
    String _timestamp = FORMAT_TIMESTAMP(); \
    snprintf(_msg_buf, sizeof(_msg_buf), "[at %s] %s: ", _timestamp.c_str(), M5_DEVICE_NAME); \
    M5_MODULE_DEBUG_SERIAL.print(_msg_buf); \
    M5_MODULE_DEBUG_SERIAL.println(__VA_ARGS__); \
    M5_MODULE_DEBUG_SERIAL.flush(); \
} while(0)

#define serialPrintf(...)  do { \
    char _msg_buf[256]; \
    String _timestamp = FORMAT_TIMESTAMP(); \
    snprintf(_msg_buf, sizeof(_msg_buf), "[at %s] %s: ", _timestamp.c_str(), M5_DEVICE_NAME); \
    M5_MODULE_DEBUG_SERIAL.print(_msg_buf); \
    M5_MODULE_DEBUG_SERIAL.printf(__VA_ARGS__); \
    M5_MODULE_DEBUG_SERIAL.flush(); \
} while(0)

#define serialFlush()      M5_MODULE_DEBUG_SERIAL.flush()
#else
#define serialPrint(...)
#define serialPrintln(...)
#define serialPrintf(...)
#define serialFlush()
#endif

// 指纹传感器常量定义
#define FINGERPRINT_STARTCODE 0xEF01    // 指纹传感器数据包起始码
#define FINGERPRINT_MAX_PACKET_SIZE 256 // 指纹传感器数据包最大长度
#define FINGERPRINT_RECV_BUFFER_SIZE 16384 // 接收缓冲区大小，应该能容纳多个连续数据包
#define MAX_PARSED_PACKETS 256 // 最大解析包数量
#define PACKET_INTERVAL_MS 500 // 包间隔时间（毫秒）

// 包标识
#define FINGERPRINT_PACKET_COMMANDPACKET 0x1  //!< Command packet
#define FINGERPRINT_PACKET_DATAPACKET    0x2  //!< Data packet, must follow command packet or acknowledge packet
#define FINGERPRINT_PACKET_ACKPACKET     0x7  //!< Acknowledge packet
#define FINGERPRINT_PACKET_ENDDATAPACKET 0x8  //!< End of data packet



// 命令代码
#define FINGERPRINT_GET_IMAGE          0x01  //!< 验证用获取图像 Collect finger image
#define FINGERPRINT_GENERATE_CHARACTER 0x02  //!< 根据原始图像生成指纹特征文件存于模板缓冲区 Generate character file from image
#define FINGERPRINT_MATCH              0x03  //!< 精确比对模板缓冲区中的特征文件或者模板 Match fingerprint
#define FINGERPRINT_SEARCH             0x04  //!< 以模板缓冲区中的特征文件搜索整个或部分指纹库 Search for fingerprint in slot
#define FINGERPRINT_REG_MODEL          0x05  //!< 将特征文件融合后生成一个模板 Combine character files and generate template
#define FINGERPRINT_STORE              0x06  //!< 将模板缓冲区中的模板文件储存到 flash 指纹库中 Store template
#define FINGERPRINT_LOAD_MODEL         0x07  //!< 从 flash 指纹库中读取一个模板到模板缓冲区 Read/load template
#define FINGERPRINT_UPLOAD_MODEL       0x08  //!< 将模板缓冲区中的模板文件上传给主控 Upload template
#define FINGERPRINT_DOWNLOAD_MODEL     0x09  //!< 主控下载一个模板文件到模板缓冲区 Download character file
#define FINGERPRINT_UPLOAD_IMAGE       0x0A  //!< 上传原始图像 Upload image
#define FINGERPRINT_DOWNLOAD_IMAGE     0x0B  //!< 下载原始图像 Download image
#define FINGERPRINT_DELETE_MODEL       0x0C  //!< 删除 flash 指纹库中的一个模板文件 Delete templates
#define FINGERPRINT_EMPTY              0x0D  //!< 清空 flash 指纹库 Empty library
#define FINGERPRINT_WRITE_REG          0x0E  //!< 写 SOC 系统寄存器 Write system register instruction
#define FINGERPRINT_READ_SYSTEM_PARAM  0x0F  //!< 读系统基本参数 Read system parameters
#define FINGERPRINT_SET_PASSWORD       0x12  //!< 设置设备握手口令 Sets passwords
#define FINGERPRINT_VERIFY_PASSWORD    0x13  //!< 验证设备握手口令 Verifies the password
#define FINGERPRINT_GET_RANDOM_CODE    0x14  //!< 采样随机数 Generate random code
#define FINGERPRINT_SET_CHIP_ADDR      0x15  //!< 设置设备地址 Set chip address
#define FINGERPRINT_READ_INFO_PAGE     0x16  //!< 读取 FLASH Information Page 内容 Read information page
#define FINGERPRINT_WRITE_NOTEPAD      0x18  //!< 写记事本 Write notepad
#define FINGERPRINT_READ_NOTEPAD       0x19  //!< 读记事本 Read notepad
#define FINGERPRINT_VALID_MODEL_COUNT  0x1D  //!< 读有效模板个数 Read finger valid template numbers
#define FINGERPRINT_READ_INDEX_TABLE   0x1F  //!< 读索引表 Read index table
#define FINGERPRINT_GET_ENROLL_IMAGE   0x29  //!< 注册用获取图像 Get enroll image

#define FINGERPRINT_CANCEL_AUTO_FLOW 0x30  //!< Cancel auto enroll or auto identify
#define FINGERPRINT_AUTO_ENROLL      0x31  //!< Auto enroll
#define FINGERPRINT_AUTO_IDENTIFY    0x32  //!< Auto identify

#define FINGERPRINT_CHIP_SN        0x34  //!< 获取芯片唯一序列号 Read chip serial number
#define FINGERPRINT_HAND_SHAKE     0x35  //!< 握手指令 Check module is working
#define FINGERPRINT_CHECK_SENSOR   0x36  //!< 校验传感器 Check sensor is working
#define FINGERPRINT_RESET_SETTINGS 0x3B  //!< 恢复出厂设置 Reset settings to factory defaults
#define FINGERPRINT_CONTROL_LED    0x3C  //!< 呼吸灯指令 Control LED
#define FINGERPRINT_GET_IMAGE_INFO 0x3D  //!< 获取图像信息 Get image information
#define FINGERPRINT_SEARCH_NOW     0x3E  //!< 搜索当前指纹 Search now
#define FINGERPRINT_UP_TEMPLATE    0x7A  //!< 特殊上传模板 Upload template
#define FINGERPRINT_DOWN_TEMPLATE  0x7B  //!< 特殊下载模板 Download template

#define FINGERPRINT_SET_SLEEP_TIME          0xD0  //!< 设置休眠时间 SET the sleep time
#define FINGERPRINT_GET_SLEEP_TIME          0xD1  //!< 获取休眠时间 GET the sleep time
#define FINGERPRINT_SET_WORK_MODE           0xD2  //!< 设置工作模式 SET the work mode
#define FINGERPRINT_GET_WORK_MODE           0xD3  //!< 获取工作模式 GET the work mode
#define FINGERPRINT_ACTIVATE_MODULE         0xD4  //!< 激活模块 Activate module
#define FINGERPRINT_GET_MODULE_STATUS       0xD5  //!< 获取模块状态 Get module status
#define FINGERPRINT_SAVE_CONF_TO_FLASH      0xD6  //!< 保存配置到 Flash Save configuration to flash
#define FINGERPRINT_GET_FINGERPRINT_VERSION 0xD7  //!< 获取指纹模块版本 Get fingerprint module version

//安全相关
#define FINGERPRINT_GET_KEYT             0xE0  //!< Get key token
#define FINGERPRINT_LOCK_KEYT            0xE1  //!< Lock key token
#define FINGERPRINT_GET_CIPHERTEXT       0xE2  //!< Get ciphertext
#define FINGERPRINT_SECURITY_STORE_CHAR  0xE3  //!< Store character
#define FINGERPRINT_SECURITY_SEARCH      0xE4  //!< Search character

#define FINGERPRINT_REG_ADDR_ERROR 0x1A  //!< shows register address error

//
#define FINGERPRINT_BAUD_REG_ADDR   0x4  //!< BAUDRATE register address
#define FINGERPRINT_BAUDRATE_9600   0x1  //!< UART baud 9600
#define FINGERPRINT_BAUDRATE_19200  0x2  //!< UART baud 19200
#define FINGERPRINT_BAUDRATE_28800  0x3  //!< UART baud 28800
#define FINGERPRINT_BAUDRATE_38400  0x4  //!< UART baud 38400
#define FINGERPRINT_BAUDRATE_48000  0x5  //!< UART baud 48000
#define FINGERPRINT_BAUDRATE_57600  0x6  //!< UART baud 57600
#define FINGERPRINT_BAUDRATE_67200  0x7  //!< UART baud 67200
#define FINGERPRINT_BAUDRATE_76800  0x8  //!< UART baud 76800
#define FINGERPRINT_BAUDRATE_86400  0x9  //!< UART baud 86400
#define FINGERPRINT_BAUDRATE_96000  0xA  //!< UART baud 96000
#define FINGERPRINT_BAUDRATE_105600 0xB  //!< UART baud 105600
#define FINGERPRINT_BAUDRATE_115200 0xC  //!< UART baud 115200

#define FINGERPRINT_SECURITY_REG_ADDR 0x5  //!< Security level register address
// The safety level is 1 The highest rate of false recognition , The rejection
// rate is the lowest . The safety level is 5 The lowest tate of false
// recognition, The rejection rate is the highest .
#define FINGERPRINT_SECURITY_LEVEL_1 0X1  //!< Security level 1
#define FINGERPRINT_SECURITY_LEVEL_2 0X2  //!< Security level 2
#define FINGERPRINT_SECURITY_LEVEL_3 0X3  //!< Security level 3
#define FINGERPRINT_SECURITY_LEVEL_4 0X4  //!< Security level 4
#define FINGERPRINT_SECURITY_LEVEL_5 0X5  //!< Security level 5

#define FINGERPRINT_PACKET_REG_ADDR 0x6  //!< Packet size register address
#define FINGERPRINT_PACKET_SIZE_32  0X0  //!< Packet size is 32 Byte
#define FINGERPRINT_PACKET_SIZE_64  0X1  //!< Packet size is 64 Byte
#define FINGERPRINT_PACKET_SIZE_128 0X2  //!< Packet size is 128 Byte
#define FINGERPRINT_PACKET_SIZE_256 0X3  //!< Packet size is 256 Byte


// 状态代码枚举
typedef enum {
    FINGERPRINT_OK           = 0x00,  // 指令执行完毕或 OK //!< Command execution is complete
    FINGERPRINT_PACKET_ERROR = 0x01,  // 数据包接收错误 //!< Error when receiving data package
    FINGERPRINT_NO_FINGER    = 0x02,  // 传感器上没有手指 //!< No finger on the sensor
    FINGERPRINT_IMAGE_FAIL   = 0x03,  // 录入指纹图像失败 //!< Failed to enroll the finger
    FINGERPRINT_IMAGE_DRY    = 0x04,  // 指纹图像太干、太淡而生不成特征
    FINGERPRINT_IMAGE_WET    = 0x05,  // 指纹图像太湿、太糊而生不成特征
    FINGERPRINT_IMAGE_MESSY  = 0x06,  // 指纹图像太乱而生不成特征  //!< Failed to generate character file due to overly
                                      // disorderly fingerprint image
    FINGERPRINT_FEATURE_TOO_FEW = 0x07,  // 指纹图像正常，但特征点太少（或面积太小） //!< Failed to generate character
                                         // file due to the lack of character point  or small fingerprint image
    FINGERPRINT_NOT_MATCH  = 0x08,       // 指纹不匹配 //!< Finger doesn't match
    FINGERPRINT_NOT_FOUND  = 0x09,       // 没搜索到指纹 //!< Failed to find matching finger
    FINGERPRINT_MERGE_FAIL = 0x0A,       // 特征合并失败 //!< Failed to combine the character files
    FINGERPRINT_ADDR_OVERFLOW = 0x0B,  // 访问指纹库时地址序号超出指纹库范围 //!< Addressed PageID is beyond the finger library
    FINGERPRINT_READ_TEMPLATE_FAIL = 0x0C,  // 从指纹库读模板出错或无效 //!< Error when reading template from library or invalid template
    FINGERPRINT_UPLOAD_FEATURE_FAIL = 0x0D,  // 上传特征失败 //!< Error when uploading template
    FINGERPRINT_RECEIVE_ERROR = 0x0E,  // 模块不能接收后续数据包 //!< Module failed to receive the following data packages
    FINGERPRINT_UPLOAD_IMAGE_FAIL = 0x0F,  // 上传图像失败 //!< Error when uploading image
    FINGERPRINT_DELETE_FAIL       = 0x10,  // 删除模板失败 /!< Failed to delete the template
    FINGERPRINT_CLEAR_FAIL        = 0x11,  // 清空指纹库失败 //!< Failed to clear finger library
    FINGERPRINT_LOW_POWER_FAIL    = 0x12,  // 不能进入低功耗状态
    FINGERPRINT_PASSWORD_ERROR    = 0x13,  // 口令不正确 //!< Find whether the fingerprint passed or failed
    FINGERPRINT_RESET_FAIL        = 0x14,  // 系统复位失败
    FINGERPRINT_NO_VALID_IMAGE = 0x15,  // 缓冲区内没有有效原始图而生不成图像 //!< Failed to generate image because of lac of valid primary image
    FINGERPRINT_UPGRADE_FAIL           = 0x16,  // 在线升级失败
    FINGERPRINT_REMAINING_FINGER       = 0x17,  // 残留指纹或两次采集之间手指没有移动过
    FINGERPRINT_FLASH_RW_ERROR         = 0x18,  // 读写 FLASH 出错 //!< Error when writing flash
    FINGERPRINT_RANDOM_FAIL            = 0x19,  // 随机数生成失败
    FINGERPRINT_INVALID_REGISTER       = 0x1A,  // 无效寄存器号 //!< Invalid register number
    FINGERPRINT_REGISTER_ERROR         = 0x1B,  // 寄存器设定内容错误
    FINGERPRINT_NOTEBOOK_PAGE_ERROR    = 0x1C,  // 记事本页码指定错误
    FINGERPRINT_PORT_ERROR             = 0x1D,  // 端口操作失败
    FINGERPRINT_AUTO_ENROLL_FAIL       = 0x1E,  // 自动注册（enroll）失败
    FINGERPRINT_DATABASE_FULL          = 0x1F,  // 指纹库满
    FINGERPRINT_DEVICE_ADDRESS_ERROR   = 0x20,  // 设备地址错误 //!< Address code
    FINGERPRINT_WRONG_PASSWORD         = 0x21,  // 密码有误 //!< Verify the fingerprint passed
    FINGERPRINT_TEMPLATE_NOT_EMPTY     = 0x22,  // 指纹模板非空
    FINGERPRINT_TEMPLATE_EMPTY         = 0x23,  // 指纹模板为空
    FINGERPRINT_LIBRARY_EMPTY          = 0x24,  // 指纹库为空
    FINGERPRINT_ENROLL_COUNT_ERROR     = 0x25,  // 录入次数设置错误
    FINGERPRINT_TIMEOUT                = 0x26,  // 超时
    FINGERPRINT_ALREADY_EXISTS         = 0x27,  // 指纹已存在
    FINGERPRINT_FEATURE_ASSOCIATED     = 0x28,  // 指纹特征有关联
    FINGERPRINT_SENSOR_INIT_FAIL       = 0x29,  // 传感器初始化失败
    FINGERPRINT_MODULE_INFO_NOT_EMPTY  = 0x2A,  // 模组信息非空
    FINGERPRINT_MODULE_INFO_EMPTY      = 0x2B,  // 模组信息为空
    FINGERPRINT_OTP_FAIL               = 0x2C,  // OTP 操作失败
    FINGERPRINT_KEY_GEN_FAIL           = 0x2D,  // 密钥生成失败
    FINGERPRINT_KEY_NOT_EXIST          = 0x2E,  // 密钥不存在
    FINGERPRINT_ALGORITHM_FAIL         = 0x2F,  // 安全算法执行失败
    FINGERPRINT_ALGORITHM_RESULT_ERROR = 0x30,  // 安全算法加解密结果有误
    FINGERPRINT_FUNCTION_MISMATCH      = 0x31,  // 功能与加密等级不匹配
    FINGERPRINT_KEY_LOCKED             = 0x32,  // 密钥已锁定
    FINGERPRINT_IMAGE_TOO_SMALL        = 0x33,  // 图像面积小
    FINGERPRINT_IMAGE_STATIC_OBJECT    = 0x34,  // 图像中静态异物（Orange）
    FINGERPRINT_ILLEGAL_DATA           = 0x35,  // 非法数据

    FINGERPRINT_PACKET_TIMEOUT         = 0xF9,  // 收包超时
    FINGERPRINT_PACKET_BADPACKET       = 0xFA,  // 错误包 （如数据未接收完成，接收到其他包）
    FINGERPRINT_PACKET_OVERFLOW        = 0xFB,  // 数据包溢出（如数据包长度超过最大长度）

    FINGERPRINT_OPERATION_BLOCKED      = 0xFC,  // 该操作被屏蔽
    FINGERPRINT_PARAM_ERROR            = 0xFD,  // 参数错误
    FINGERPRINT_MODULE_NOT_OPEN        = 0xFE,  // 指纹模组未打开
    FINGERPRINT_PASSIVE_ACTIVATION     = 0xFF   // 被动激活
} fingerprint_status_t;
// 补充包标识

// 寄存器枚举
typedef enum {
    FINGERPRINT_REG_DELAY_TIME         = 0x00,
    FINGERPRINT_REG_ENROLL_NUM         = 0x01,
    FINGERPRINT_REG_IMAGE_FORMAT       = 0x02,
    FINGERPRINT_REG_ENROLL_LOGIC       = 0x03,
    FINGERPRINT_REG_INTERNAL_BAUD_RATE = 0x04,
    FINGERPRINT_REG_COMPARE_THRESHOLD  = 0x05,
    FINGERPRINT_REG_PACKET_SIZE        = 0x06,
    FINGERPRINT_REG_ENCRYPTION_LEVEL   = 0x07,
    FINGERPRINT_REG_PRODUCT_MODUEL     = 0x0A,
    FINGERPRINT_REG_AUTO_LIGHT         = 0x0B,
} fingerprint_reg_t;

// 队列数据结构
typedef struct {
    uint8_t* data;
    size_t length;
    unsigned long timestamp;
} QueuedData;

// 自动注册回调函数类型定义
// 参数：
// - ID: 注册的指纹ID
// - confirmationCode: 当前状态 (fingerprint_status_t)
// - param1: 参数1 (ps_auto_enroll_param1_t)
// - param2: 参数2 (ps_auto_enroll_param2_t)
// - packetCount: 已接收到的数据包数量
// 返回值：true 继续执行，false 中止执行
typedef bool (*PS_AutoEnrollCallback_t)(uint16_t ID, 
                                        fingerprint_status_t confirmationCode, 
                                        uint8_t param1, 
                                        uint8_t param2, 
                                        int packetCount);

// PS_AutoIdentify 回调函数类型定义
// 用于 PS_AutoIdentify 自动识别过程中的状态回调
// 参数：
//   securityLevel: 安全等级
//   confirmationCode: 确认码/状态码
//   param: 阶段参数 (ps_auto_identify_param_t)
//   matchedID: 匹配到的指纹ID (仅在匹配成功时有效)
//   matchScore: 匹配分数 (仅在匹配成功时有效)
//   packetCount: 当前包序号
// 返回值：true 继续处理，false 中止处理
typedef bool (*PS_AutoIdentifyCallback_t)(uint8_t securityLevel,
                                          fingerprint_status_t confirmationCode,
                                          uint8_t param,
                                          uint16_t matchedID,
                                          uint16_t matchScore,
                                          int packetCount);

// PS_UploadTemplateAuto 回调函数类型定义
// 用于 PS_UploadTemplateAuto 自动上传模板过程中的进度回调
// 参数：
//   currentSize: 当前接收到的数据大小
//   totalSize: 已累计接收的总数据大小
//   packetCount: 当前数据包序号
//   isComplete: 是否完成上传 (当前包是否为最后一包)
// 返回值：true 继续处理，false 中止处理
typedef bool (*PS_UploadTemplateCallback_t)(uint16_t currentSize,
                                            uint32_t totalSize,
                                            int packetCount,
                                            bool isComplete);

// PS_DownloadTemplateAuto 回调函数类型定义
// 用于 PS_DownloadTemplateAuto 自动下载模板过程中的进度回调
// 参数：
//   currentSize: 当前发送的数据大小
//   totalSize: 已累计发送的总数据大小
//   packetCount: 当前数据包序号
//   isComplete: 是否完成下载 (当前包是否为最后一包)
// 返回值：true 继续处理，false 中止处理
typedef bool (*PS_DownloadTemplateCallback_t)(uint16_t currentSize,
                                              uint32_t totalSize,
                                              int packetCount,
                                              bool isComplete);

#endif // __M5_UNIT_FINGERPRINT2_DEFS_H

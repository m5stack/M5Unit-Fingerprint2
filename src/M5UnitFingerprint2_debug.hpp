/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef __M5_UNIT_FINGERPRINT2_DEBUG_H
#define __M5_UNIT_FINGERPRINT2_DEBUG_H

#include "M5UnitFingerprint2_defs.hpp"

// 前置声明 / Forward declaration
class Fingerprint_Packet;

#if defined M5_MODULE_DEBUG_SERIAL

/**
 * @brief 调试工具类 / Debug utility class
 * 提供指纹传感器通信调试功能 / Provides fingerprint sensor communication debugging functionality
 */
class FingerprintDebugUtils {
public:
    /**
     * @brief 打印数据包的十六进制格式 (用于调试) / Print packet in hexadecimal format (for debugging)
     * @param prefix 前缀字符串 / Prefix string
     * @param buffer 数据缓冲区 / Data buffer
     * @param length 数据长度 / Data length
     */
    static void printPacketHex(const char* prefix, const uint8_t* buffer, size_t length);
    
    /**
     * @brief 打印数据包结构化信息 (用于调试) / Print packet structured information (for debugging)
     * @param prefix 前缀字符串 / Prefix string
     * @param packet 数据包对象 / Packet object
     */
    static void printPacketStructure(const char* prefix, const Fingerprint_Packet& packet);
    
    /**
     * @brief 解析并打印命令名称 (用于调试) / Parse and print command name (for debugging)
     * @param command 命令代码 / Command code
     * @return 命令名称字符串 / Command name string
     */
    static String getCommandName(uint8_t command);
    
    /**
     * @brief 解析并打印状态码含义 (用于调试) / Parse and print status code meaning (for debugging)
     * @param status 状态代码 / Status code
     * @return 状态含义字符串 / Status meaning string
     */
    static String getStatusName(uint8_t status);
    
    /**
     * @brief 解析PS_AutoEnroll参数1的含义 (用于调试) / Parse PS_AutoEnroll parameter 1 meaning (for debugging)
     * @param param1 参数1值 / Parameter 1 value
     * @return 参数1含义字符串 / Parameter 1 meaning string
     */
    static String getAutoEnrollParam1Name(uint8_t param1);
    
    /**
     * @brief 解析PS_AutoEnroll参数2的含义 (用于调试) / Parse PS_AutoEnroll parameter 2 meaning (for debugging)
     * @param param2 参数2值 / Parameter 2 value
     * @return 参数2含义字符串 / Parameter 2 meaning string
     */
    static String getAutoEnrollParam2Name(uint8_t param2);
    
    /**
     * @brief 解析PS_AutoIdentify参数的含义 (用于调试) / Parse PS_AutoIdentify parameter meaning (for debugging)
     * @param param 参数值 / Parameter value
     * @return 参数含义字符串 / Parameter meaning string
     */
    static String getAutoIdentifyParamName(uint8_t param);
};

#endif // M5_MODULE_DEBUG_SERIAL

#endif // __M5_UNIT_FINGERPRINT2_DEBUG_H

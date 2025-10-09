/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef __M5_UNIT_FINGERPRINT2_DEBUG_H
#define __M5_UNIT_FINGERPRINT2_DEBUG_H

#include "M5UnitFingerprint2_defs.hpp"

// 前置声明
class Fingerprint_Packet;

#if defined M5_MODULE_DEBUG_SERIAL

/**
 * @brief 调试工具类
 * 提供指纹传感器通信调试功能
 */
class FingerprintDebugUtils {
public:
    /**
     * @brief 打印数据包的十六进制格式 (用于调试)
     * @param prefix 前缀字符串
     * @param buffer 数据缓冲区
     * @param length 数据长度
     */
    static void printPacketHex(const char* prefix, const uint8_t* buffer, size_t length);
    
    /**
     * @brief 打印数据包结构化信息 (用于调试)
     * @param prefix 前缀字符串
     * @param packet 数据包对象
     */
    static void printPacketStructure(const char* prefix, const Fingerprint_Packet& packet);
    
    /**
     * @brief 解析并打印命令名称 (用于调试)
     * @param command 命令代码
     * @return 命令名称字符串
     */
    static String getCommandName(uint8_t command);
    
    /**
     * @brief 解析并打印状态码含义 (用于调试)
     * @param status 状态代码
     * @return 状态含义字符串
     */
    static String getStatusName(uint8_t status);
    
    /**
     * @brief 解析PS_AutoEnroll参数1的含义 (用于调试)
     * @param param1 参数1值
     * @return 参数1含义字符串
     */
    static String getAutoEnrollParam1Name(uint8_t param1);
    
    /**
     * @brief 解析PS_AutoEnroll参数2的含义 (用于调试)
     * @param param2 参数2值
     * @return 参数2含义字符串
     */
    static String getAutoEnrollParam2Name(uint8_t param2);
    
    /**
     * @brief 解析PS_AutoIdentify参数的含义 (用于调试)
     * @param param 参数值
     * @return 参数含义字符串
     */
    static String getAutoIdentifyParamName(uint8_t param);
};

#endif // M5_MODULE_DEBUG_SERIAL

#endif // __M5_UNIT_FINGERPRINT2_DEBUG_H

/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "m5_unit_fingerprint2.hpp"

// 设置休眠时间
fingerprint_status_t M5UnitFingerprint2::PS_SetSleepTime(uint8_t SleepTime) const
{
    // 验证睡眠时间范围 (10-254)
    if (SleepTime < 10 || SleepTime > 254) {
        serialPrintln("Invalid sleep time. Range: 10-254");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 创建命令数据包
    uint8_t params[] = {SleepTime};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_SET_SLEEP_TIME, params, 1);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_SetSleepTime command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_SetSleepTime response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_SetSleepTime");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_SetSleepTime");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Set sleep time result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 获取休眠时间
fingerprint_status_t M5UnitFingerprint2::PS_GeTSleepTime(uint8_t& SleepTime) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_SLEEP_TIME, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GeTSleepTime command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GeTSleepTime response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GeTSleepTime");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 2) {
        serialPrintln("Invalid response data length for PS_GeTSleepTime");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];
    SleepTime                   = responseData[1];

    serialPrintf("Get sleep time: %d [confirmation: %s]\r\n", SleepTime,
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 设置工作模式
fingerprint_status_t M5UnitFingerprint2::PS_SeTWorkMode(uint8_t WorkMode) const
{
    // 验证工作模式 (0:定时休眠模式 1:开启模式)
    if (WorkMode > 1) {
        serialPrintln("Invalid work mode. 0:timed sleep mode 1:active mode");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 创建命令数据包
    uint8_t params[] = {WorkMode};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_SET_WORK_MODE, params, 1);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_SeTWorkMode command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_SeTWorkMode response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_SeTWorkMode");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_SeTWorkMode");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Set work mode result: %s [mode: %s, confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 (WorkMode == 0) ? "Timed Sleep" : "Active",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 获取工作模式
fingerprint_status_t M5UnitFingerprint2::PS_GetWorkMode(uint8_t& WorkMode) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_WORK_MODE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetWorkMode command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetWorkMode response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetWorkMode");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 2) {
        serialPrintln("Invalid response data length for PS_GetWorkMode");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];
    WorkMode                    = responseData[1];

    serialPrintf("Get work mode: %s(0x%02X) [confirmation: %s]\r\n", (WorkMode == 0) ? "Timed Sleep" : "Active",
                 WorkMode, FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 激活指纹模块
fingerprint_status_t M5UnitFingerprint2::PS_ActivateFingerprintModule(void) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_ACTIVATE_MODULE, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_ActivateFingerprintModule command");
        return FINGERPRINT_PACKET_TIMEOUT;  // 返回错误状态
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_ActivateFingerprintModule response");
        return FINGERPRINT_PACKET_TIMEOUT;  // 返回错误状态
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_ActivateFingerprintModule");
        return FINGERPRINT_PACKET_BADPACKET;  // 返回错误状态
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_ActivateFingerprintModule");
        return FINGERPRINT_PACKET_OVERFLOW;  // 返回错误状态
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];  // 确认码 0x00:正确 0x01:收包有错 0xFD:参数错误

    serialPrintf("Module activation result: %s [confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 获取模块激活状态
fingerprint_status_t M5UnitFingerprint2::PS_GetFingerprintModuleStatus(uint8_t& ModuleStatus) const
{
    // 创建命令数据包
    uint8_t commandData[] = {FINGERPRINT_GET_MODULE_STATUS};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_MODULE_STATUS, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetFingerprintModuleStatus command");
        return FINGERPRINT_PACKET_TIMEOUT;  // 返回错误状态
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetFingerprintModuleStatus response");
        return FINGERPRINT_PACKET_TIMEOUT;  // 返回错误状态
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetFingerprintModuleStatus");
        return FINGERPRINT_PACKET_BADPACKET;  // 返回错误状态
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 2) {
        serialPrintln("Invalid response data length for PS_GetFingerprintModuleStatus");
        return FINGERPRINT_PACKET_OVERFLOW;  // 返回错误状态
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];  // 确认码 0x00:正确 0x01:收包有错 0xFD:参数错误
    ModuleStatus                = responseData[1];  // 模块状态 0x00:模块未开启 0x01:模块已开启

    serialPrintf("Module active status: %s(0x%02X) [confirmation: %s]\r\n",
                 (ModuleStatus == 0x01) ? "Active" : "Inactive", ModuleStatus,
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 保存配置到Flash
fingerprint_status_t M5UnitFingerprint2::PS_SaveConfigurationToFlash(uint8_t SaveOptions) const
{
    // 验证保存选项 (0:保存休眠时间 1:保存工作模式)
    if (SaveOptions > 1) {
        serialPrintln("Invalid save options. 0:save sleep time 1:save work mode");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 创建命令数据包
    uint8_t params[] = {SaveOptions};
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_SAVE_CONF_TO_FLASH, params, 1);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_SaveConfigurationToFlash command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_SaveConfigurationToFlash response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_SaveConfigurationToFlash");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 1) {
        serialPrintln("Invalid response data length for PS_SaveConfigurationToFlash");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];

    serialPrintf("Save configuration to flash result: %s [option: %s, confirmation: %s]\r\n",
                 (confirmationCode == FINGERPRINT_OK) ? "Success" : "Failed",
                 (SaveOptions == 0) ? "Sleep Time" : "Work Mode",
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

// 获取固件版本
fingerprint_status_t M5UnitFingerprint2::PS_GetFirmwareVersion(uint8_t& FwVersion) const
{
    // 创建命令数据包
    Fingerprint_Packet commandPacket =
        Fingerprint_Packet::new_command_packet(_fp2_address, FINGERPRINT_GET_FINGERPRINT_VERSION, nullptr, 0);

    // 发送命令包
    M5UnitFingerprint2* nonConstThis = const_cast<M5UnitFingerprint2*>(this);
    if (!nonConstThis->sendPacketData(commandPacket)) {
        serialPrintln("Failed to send PS_GetFirmwareVersion command");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 接收响应包
    Fingerprint_Packet responsePacket(FINGERPRINT_STARTCODE, 0, FINGERPRINT_PACKET_ACKPACKET, nullptr, 0);
    if (!nonConstThis->receivePacketData(responsePacket, 1000)) {
        serialPrintln("Failed to receive PS_GetFirmwareVersion response");
        return FINGERPRINT_PACKET_TIMEOUT;
    }

    // 检查响应包类型
    if (responsePacket.get_type() != FINGERPRINT_PACKET_ACKPACKET) {
        serialPrintln("Invalid response packet type for PS_GetFirmwareVersion");
        return FINGERPRINT_PACKET_BADPACKET;
    }

    // 检查数据长度
    if (responsePacket.get_actual_data_length() < 2) {
        serialPrintln("Invalid response data length for PS_GetFirmwareVersion");
        return FINGERPRINT_PACKET_OVERFLOW;
    }

    // 获取响应数据
    const uint8_t* responseData = responsePacket.get_data();
    uint8_t confirmationCode    = responseData[0];
    FwVersion                     = responseData[1];

    serialPrintf("Get firmware FwVersion: 0x%02X [confirmation: %s]\r\n", FwVersion,
                 FingerprintDebugUtils::getStatusName(confirmationCode).c_str());

    return static_cast<fingerprint_status_t>(confirmationCode);
}

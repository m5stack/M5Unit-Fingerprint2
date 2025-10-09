/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "M5UnitFingerprint2_debug.hpp"
#include "M5UnitFingerprint2.hpp"

#if defined M5_MODULE_DEBUG_SERIAL

void FingerprintDebugUtils::printPacketHex(const char* prefix, const uint8_t* buffer, size_t length) {
#if M5_MODULE_DEBUG_SERIAL_ENABLED
    serialPrint(String(prefix) + " Hex: ");
    
    if (length >= 9) {
        // 起始码 (2字节)
        M5_MODULE_DEBUG_SERIAL.printf("%02X %02X", buffer[0], buffer[1]);
        M5_MODULE_DEBUG_SERIAL.print(" | ");
        
        // 地址 (4字节) - 分别打印每个字节以确保显示
        for (int i = 2; i <= 5; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X", buffer[i]);
            if (i < 5) M5_MODULE_DEBUG_SERIAL.print(" ");
        }
        M5_MODULE_DEBUG_SERIAL.print(" | ");
        
        // 包类型 (1字节)
        M5_MODULE_DEBUG_SERIAL.printf("%02X", buffer[6]);
        M5_MODULE_DEBUG_SERIAL.print(" | ");
        
        // 包长度 (2字节)
        M5_MODULE_DEBUG_SERIAL.printf("%02X %02X", buffer[7], buffer[8]);
        M5_MODULE_DEBUG_SERIAL.print(" | ");
        
        // 数据部分 (可变长度)
        uint16_t dataLength = (buffer[7] << 8) | buffer[8];
        uint16_t actualDataLength = dataLength - 2; // 减去校验和长度
        
        if (actualDataLength > 0 && length >= 9 + actualDataLength) {
            for (uint16_t i = 0; i < actualDataLength; i++) {
                M5_MODULE_DEBUG_SERIAL.printf("%02X", buffer[9 + i]);
                if (i < actualDataLength - 1) {
                    M5_MODULE_DEBUG_SERIAL.print(" ");
                }
            }
            M5_MODULE_DEBUG_SERIAL.print(" | ");
        }
        
        // 校验和 (2字节)
        if (length >= 9 + dataLength) {
            size_t checksumPos = 9 + dataLength - 2;
            M5_MODULE_DEBUG_SERIAL.printf("%02X %02X", buffer[checksumPos], buffer[checksumPos + 1]);
        }
    } else {
        // 如果数据包太短，按原来的方式显示
        for (size_t i = 0; i < length; i++) {
            M5_MODULE_DEBUG_SERIAL.printf("%02X", buffer[i]);
            if (i < length - 1) {
                M5_MODULE_DEBUG_SERIAL.print(" | ");
            }
        }
    }
    
    M5_MODULE_DEBUG_SERIAL.println();
    serialFlush();
#endif // M5_MODULE_DEBUG_SERIAL_ENABLED
}

void FingerprintDebugUtils::printPacketStructure(const char* prefix, const Fingerprint_Packet& packet) {
#if M5_MODULE_COMMAND_PARSING_ENABLED
    serialPrint(String(prefix) + "Structure: ");
    
    // 打印包结构，用 | 分隔各个字段
    M5_MODULE_DEBUG_SERIAL.printf("StartCode:0x%04X | Address:0x%08lX | Type:0x%02X | Length:%d | Data:", 
        packet.get_start_code(), 
        packet.get_address(), 
        packet.get_type(), 
        packet.get_actual_data_length());
    
    // 打印数据部分
    const uint8_t* data = packet.get_data();
    for (uint16_t i = 0; i < packet.get_actual_data_length(); i++) {
        M5_MODULE_DEBUG_SERIAL.printf("%02X", data[i]);
        if (i < packet.get_actual_data_length() - 1) {
            M5_MODULE_DEBUG_SERIAL.print(" ");
        }
    }
    
    M5_MODULE_DEBUG_SERIAL.printf(" | Checksum:0x%04X", packet.get_checksum());
    M5_MODULE_DEBUG_SERIAL.println();
    
    // 另起一行解析结果
    serialPrint(String(prefix) + "Parsed: ");
    
    // 根据包类型解析内容
    switch (packet.get_type()) {
        case FINGERPRINT_PACKET_COMMANDPACKET:
            if (packet.get_actual_data_length() > 0) {
                uint8_t command = data[0];
                String cmdName = getCommandName(command);
                M5_MODULE_DEBUG_SERIAL.printf("Command Packet - Command: %s (0x%02X)", cmdName.c_str(), command);
                if (packet.get_actual_data_length() > 1) {
                    M5_MODULE_DEBUG_SERIAL.print(" | Params: ");
                    for (uint16_t i = 1; i < packet.get_actual_data_length(); i++) {
                        M5_MODULE_DEBUG_SERIAL.printf("0x%02X ", data[i]);
                    }
                }
            }
            break;
            
        case FINGERPRINT_PACKET_ACKPACKET:
            if (packet.get_actual_data_length() > 0) {
                uint8_t status = data[0];
                String statusName = getStatusName(status);
                M5_MODULE_DEBUG_SERIAL.printf("ACK Packet - Status: %s (0x%02X)", statusName.c_str(), status);
                if (packet.get_actual_data_length() > 1) {
                    M5_MODULE_DEBUG_SERIAL.print(" | Response Data: ");
                    for (uint16_t i = 1; i < packet.get_actual_data_length(); i++) {
                        M5_MODULE_DEBUG_SERIAL.printf("0x%02X ", data[i]);
                    }
                }
            }
            break;
            
        case FINGERPRINT_PACKET_DATAPACKET:
            M5_MODULE_DEBUG_SERIAL.print("Data Packet - Data: ");
            for (uint16_t i = 0; i < packet.get_actual_data_length(); i++) {
                M5_MODULE_DEBUG_SERIAL.printf("0x%02X ", data[i]);
            }
            break;
            
        case FINGERPRINT_PACKET_ENDDATAPACKET:
            M5_MODULE_DEBUG_SERIAL.print("End Data Packet - Data: ");
            for (uint16_t i = 0; i < packet.get_actual_data_length(); i++) {
                M5_MODULE_DEBUG_SERIAL.printf("0x%02X ", data[i]);
            }
            break;
            
        default:
            M5_MODULE_DEBUG_SERIAL.printf("Unknown Packet Type: 0x%02X", packet.get_type());
            break;
    }
    
    M5_MODULE_DEBUG_SERIAL.println();
    serialFlush();
#endif // M5_MODULE_COMMAND_PARSING_ENABLED
}

String FingerprintDebugUtils::getCommandName(uint8_t command) {
    switch (command) {
        case FINGERPRINT_GET_IMAGE: return "GET_IMAGE";
        case FINGERPRINT_GENERATE_CHARACTER: return "GENERATE_CHARACTER";
        case FINGERPRINT_MATCH: return "MATCH";
        case FINGERPRINT_SEARCH: return "SEARCH";
        case FINGERPRINT_REG_MODEL: return "REG_MODEL";
        case FINGERPRINT_STORE: return "STORE";
        case FINGERPRINT_LOAD_MODEL: return "LOAD_MODEL";
        case FINGERPRINT_UPLOAD_MODEL: return "UPLOAD_MODEL";
        case FINGERPRINT_DOWNLOAD_MODEL: return "DOWNLOAD_MODEL";
        case FINGERPRINT_UPLOAD_IMAGE: return "UPLOAD_IMAGE";
        case FINGERPRINT_DOWNLOAD_IMAGE: return "DOWNLOAD_IMAGE";
        case FINGERPRINT_DELETE_MODEL: return "DELETE_MODEL";
        case FINGERPRINT_EMPTY: return "EMPTY";
        case FINGERPRINT_WRITE_REG: return "WRITE_REG";
        case FINGERPRINT_READ_SYSTEM_PARAM: return "READ_SYSTEM_PARAM";
        case FINGERPRINT_SET_PASSWORD: return "SET_PASSWORD";
        case FINGERPRINT_VERIFY_PASSWORD: return "VERIFY_PASSWORD";
        case FINGERPRINT_GET_RANDOM_CODE: return "GET_RANDOM_CODE";
        case FINGERPRINT_SET_CHIP_ADDR: return "SET_CHIP_ADDR";
        case FINGERPRINT_READ_INFO_PAGE: return "READ_INFO_PAGE";
        case FINGERPRINT_WRITE_NOTEPAD: return "WRITE_NOTEPAD";
        case FINGERPRINT_READ_NOTEPAD: return "READ_NOTEPAD";
        case FINGERPRINT_VALID_MODEL_COUNT: return "VALID_MODEL_COUNT";
        case FINGERPRINT_READ_INDEX_TABLE: return "READ_INDEX_TABLE";
        case FINGERPRINT_GET_ENROLL_IMAGE: return "GET_ENROLL_IMAGE";
        case FINGERPRINT_CANCEL_AUTO_FLOW: return "CANCEL_AUTO_FLOW";
        case FINGERPRINT_AUTO_ENROLL: return "AUTO_ENROLL";
        case FINGERPRINT_AUTO_IDENTIFY: return "AUTO_IDENTIFY";
        case FINGERPRINT_CHIP_SN: return "CHIP_SN";
        case FINGERPRINT_HAND_SHAKE: return "HAND_SHAKE";
        case FINGERPRINT_CHECK_SENSOR: return "CHECK_SENSOR";
        case FINGERPRINT_RESET_SETTINGS: return "RESET_SETTINGS";
        case FINGERPRINT_CONTROL_LED: return "CONTROL_LED";
        case FINGERPRINT_GET_IMAGE_INFO: return "GET_IMAGE_INFO";
        case FINGERPRINT_SEARCH_NOW: return "SEARCH_NOW";
        case FINGERPRINT_UP_TEMPLATE: return "UP_TEMPLATE";
        case FINGERPRINT_DOWN_TEMPLATE: return "DOWN_TEMPLATE";
        case FINGERPRINT_SET_SLEEP_TIME: return "SET_SLEEP_TIME";
        case FINGERPRINT_GET_SLEEP_TIME: return "GET_SLEEP_TIME";
        case FINGERPRINT_SET_WORK_MODE: return "SET_WORK_MODE";
        case FINGERPRINT_GET_WORK_MODE: return "GET_WORK_MODE";
        case FINGERPRINT_ACTIVATE_MODULE: return "ACTIVATE_MODULE";
        case FINGERPRINT_GET_MODULE_STATUS: return "GET_MODULE_STATUS";
        case FINGERPRINT_SAVE_CONF_TO_FLASH: return "SAVE_CONF_TO_FLASH";
        case FINGERPRINT_GET_FINGERPRINT_VERSION: return "GET_FINGERPRINT_VERSION";
        case FINGERPRINT_GET_KEYT: return "GET_KEYT";
        case FINGERPRINT_LOCK_KEYT: return "LOCK_KEYT";
        case FINGERPRINT_GET_CIPHERTEXT: return "GET_CIPHERTEXT";
        case FINGERPRINT_SECURITY_STORE_CHAR: return "SECURITY_STORE_CHAR";
        case FINGERPRINT_SECURITY_SEARCH: return "SECURITY_SEARCH";
        default: return "UNKNOWN";
    }
}

String FingerprintDebugUtils::getStatusName(uint8_t status) {
    switch (status) {
        case FINGERPRINT_OK: return "OK";
        case FINGERPRINT_PACKET_ERROR: return "PACKET_ERROR";
        case FINGERPRINT_NO_FINGER: return "NO_FINGER";
        case FINGERPRINT_IMAGE_FAIL: return "IMAGE_FAIL";
        case FINGERPRINT_IMAGE_DRY: return "IMAGE_DRY";
        case FINGERPRINT_IMAGE_WET: return "IMAGE_WET";
        case FINGERPRINT_IMAGE_MESSY: return "IMAGE_MESSY";
        case FINGERPRINT_FEATURE_TOO_FEW: return "FEATURE_TOO_FEW";
        case FINGERPRINT_NOT_MATCH: return "NOT_MATCH";
        case FINGERPRINT_NOT_FOUND: return "NOT_FOUND";
        case FINGERPRINT_MERGE_FAIL: return "MERGE_FAIL";
        case FINGERPRINT_ADDR_OVERFLOW: return "ADDR_OVERFLOW";
        case FINGERPRINT_READ_TEMPLATE_FAIL: return "READ_TEMPLATE_FAIL";
        case FINGERPRINT_UPLOAD_FEATURE_FAIL: return "UPLOAD_FEATURE_FAIL";
        case FINGERPRINT_RECEIVE_ERROR: return "RECEIVE_ERROR";
        case FINGERPRINT_UPLOAD_IMAGE_FAIL: return "UPLOAD_IMAGE_FAIL";
        case FINGERPRINT_DELETE_FAIL: return "DELETE_FAIL";
        case FINGERPRINT_CLEAR_FAIL: return "CLEAR_FAIL";
        case FINGERPRINT_LOW_POWER_FAIL: return "LOW_POWER_FAIL";
        case FINGERPRINT_PASSWORD_ERROR: return "PASSWORD_ERROR";
        case FINGERPRINT_RESET_FAIL: return "RESET_FAIL";
        case FINGERPRINT_NO_VALID_IMAGE: return "NO_VALID_IMAGE";
        case FINGERPRINT_UPGRADE_FAIL: return "UPGRADE_FAIL";
        case FINGERPRINT_REMAINING_FINGER: return "REMAINING_FINGER";
        case FINGERPRINT_FLASH_RW_ERROR: return "FLASH_RW_ERROR";
        case FINGERPRINT_RANDOM_FAIL: return "RANDOM_FAIL";
        case FINGERPRINT_INVALID_REGISTER: return "INVALID_REGISTER";
        case FINGERPRINT_REGISTER_ERROR: return "REGISTER_ERROR";
        case FINGERPRINT_NOTEBOOK_PAGE_ERROR: return "NOTEBOOK_PAGE_ERROR";
        case FINGERPRINT_PORT_ERROR: return "PORT_ERROR";
        case FINGERPRINT_AUTO_ENROLL_FAIL: return "AUTO_ENROLL_FAIL";
        case FINGERPRINT_DATABASE_FULL: return "DATABASE_FULL";
        case FINGERPRINT_DEVICE_ADDRESS_ERROR: return "DEVICE_ADDRESS_ERROR";
        case FINGERPRINT_WRONG_PASSWORD: return "WRONG_PASSWORD";
        case FINGERPRINT_TEMPLATE_NOT_EMPTY: return "TEMPLATE_NOT_EMPTY";
        case FINGERPRINT_TEMPLATE_EMPTY: return "TEMPLATE_EMPTY";
        case FINGERPRINT_LIBRARY_EMPTY: return "LIBRARY_EMPTY";
        case FINGERPRINT_ENROLL_COUNT_ERROR: return "ENROLL_COUNT_ERROR";
        case FINGERPRINT_TIMEOUT: return "TIMEOUT";
        case FINGERPRINT_ALREADY_EXISTS: return "ALREADY_EXISTS";
        case FINGERPRINT_FEATURE_ASSOCIATED: return "FEATURE_ASSOCIATED";
        case FINGERPRINT_SENSOR_INIT_FAIL: return "SENSOR_INIT_FAIL";
        case FINGERPRINT_MODULE_INFO_NOT_EMPTY: return "MODULE_INFO_NOT_EMPTY";
        case FINGERPRINT_MODULE_INFO_EMPTY: return "MODULE_INFO_EMPTY";
        case FINGERPRINT_OTP_FAIL: return "OTP_FAIL";
        case FINGERPRINT_KEY_GEN_FAIL: return "KEY_GEN_FAIL";
        case FINGERPRINT_KEY_NOT_EXIST: return "KEY_NOT_EXIST";
        case FINGERPRINT_ALGORITHM_FAIL: return "ALGORITHM_FAIL";
        case FINGERPRINT_ALGORITHM_RESULT_ERROR: return "ALGORITHM_RESULT_ERROR";
        case FINGERPRINT_FUNCTION_MISMATCH: return "FUNCTION_MISMATCH";
        case FINGERPRINT_KEY_LOCKED: return "KEY_LOCKED";
        case FINGERPRINT_IMAGE_TOO_SMALL: return "IMAGE_TOO_SMALL";
        case FINGERPRINT_IMAGE_STATIC_OBJECT: return "IMAGE_STATIC_OBJECT";
        case FINGERPRINT_ILLEGAL_DATA: return "ILLEGAL_DATA";
        case FINGERPRINT_OPERATION_BLOCKED: return "OPERATION_BLOCKED";
        case FINGERPRINT_PARAM_ERROR: return "PARAM_ERROR";
        case FINGERPRINT_MODULE_NOT_OPEN: return "MODULE_NOT_OPEN";
        case FINGERPRINT_PASSIVE_ACTIVATION: return "PASSIVE_ACTIVATION";
        default: return "UNKNOWN_STATUS";
    }
}

String FingerprintDebugUtils::getAutoEnrollParam1Name(uint8_t param1) {
    switch (param1) {
        case PS_AUTO_ENROLL_PARAM1_LEGAL_CHECK:    return "LEGAL_CHECK";
        case PS_AUTO_ENROLL_PARAM1_GET_IMAGE:      return "GET_IMAGE";
        case PS_AUTO_ENROLL_PARAM1_GEN_CHAR:       return "GEN_CHAR";
        case PS_AUTO_ENROLL_PARAM1_LIFT_FINGER:    return "LIFT_FINGER";
        case PS_AUTO_ENROLL_PARAM1_MERGE_TEMPLATE: return "MERGE_TEMPLATE";
        case PS_AUTO_ENROLL_PARAM1_VERIFY:         return "VERIFY";
        case PS_AUTO_ENROLL_PARAM1_STORE:          return "STORE";
        default:                                   return "UNKNOWN_STAGE";
    }
}

String FingerprintDebugUtils::getAutoEnrollParam2Name(uint8_t param2) {
    switch (param2) {
        case PS_AUTO_ENROLL_PARAM2_OK:                return "OK";
        case PS_AUTO_ENROLL_PARAM2_MERGE_TEMPLATE:    return "MERGE_TEMPLATE";
        case PS_AUTO_ENROLL_PARAM2_FINGER_DUPLICATED: return "FINGER_DUPLICATED";
        case PS_AUTO_ENROLL_PARAM2_STORE_TEMPLATE:    return "STORE_TEMPLATE";
        // default:                                      return "UNKNOWN_STATUS";
    }
    // 如果都不匹配，则返回 “N%d” ,其中 %d 是 param2 的值
    return String("N") + String(param2);
}

String FingerprintDebugUtils::getAutoIdentifyParamName(uint8_t param) {
    switch (param) {
        case PS_AUTO_IDENTIFY_PARAM_LEGAL_CHECK: return "LEGAL_CHECK";
        case PS_AUTO_IDENTIFY_PARAM_GET_IMAGE:   return "GET_IMAGE";
        case PS_AUTO_IDENTIFY_PARAM_VERIFY:      return "VERIFY";
        default:                                 return "UNKNOWN_PARAM";
    }
}

#endif // M5_MODULE_DEBUG_SERIAL

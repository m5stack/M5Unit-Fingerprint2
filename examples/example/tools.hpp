#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include <Arduino.h>
#include <M5Unified.hpp>
#include <m5_unit_fingerprint2.hpp>


extern TaskHandle_t fingerDetectionTaskHandle;

/**
 * @brief 自动获取、显示指纹图像到画布上并管理内存
 * 
 * @param fingerprint2 指纹传感器对象引用
 * @param maxBufferSize 允许分配的最大内存大小（字节）
 * @param canvas 画布对象引用
 * @param displayX 显示位置X坐标
 * @param displayY 显示位置Y坐标
 * @param rotation 旋转角度
 * @return bool true表示成功，false表示失败
 */
bool displayFingerprintImage(M5UnitFingerprint2& fingerprint2, uint32_t maxBufferSize, M5Canvas& canvas, int displayX, int displayY, uint8_t rotation);

/**
 * @brief 自动读取并显示系统参数到画布上
 * 
 * @param fingerprint2 指纹传感器对象引用
 * @param canvas 画布对象引用
 * @param displayX 显示位置X坐标
 * @param displayY 显示位置Y坐标
 * @param clearArea 是否清除显示区域 (true/false)
 * @return bool true表示成功，false表示失败
 */
bool displaySystemParameters(M5UnitFingerprint2& fingerprint2, M5Canvas& canvas, int displayX, int displayY, bool clearArea = true);

/**
 * @brief 显示菜单界面，包含Auto Enroll和Auto Identify两个选项
 * 
 * @param canvas 画布对象引用
 * @param displayX 显示位置X坐标
 * @param displayY 显示位置Y坐标
 * @return bool true表示成功，false表示失败
 */
bool displayMenu(M5Canvas& canvas, int displayX, int displayY);

/**
 * @brief 初始化指纹传感器并执行基本检查
 * 
 * 此函数执行完整的指纹传感器初始化序列，包括：
 * - 检查模块状态
 * - 激活指纹模块
 * - 获取固件版本
 * - 获取芯片序列号
 * - 执行握手指令
 * - 校验传感器状态
 * 
 * @param fingerprint2 指纹传感器对象引用
 * @param canvas 画布对象引用，用于显示状态信息
 * @param maxRetries 最大重试次数，设为0表示无限重试，默认为10次
 * @param retryDelay 重试间隔时间(毫秒)，默认为1000ms
 * @return bool true表示初始化成功，false表示失败
 */
bool initializeFingerprintSensor(M5UnitFingerprint2& fingerprint2, M5Canvas& canvas, int maxRetries = 10, int retryDelay = 1000);

/**
 * @brief 指纹检测线程任务结构体
 */
struct FingerDetectionParams {
    M5UnitFingerprint2* fingerprint2;
    M5Canvas* canvas;
};

/**
 * @brief 指纹检测线程函数
 * @param parameter 指向FingerDetectionParams结构体的指针
 */
void fingerDetectionTask(void *parameter);

/**
 * @brief 处理触摸输入
 * @param canvas 画布对象引用
 * @param fingerprint2 指纹传感器对象引用
 */
void handleTouchInput(M5Canvas& canvas, M5UnitFingerprint2& fingerprint2);

/**
 * @brief PS_AutoEnroll 回调函数
 * 在指纹自动注册过程中被调用，用于显示进度和状态
 * @param ID 注册的指纹ID
 * @param confirmationCode 确认码/状态码
 * @param param1 阶段参数1
 * @param param2 阶段参数2  
 * @param packetCount 当前包序号
 * @return bool true继续处理，false中止处理
 */
bool PS_AutoEnroll_callback(uint16_t ID,
                            fingerprint_status_t confirmationCode,
                            uint8_t param1,
                            uint8_t param2,
                            int packetCount);

/**
 * @brief PS_AutoIdentify 回调函数
 * 在指纹自动识别过程中被调用，用于显示进度和状态
 * @param securityLevel 安全等级
 * @param confirmationCode 确认码/状态码
 * @param param 阶段参数
 * @param matchedID 匹配到的指纹ID
 * @param matchScore 匹配分数
 * @param packetCount 当前包序号
 * @return bool true继续处理，false中止处理
 */
bool PS_AutoIdentify_callback(uint8_t securityLevel,
                              fingerprint_status_t confirmationCode,
                              uint8_t param,
                              uint16_t matchedID,
                              uint16_t matchScore,
                              int packetCount);

/**
 * @brief 模板上传进度回调函数
 * 在模板上传过程中被调用，用于显示上传进度
 * @param currentSize 当前包大小
 * @param totalSize 总大小
 * @param packetCount 当前包序号
 * @param isComplete 是否完成
 * @return bool true继续上传，false中止上传
 */
bool templateUploadProgressCallback(uint16_t currentSize, uint32_t totalSize, int packetCount, bool isComplete);


/**
 * @brief 模板下载进度回调函数
 * 在模板下载过程中被调用，用于显示下载进度
 * @param currentSize 当前包大小
 * @param totalSize 总大小
 * @param packetCount 当前包序号
 * @param isComplete 是否完成
 */
bool templateDownloadProgressCallback(uint16_t currentSize, uint32_t totalSize, int packetCount, bool isComplete);

#endif // __TOOLS_HPP__
#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include <Arduino.h>
#include <M5Unified.hpp>
#include <M5UnitFingerprint2.hpp>


extern TaskHandle_t fingerDetectionTaskHandle;

/**
 * @brief 自动获取、显示指纹图像到画布上并管理内存 / Automatically get, display fingerprint image on canvas and manage memory
 * 
 * @param fp2 指纹传感器对象引用 / Fingerprint sensor object reference
 * @param maxBufferSize 允许分配的最大内存大小（字节） / Maximum allowed memory size (bytes)
 * @param canvas 画布对象引用 / Canvas object reference
 * @param displayX 显示位置X坐标 / Display position X coordinate
 * @param displayY 显示位置Y坐标 / Display position Y coordinate
 * @param rotation 旋转角度 / Rotation angle
 * @return bool true表示成功，false表示失败 / true for success, false for failure
 */
bool displayFingerprintImage(M5UnitFingerprint2& fp2, uint32_t maxBufferSize, M5Canvas& canvas, int displayX, int displayY, uint8_t rotation);

/**
 * @brief 自动读取并显示系统参数到画布上 / Automatically read and display system parameters on canvas
 * 
 * @param fp2 指纹传感器对象引用 / Fingerprint sensor object reference
 * @param canvas 画布对象引用 / Canvas object reference
 * @param displayX 显示位置X坐标 / Display position X coordinate
 * @param displayY 显示位置Y坐标 / Display position Y coordinate
 * @param clearArea 是否清除显示区域 (true/false) / Whether to clear display area (true/false)
 * @return bool true表示成功，false表示失败 / true for success, false for failure
 */
bool displaySystemParameters(M5UnitFingerprint2& fp2, M5Canvas& canvas, int displayX, int displayY, bool clearArea = true);

/**
 * @brief 显示菜单界面，包含Auto Enroll和Auto Identify两个选项 / Display menu interface with Auto Enroll and Auto Identify options
 * 
 * @param canvas 画布对象引用 / Canvas object reference
 * @param displayX 显示位置X坐标 / Display position X coordinate
 * @param displayY 显示位置Y坐标 / Display position Y coordinate
 * @return bool true表示成功，false表示失败 / true for success, false for failure
 */
bool displayMenu(M5Canvas& canvas, int displayX, int displayY);

/**
 * @brief 初始化指纹传感器并执行基本检查 / Initialize fingerprint sensor and perform basic checks
 * 
 * 此函数执行完整的指纹传感器初始化序列，包括： / This function performs complete fingerprint sensor initialization sequence, including:
 * - 检查模块状态 / Check module status
 * - 激活指纹模块 / Activate fingerprint module
 * - 获取固件版本 / Get firmware version
 * - 获取芯片序列号 / Get chip serial number
 * - 执行握手指令 / Execute handshake command
 * - 校验传感器状态 / Verify sensor status
 * 
 * @param fp2 指纹传感器对象引用 / Fingerprint sensor object reference
 * @param canvas 画布对象引用，用于显示状态信息 / Canvas object reference for displaying status information
 * @param maxRetries 最大重试次数，设为0表示无限重试，默认为10次 / Maximum retry count, 0 for infinite retry, default is 10
 * @param retryDelay 重试间隔时间(毫秒)，默认为1000ms / Retry interval (milliseconds), default is 1000ms
 * @return bool true表示初始化成功，false表示失败 / true for initialization success, false for failure
 */
bool initializeFingerprintSensor(M5UnitFingerprint2& fp2, M5Canvas& canvas, int maxRetries = 10, int retryDelay = 1000);

/**
 * @brief 指纹检测线程任务结构体 / Finger detection thread task structure
 */
struct FingerDetectionParams {
    M5UnitFingerprint2* fp2;
    M5Canvas* canvas;
};

/**
 * @brief 指纹检测线程函数 / Finger detection thread function
 * @param parameter 指向FingerDetectionParams结构体的指针 / Pointer to FingerDetectionParams structure
 */
void fingerDetectionTask(void *parameter);

/**
 * @brief 处理触摸输入 / Handle touch input
 * @param canvas 画布对象引用 / Canvas object reference
 * @param fp2 指纹传感器对象引用 / Fingerprint sensor object reference
 */
void handleTouchInput(M5Canvas& canvas, M5UnitFingerprint2& fp2);

/**
 * @brief PS_AutoEnroll 回调函数 / PS_AutoEnroll callback function
 * 在指纹自动注册过程中被调用，用于显示进度和状态 / Called during fingerprint auto enrollment to display progress and status
 * @param ID 注册的指纹ID / Enrolled fingerprint ID
 * @param confirmationCode 确认码/状态码 / Confirmation code/status code
 * @param param1 阶段参数1 / Stage parameter 1
 * @param param2 阶段参数2 / Stage parameter 2
 * @param packetCount 当前包序号 / Current packet number
 * @return bool true继续处理，false中止处理 / true to continue processing, false to abort processing
 */
bool PS_AutoEnroll_callback(uint16_t ID,
                            fingerprint_status_t confirmationCode,
                            uint8_t param1,
                            uint8_t param2,
                            int packetCount);

/**
 * @brief PS_AutoIdentify 回调函数 / PS_AutoIdentify callback function
 * 在指纹自动识别过程中被调用，用于显示进度和状态 / Called during fingerprint auto identification to display progress and status
 * @param securityLevel 安全等级 / Security level
 * @param confirmationCode 确认码/状态码 / Confirmation code/status code
 * @param param 阶段参数 / Stage parameter
 * @param matchedID 匹配到的指纹ID / Matched fingerprint ID
 * @param matchScore 匹配分数 / Match score
 * @param packetCount 当前包序号 / Current packet number
 * @return bool true继续处理，false中止处理 / true to continue processing, false to abort processing
 */
bool PS_AutoIdentify_callback(uint8_t securityLevel,
                              fingerprint_status_t confirmationCode,
                              uint8_t param,
                              uint16_t matchedID,
                              uint16_t matchScore,
                              int packetCount);

/**
 * @brief 模板上传进度回调函数 / Template upload progress callback function
 * 在模板上传过程中被调用，用于显示上传进度 / Called during template upload to display upload progress
 * @param currentSize 当前包大小 / Current packet size
 * @param totalSize 总大小 / Total size
 * @param packetCount 当前包序号 / Current packet number
 * @param isComplete 是否完成 / Whether complete
 * @return bool true继续上传，false中止上传 / true to continue upload, false to abort upload
 */
bool templateUploadProgressCallback(uint16_t currentSize, uint32_t totalSize, int packetCount, bool isComplete);


/**
 * @brief 模板下载进度回调函数 / Template download progress callback function
 * 在模板下载过程中被调用，用于显示下载进度 / Called during template download to display download progress
 * @param currentSize 当前包大小 / Current packet size
 * @param totalSize 总大小 / Total size
 * @param packetCount 当前包序号 / Current packet number
 * @param isComplete 是否完成 / Whether complete
 */
bool templateDownloadProgressCallback(uint16_t currentSize, uint32_t totalSize, int packetCount, bool isComplete);

#endif // __TOOLS_HPP__
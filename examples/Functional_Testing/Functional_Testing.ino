#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <M5UnitFingerprint2.hpp>
#include <M5Unified.hpp>

#include <tools.hpp>

const char *VERSION = "Unit-Fingerprint2_k232cp_Test_VF2.0";
const char *PLATFORM = "in M5Stack CoreS3/-SE";

M5Canvas canvas(&M5.Lcd);

M5UnitFingerprint2 fp2(&Serial1, 2, 1); // 创建指纹传感器对象，使用默认地址 / Create fingerprint sensor object with default address

bool fingerprintInitialized = false; // 指纹传感器初始化状态标志 / Fingerprint sensor initialization status flag

void setup() {
  Serial.begin(921600);
  Serial0.begin(921600, SERIAL_8N1, 9, 8); // 初始化 Serial0 用于调试输出 / Initialize Serial0 for debug output
  Serial.printf("VERSION: %s\r\n", VERSION);
  Serial.printf("PLATFORM: %s\r\n", PLATFORM);

  M5.begin();

  // 创建画布 / Create canvas
  canvas.createSprite(320, 240);
  // 清屏 / Clear screen
  canvas.fillScreen(0x0000);
  // 设置字体 / Set font
  // canvas.setTextFont(2);
  // 设置文本颜色 / Set text color
  canvas.setTextColor(0xFFFF, 0x0000);
  // 设置文本大小 / Set text size
  canvas.setTextSize(2);
  // 在画布上打印文本 / Print text on canvas
  canvas.drawString("Unit-Fp2_k232cp_Test_V2", 2, 0);
  // 画一条分割线 / Draw a dividing line
  canvas.drawLine(0, 40, 320, 40, 0xFFFF);
  // 将画布内容显示到屏幕上 / Display canvas content to screen
  canvas.pushSprite(0, 0); 
  // 设置亮度 / Set brightness
  M5.Display.setBrightness(255);

  fp2.begin(); // 初始化指纹传感器 / Initialize fingerprint sensor

  // 使用封装的函数初始化指纹传感器 / Initialize fingerprint sensor using wrapped function
  if (initializeFingerprintSensor(fp2, canvas, 0, 1000)) {
    Serial.println("Fingerprint sensor ready for operations.");
    fingerprintInitialized = true; // 设置初始化成功标志 / Set initialization success flag

    // 设置休眠时间 / Set sleep time
    fp2.PS_SetSleepTime(60); // 设置休眠时间为60秒 / Set sleep time to 60 seconds
    // 获取并打印休眠时间 / Get and print sleep time
    uint8_t SleepTime;
  if (fp2.PS_GeTSleepTime(SleepTime) == FINGERPRINT_OK) {
      Serial.printf("Sleep time set to: %d seconds\r\n", SleepTime);
    } else {
      Serial.println("Failed to get sleep time.");
    }

    // 设置工作模式为开启模式 / Set work mode to enabled mode
    fp2.PS_SetWorkMode(1); // 1:开启模式 / 1: enabled mode
    // 获取并打印工作模式 / Get and print work mode
    uint8_t WorkMode;
  if (fp2.PS_GetWorkMode(WorkMode) == FINGERPRINT_OK) {
      Serial.printf("Work mode set to: %d\r\n", WorkMode);
    } else {
      Serial.println("Failed to get work mode.");
      canvas.drawString("Failed to get work mode.", 2, 40);
    }

    //保存休眠时间和工作模式 / Save sleep time and work mode
    fp2.PS_SaveConfigurationToFlash(0) == FINGERPRINT_OK ? 
    Serial.println("\"Sleep Time\" -> Configuration saved to flash.") :
    Serial.println("\"Sleep Time\" -> Failed to save configuration to flash.");
    fp2.PS_SaveConfigurationToFlash(1) == FINGERPRINT_OK ? 
    Serial.println("\"Work Mode\" -> Configuration saved to flash.") :
    Serial.println("\"Work Mode\" -> Failed to save configuration to flash.");

    // 显示菜单 / Display menu
    displayMenu(canvas, 0, 45);

    // 将画布内容显示到屏幕上 / Display canvas content to screen
    canvas.pushSprite(0, 0);
    
    // 创建指纹检测任务参数 / Create finger detection task parameters
    static FingerDetectionParams taskParams;
    taskParams.fp2 = &fp2;
    taskParams.canvas = &canvas;
    
    // 创建指纹检测线程 / Create finger detection thread
    xTaskCreate(
      fingerDetectionTask,           // 任务函数 / Task function
      "FingerDetection",            // 任务名称 / Task name
      4096,                         // 堆栈大小 / Stack size
      &taskParams,                  // 参数 / Parameters
      1,                            // 优先级 / Priority
      &fingerDetectionTaskHandle    // 任务句柄 / Task handle
    );
    Serial.println("Finger detection task created.");
    
  } else {
    Serial.println("Fingerprint sensor initialization failed. Check connections.");
    fingerprintInitialized = false; // 设置初始化失败标志 / Set initialization failure flag
  }

  delay(1000); // 等待1秒 / Wait for 1 second

  // 以下是一些额外的指纹传感器操作示例 / Here are some additional fingerprint sensor operation examples

  // uint8_t INFData_Buffer[512] = {0}; // 创建一个512字节的缓冲区来存储INF页数据 / Create a 512-byte buffer to store INF page data
  // if (fp2.PS_ReadINFpage(INFData_Buffer) == FINGERPRINT_OK) {
  //   Serial.println("INF page read successfully.");
  // } else {
  //   Serial.println("Failed to read INF page.");
  // }

  // uint16_t validTemplateCount = 0;
  // if (fp2.PS_ValidTemplateNum(validTemplateCount) == FINGERPRINT_OK) {
  //   Serial.printf("Valid template count: %d\r\n", validTemplateCount);
  //   canvas.setCursor(2, 60);
  // } else {
  //   Serial.println("Failed to get valid template count.");
  // }

  // uint8_t indexTableData[32] = {0}; // 创建一个32字节的缓冲区来存储索引表数据 / Create a 32-byte buffer to store index table data
  // fp2.PS_ReadIndexTable(indexTableData) == FINGERPRINT_OK ? 
  //   Serial.println("Index table read successfully.") :
  //   Serial.println("Failed to read index table.");

  // fp2.PS_ControlBLN(FINGERPRINT_LED_BREATHING, FINGERPRINT_LED_COLOR_BLUE, FINGERPRINT_LED_COLOR_BLUE, 0) == FINGERPRINT_OK ? 
  //   Serial.println("LED control command sent successfully.") :
  //   Serial.println("Failed to send LED control command.");

  // uint8_t imageArea, imageQuality;
  // fp2.PS_GetImageInfo(imageArea, imageQuality) == FINGERPRINT_OK ? 
  //   Serial.printf("Image area: %d, Image quality: %d\r\n", imageArea, imageQuality) :
  //   Serial.println("Failed to get image info.");

}

void loop() {
  M5.update(); // 更新M5设备状态，包括触摸检测 / Update M5 device status, including touch detection
  
  // 只有在指纹传感器初始化成功后才允许触摸检测 / Only allow touch detection after fingerprint sensor initialization succeeds
  if (!fingerprintInitialized) {
    delay(100); // 如果未初始化，等待一段时间后再检查 / If not initialized, wait for a while before checking again
    return;
  }
  
  // 使用封装的触摸检测函数 / Use wrapped touch detection function
  handleTouchInput(canvas, fp2);
  
  delay(10); // 小延时，避免CPU占用过高 / Small delay to avoid excessive CPU usage
}
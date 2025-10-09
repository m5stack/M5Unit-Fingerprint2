# M5Unit-Fingerprint2 库

## 概述

M5Unit-Fingerprint2 是一个用于控制和管理 M5Stack Unit-Fingerprint2 指纹识别模块的Arduino库。该库提供了完整的指纹识别API，支持指纹注册、识别、删除、系统配置等功能，适用于各种需要生物识别认证的应用场景。

### 主要特性

- **指纹管理**: 支持指纹注册、验证、删除和模板管理
- **自动识别**: 提供自动注册模板和自动识别模板支持
- **LED控制**: 支持多种LED灯效控制，包括呼吸灯、闪烁灯等
- **系统配置**: 支持系统参数读取和配置（波特率不支持）
- **模板管理**: 支持模板上传、下载
- **线程安全**: 使用FreeRTOS互斥锁确保多线程环境下的安全操作
- **完整的错误处理**: 提供详细的状态码和错误信息
- **图像处理**: 支持指纹图像获取和质量检测
- **便笺功能**: 支持读写便笺功能，可存储用户自定义数据

### 支持平台

- **ESP32系列**
- **M5Stack系列设备**(推荐)
- **Arduino平台**
- **ESP-IDF框架**（暂无计划）

## 快速开始

### 基础使用

```cpp
#include <M5Unified.h>
#include <M5UnitFingerprint2.hpp>

// 创建指纹传感器对象
M5UnitFingerprint2 fingerprint2(0x00000000, 0xFFFFFFFF, &Serial1, 2, 1);

void setup() {
    Serial.begin(115200);
    M5.begin();
  
    // 初始化指纹传感器
    if (fingerprint2.begin()) {
        Serial.println("指纹传感器初始化成功");
    } else {
        Serial.println("指纹传感器初始化失败");
        return;
    }
  
    // 设置休眠时间
    fingerprint2.PS_SetSleepTime(60); // 60秒
  
    // 设置工作模式为常开模式
    fingerprint2.PS_SetWorkMode(1);
  
    // 保存配置到Flash
    fingerprint2.PS_SaveConfigurationToFlash(0); // 保存休眠时间
    fingerprint2.PS_SaveConfigurationToFlash(1); // 保存工作模式
}

void loop() {
    M5.update();
  
    // 检查是否有手指按下
    if (M5.BtnA.wasPressed()) {
        enrollFingerprint(1); // 注册ID为1的指纹
    }
  
    if (M5.BtnB.wasPressed()) {
        verifyFingerprint(); // 验证指纹
    }
  
    if (M5.BtnC.wasPressed()) {
        deleteFingerprint(1); // 删除ID为1的指纹
    }
  
    delay(100);
}

// 注册指纹函数
void enrollFingerprint(uint16_t id) {
    Serial.println("开始指纹注册...");
  
    // 使用自动注册功能
    fingerprint_status_t status = fingerprint2.PS_AutoEnroll(
        id,                                    // 指纹ID
        6,                                     // 注册次数
        FINGERPRINT_AUTO_ENROLL_DEFAULT,       // 默认标志
        nullptr,                               // 参数1
        nullptr,                               // 参数2
        nullptr                                // 回调函数
    );
  
    if (status == FINGERPRINT_OK) {
        Serial.printf("指纹ID %d 注册成功\n", id);
    } else {
        Serial.printf("指纹注册失败，错误码: %d\n", status);
    }
}

// 验证指纹函数
void verifyFingerprint() {
    Serial.println("开始指纹验证...");
  
    uint16_t matchedID = 0;
    fingerprint_status_t status = fingerprint2.PS_AutoIdentify(
        3,                                     // 安全等级 (1-5)
        0,                                     // 搜索起始ID
        FINGERPRINT_AUTO_VERIFY_DEFAULT,       // 默认标志
        matchedID,                             // 匹配的ID
        nullptr                                // 回调函数
    );
  
    if (status == FINGERPRINT_OK) {
        Serial.printf("指纹验证成功，匹配ID: %d\n", matchedID);
    } else {
        Serial.printf("指纹验证失败，错误码: %d\n", status);
    }
}

// 删除指纹函数
void deleteFingerprint(uint16_t id) {
    fingerprint_status_t status = fingerprint2.PS_DeletChar(id, 1);
  
    if (status == FINGERPRINT_OK) {
        Serial.printf("指纹ID %d 删除成功\n", id);
    } else {
        Serial.printf("指纹删除失败，错误码: %d\n", status);
    }
}
```

### 调试输出激活

在调试模式下，可以启用详细日志输出。需要在代码中定义以下宏：

```cpp
#define M5_MODULE_DEBUG_SERIAL Serial0
```

### M5Stack CoreS3 示例

```cpp
#include <M5Unified.h>
#include <M5UnitFingerprint2.hpp>

M5UnitFingerprint2 fingerprint2(0x00000000, 0xFFFFFFFF, &Serial1, 2, 1);
M5Canvas canvas(&M5.Lcd);

void setup() {
    Serial.begin(115200);
    M5.begin();
  
    // 创建画布用于显示
    canvas.createSprite(320, 240);
    canvas.fillScreen(0x0000);
    canvas.setTextColor(0xFFFF, 0x0000);
    canvas.setTextSize(2);
  
    // 显示标题
    canvas.drawString("Unit-Fingerprint2 Demo", 2, 0);
    canvas.drawLine(0, 40, 320, 40, 0xFFFF);
    canvas.drawString("A:Enroll B:Verify C:Delete", 2, 50);
    canvas.pushSprite(0, 0);
  
    // 初始化指纹传感器
    if (fingerprint2.begin()) {
        canvas.drawString("Sensor Ready!", 2, 80);
        canvas.pushSprite(0, 0);
  
        // 配置传感器
        fingerprint2.PS_SetSleepTime(60);
        fingerprint2.PS_SetWorkMode(1);
        fingerprint2.PS_SaveConfigurationToFlash(0);
        fingerprint2.PS_SaveConfigurationToFlash(1);
  
        // 设置LED为蓝色呼吸灯
        fingerprint2.PS_ControlBLN(
            FINGERPRINT_LED_BREATHING, 
            FINGERPRINT_LED_COLOR_BLUE, 
            FINGERPRINT_LED_COLOR_BLUE, 
            0
        );
    } else {
        canvas.drawString("Sensor Failed!", 2, 80);
        canvas.pushSprite(0, 0);
    }
}

void loop() {
    M5.update();
  
    if (M5.BtnA.wasPressed()) {
        canvas.fillRect(2, 100, 300, 40, 0x0000);
        canvas.drawString("Enrolling ID 1...", 2, 100);
        canvas.pushSprite(0, 0);
  
        if (enrollFingerprintWithFeedback(1)) {
            canvas.fillRect(2, 100, 300, 40, 0x0000);
            canvas.drawString("Enroll Success!", 2, 100);
        } else {
            canvas.fillRect(2, 100, 300, 40, 0x0000);
            canvas.drawString("Enroll Failed!", 2, 100);
        }
        canvas.pushSprite(0, 0);
    }
  
    if (M5.BtnB.wasPressed()) {
        canvas.fillRect(2, 100, 300, 40, 0x0000);
        canvas.drawString("Verifying...", 2, 100);
        canvas.pushSprite(0, 0);
  
        uint16_t matchedID;
        if (verifyFingerprintWithFeedback(matchedID)) {
            canvas.fillRect(2, 100, 300, 40, 0x0000);
            canvas.drawString("Match ID: " + String(matchedID), 2, 100);
        } else {
            canvas.fillRect(2, 100, 300, 40, 0x0000);
            canvas.drawString("No Match!", 2, 100);
        }
        canvas.pushSprite(0, 0);
    }
  
    if (M5.BtnC.wasPressed()) {
        canvas.fillRect(2, 100, 300, 40, 0x0000);
        canvas.drawString("Deleting ID 1...", 2, 100);
        canvas.pushSprite(0, 0);
  
        if (fingerprint2.PS_DeletChar(1, 1) == FINGERPRINT_OK) {
            canvas.fillRect(2, 100, 300, 40, 0x0000);
            canvas.drawString("Delete Success!", 2, 100);
        } else {
            canvas.fillRect(2, 100, 300, 40, 0x0000);
            canvas.drawString("Delete Failed!", 2, 100);
        }
        canvas.pushSprite(0, 0);
    }
  
    delay(100);
}

bool enrollFingerprintWithFeedback(uint16_t id) {
    // 设置LED为黄色，表示等待录入
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_ON, 
        FINGERPRINT_LED_COLOR_YELLOW, 
        FINGERPRINT_LED_COLOR_YELLOW, 
        0
    );
  
    fingerprint_status_t status = fingerprint2.PS_AutoEnroll(
        id, 6, FINGERPRINT_AUTO_ENROLL_DEFAULT
    );
  
    if (status == FINGERPRINT_OK) {
        // 设置LED为绿色，表示成功
        fingerprint2.PS_ControlBLN(
            FINGERPRINT_LED_FLASHING, 
            FINGERPRINT_LED_COLOR_GREEN, 
            FINGERPRINT_LED_COLOR_GREEN, 
            3
        );
        return true;
    } else {
        // 设置LED为红色，表示失败
        fingerprint2.PS_ControlBLN(
            FINGERPRINT_LED_FLASHING, 
            FINGERPRINT_LED_COLOR_RED, 
            FINGERPRINT_LED_COLOR_RED, 
            3
        );
        return false;
    }
}

bool verifyFingerprintWithFeedback(uint16_t& matchedID) {
    // 设置LED为蓝色，表示等待验证
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_ON, 
        FINGERPRINT_LED_COLOR_BLUE, 
        FINGERPRINT_LED_COLOR_BLUE, 
        0
    );
  
    fingerprint_status_t status = fingerprint2.PS_AutoIdentify(
        3, 0, FINGERPRINT_AUTO_VERIFY_DEFAULT, matchedID
    );
  
    if (status == FINGERPRINT_OK) {
        // 设置LED为绿色，表示验证成功
        fingerprint2.PS_ControlBLN(
            FINGERPRINT_LED_FLASHING, 
            FINGERPRINT_LED_COLOR_GREEN, 
            FINGERPRINT_LED_COLOR_GREEN, 
            3
        );
        return true;
    } else {
        // 设置LED为红色，表示验证失败
        fingerprint2.PS_ControlBLN(
            FINGERPRINT_LED_FLASHING, 
            FINGERPRINT_LED_COLOR_RED, 
            FINGERPRINT_LED_COLOR_RED, 
            3
        );
        return false;
    }
}
```

## API 参考

### 初始化和配置

#### `M5UnitFingerprint2(HardwareSerial* serialPort, int txPin, int rxPin, uint32_t address)`

构造函数，创建M5UnitFingerprint2实例

- **参数**:
  - `serialPort` - 串口对象指针
  - `txPin` - TX引脚编号
  - `rxPin` - RX引脚编号
  - `address` - 模块地址，默认为 0xFFFFFFFF

#### `bool begin()`

初始化模块，设置串口通信

- **返回值**: `true` 成功，`false` 失败

### MCU控制指令

#### `fingerprint_status_t PS_SetSleepTime(uint8_t SleepTime)`

设置休眠时间

- **参数**:
  - `SleepTime` - 休眠时间（10-254秒）
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_GeTSleepTime(uint8_t &SleepTime)`

获取休眠时间

- **参数**:
  - `SleepTime` - 返回的休眠时间
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_SetWorkMode(uint8_t WorkMode)`

设置工作模式

- **参数**:
  - `WorkMode` - 工作模式（0:定时休眠模式，1:开启模式）
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_GetWorkMode(uint8_t &WorkMode)`

获取工作模式

- **参数**:
  - `WorkMode` - 返回的工作模式
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_SaveConfigurationToFlash(uint8_t SaveOptions)`

保存配置到Flash

- **参数**:
  - `SaveOptions` - 保存选项（0:保存休眠时间，1:保存工作模式）
- **返回值**: 操作状态码

### 指纹操作指令

#### `fingerprint_status_t PS_GetImage()`

获取指纹图像（用于验证）

- **返回值**: 操作状态码

#### `fingerprint_status_t PS_GetEnrollImage()`

获取指纹图像（用于注册）

- **返回值**: 操作状态码

#### `fingerprint_status_t PS_GenChar(uint8_t BufferID)`

生成特征文件

- **参数**:
  - `BufferID` - 缓冲区ID（标识第几次按手指）
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_Match(uint16_t &compareScores)`

精确比对两枚指纹特征

- **参数**:
  - `compareScores` - 返回的比对得分
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, uint16_t &PageID, uint16_t &MatchScore)`

搜索指纹

- **参数**:
  - `BufferID` - 缓冲区ID，默认为1
  - `StartPage` - 起始页
  - `PageNum` - 搜索页数
  - `PageID` - 返回的页码ID
  - `MatchScore` - 返回的匹配得分
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_StoreChar(uint8_t BufferID, uint16_t PageID)`

储存模板

- **参数**:
  - `BufferID` - 缓冲区ID，默认为1
  - `PageID` - 指纹库的位置号
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_DeletChar(uint16_t PageID, uint16_t Num)`

删除模板

- **参数**:
  - `PageID` - 指纹库的模板号
  - `Num` - 删除数量
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_Empty()`

清空指纹库

- **返回值**: 操作状态码

### 自动化指令

#### `fingerprint_status_t PS_AutoEnroll(uint16_t ID, uint8_t enrollCount, fingerprint_auto_enroll_flags_t flags, uint8_t* param1, uint8_t* param2, PS_AutoEnrollCallback_t callback)`

自动注册模板

- **参数**:
  - `ID` - 指纹ID号
  - `enrollCount` - 录入次数
  - `flags` - 输入参数标志
  - `param1` - 返回参数1指针（可选）
  - `param2` - 返回参数2指针（可选）
  - `callback` - 回调函数（可选）
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_AutoIdentify(uint8_t securityLevel, uint16_t ID, fingerprint_auto_verify_flags_t flags, uint16_t& PageID, PS_AutoIdentifyCallback_t callback)`

自动验证指纹

- **参数**:
  - `securityLevel` - 安全等级（1-5）
  - `ID` - 搜索起始ID号
  - `flags` - 输入参数标志
  - `PageID` - 返回匹配的页码
  - `callback` - 回调函数（可选）
- **返回值**: 操作状态码

### LED控制

#### `fingerprint_status_t PS_ControlBLN(fingerprint_led_control_mode_t mode, fingerprint_led_color_t startColor, fingerprint_led_color_t endColor, uint8_t loopCount)`

控制LED灯效

- **参数**:
  - `mode` - LED控制模式
  - `startColor` - 起始颜色
  - `endColor` - 结束颜色
  - `loopCount` - 循环次数（适用于呼吸、闪烁模式）
- **返回值**: 操作状态码

### 系统管理

#### `fingerprint_status_t PS_ReadSysPara(PS_ReadSysPara_BasicParams &RawData)`

读取系统参数

- **参数**:
  - `RawData` - 返回的16字节系统参数
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_ValidTemplateNum(uint16_t &ValidNum)`

获取有效模板数量

- **参数**:
  - `ValidNum` - 返回的有效模板数量
- **返回值**: 操作状态码

#### `fingerprint_status_t PS_WriteReg(fingerprint_register_id_t RegID, uint8_t Value)`

写寄存器

- **参数**:
  - `RegID` - 寄存器ID枚举
  - `Value` - 寄存器值
- **返回值**: 操作状态码

## 数据结构

### fingerprint_led_control_mode_t

LED控制模式枚举

```cpp
typedef enum {
    FINGERPRINT_LED_BREATHING = 1,  // 普通呼吸灯
    FINGERPRINT_LED_FLASHING  = 2,  // 闪烁灯
    FINGERPRINT_LED_ON        = 3,  // 常开灯
    FINGERPRINT_LED_OFF       = 4,  // 常闭灯
    FINGERPRINT_LED_FADE_IN   = 5,  // 渐开灯
    FINGERPRINT_LED_FADE_OUT  = 6   // 渐闭灯
} fingerprint_led_control_mode_t;
```

### fingerprint_led_color_t

LED颜色枚举

```cpp
typedef enum {
    FINGERPRINT_LED_COLOR_OFF    = 0x00,  // 全灭
    FINGERPRINT_LED_COLOR_BLUE   = 0x01,  // 蓝灯
    FINGERPRINT_LED_COLOR_GREEN  = 0x02,  // 绿灯
    FINGERPRINT_LED_COLOR_CYAN   = 0x03,  // 青色灯 (蓝+绿)
    FINGERPRINT_LED_COLOR_RED    = 0x04,  // 红灯
    FINGERPRINT_LED_COLOR_PURPLE = 0x05,  // 紫色灯 (蓝+红)
    FINGERPRINT_LED_COLOR_YELLOW = 0x06,  // 黄色灯 (绿+红)
    FINGERPRINT_LED_COLOR_WHITE  = 0x07   // 白色灯 (蓝+绿+红)
} fingerprint_led_color_t;
```

### fingerprint_auto_enroll_flags_t

自动注册标志枚举

```cpp
typedef enum {
    FINGERPRINT_AUTO_ENROLL_DEFAULT              = 0x0000, // 默认设置
    FINGERPRINT_AUTO_ENROLL_NO_STATUS_RETURN     = (1 << 2), // 注册过程中不返回当前状态
    FINGERPRINT_AUTO_ENROLL_ALLOW_OVERWRITE_ID   = (1 << 3), // 允许覆盖已存在的ID号
    FINGERPRINT_AUTO_ENROLL_FORBID_DUPLICATES    = (1 << 4), // 禁止指纹重复注册
    FINGERPRINT_AUTO_ENROLL_NO_LIFT_REQUIRED     = (1 << 5), // 采集间不要求手指离开
} fingerprint_auto_enroll_flags_t;
```

### PS_ReadSysPara_BasicParams

系统参数结构体

```cpp
struct PS_ReadSysPara_BasicParams {
    uint16_t status_register;     // 状态寄存器
    uint16_t temp_size;           // 指纹模板大小
    uint16_t data_size;           // 指纹库大小
    uint16_t score_level;         // 安全等级
    uint32_t device_addr;         // 设备地址
    uint16_t packet_size;         // 数据包大小
    uint16_t baud_rate;           // 串口波特率
} __attribute__((packed));
```

## 高级使用

### 指纹库管理

```cpp
M5UnitFingerprint2 fingerprint2(0x00000000, 0xFFFFFFFF, &Serial1, 2, 1);

void setup() {
    fingerprint2.begin();
  
    // 获取系统信息
    PS_ReadSysPara_BasicParams sysParams;
    if (fingerprint2.PS_ReadSysPara(sysParams) == FINGERPRINT_OK) {
        Serial.printf("指纹库容量: %d\n", sysParams.data_size);
        Serial.printf("安全等级: %d\n", sysParams.score_level);
        Serial.printf("设备地址: 0x%08X\n", sysParams.device_addr);
    }
  
    // 获取已注册指纹数量
    uint16_t validCount = 0;
    if (fingerprint2.PS_ValidTemplateNum(validCount) == FINGERPRINT_OK) {
        Serial.printf("已注册指纹数量: %d\n", validCount);
    }
}

void manageFingerprints() {
    // 读取索引表，了解哪些位置已被占用（只有前100位有效）
    uint8_t indexTable[32] = {0};
  
    if (fingerprint2.PS_ReadIndexTable(indexTable) == FINGERPRINT_OK) {
        Serial.println("指纹库占用情况 (只显示前100个位置):");

        // 索引表固定返回 32 字节，但只有前 100 位有效（模板 ID 0-99）
        for (int templateID = 0; templateID < 100; ++templateID) {
            int byteIndex = templateID / 8;
            int bitIndex  = templateID % 8;   // 低位在前

            bool isOccupied = (indexTable[byteIndex] & (1 << bitIndex)) != 0;
            if (isOccupied) {
                Serial.printf("位置 %d: 已占用\n", templateID);
            }
        }
    }
}

// 批量删除指纹
void clearAllFingerprints() {
    if (fingerprint2.PS_Empty() == FINGERPRINT_OK) {
        Serial.println("指纹库已清空");
    } else {
        Serial.println("清空指纹库失败");
    }
}

// 删除指定范围的指纹
void deleteRangeFingerprints(uint16_t startID, uint16_t count) {
    if (fingerprint2.PS_DeletChar(startID, count) == FINGERPRINT_OK) {
        Serial.printf("已删除指纹 %d 到 %d\n", startID, startID + count - 1);
    } else {
        Serial.printf("删除指纹范围失败\n");
    }
}
```

> **提示**：`PS_ReadIndexTable` 当前固定读取索引表 0，并且会自动清零第 100 位之后的数据（仅保留 0-99 共 100 个模板位置）。如需扩展逻辑，只需在遍历前 100 位时自行统计或过滤即可。

### 模板数据管理

```cpp
void templateOperations() {
    // 上传指纹模板
    uint8_t templateBuffer[1024] = {0}; // 模板缓冲区
    uint32_t actualSize = 0;
  
    // 自动上传模板（从设备）
    if (fingerprint2.PS_UploadTemplateAuto(templateBuffer, sizeof(templateBuffer), actualSize) == FINGERPRINT_OK) {
        Serial.printf("模板上传成功，大小: %d 字节\n", actualSize);
  
        // 可以将模板数据保存到文件或发送到服务器
        saveTemplateToFile(templateBuffer, actualSize);
    }
  
    // 下载模板到设备
    if (loadTemplateFromFile(templateBuffer, actualSize)) {
        if (fingerprint2.PS_DownloadTemplateAuto(templateBuffer, actualSize) == FINGERPRINT_OK) {
            Serial.println("模板下载成功");
        }
    }
}

bool saveTemplateToFile(const uint8_t* data, uint32_t size) {
    // 实现保存到文件的逻辑
    // 这里只是示例
    Serial.printf("保存 %d 字节模板数据到文件\n", size);
    return true;
}

bool loadTemplateFromFile(uint8_t* buffer, uint32_t& size) {
    // 实现从文件加载的逻辑
    // 这里只是示例
    Serial.println("从文件加载模板数据");
    size = 512; // 示例大小
    return true;
}
```

### 便笺功能

```cpp
void notepadOperations() {
    // 写入便笺数据
    const char* userData = "User123-Admin-2025";
    uint8_t notepadData[32] = {0};
    memcpy(notepadData, userData, min(strlen(userData), 32));
  
    if (fingerprint2.PS_WriteNotepad(0, notepadData, strlen(userData)) == FINGERPRINT_OK) {
        Serial.println("便笺写入成功");
    }
  
    // 读取便笺数据
    uint8_t readData[32] = {0};
    if (fingerprint2.PS_ReadNotepad(0, readData) == FINGERPRINT_OK) {
        Serial.printf("便笺内容: %s\n", (char*)readData);
    }
}
```

### 高级LED控制

```cpp
void advancedLEDControl() {
    // 启动时显示白色呼吸灯
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_BREATHING,
        FINGERPRINT_LED_COLOR_WHITE,
        FINGERPRINT_LED_COLOR_WHITE,
        0 // 无限循环
    );
  
    delay(3000);
  
    // 注册模式：黄色常亮
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_ON,
        FINGERPRINT_LED_COLOR_YELLOW,
        FINGERPRINT_LED_COLOR_YELLOW,
        0
    );
  
    delay(2000);
  
    // 验证模式：蓝色常亮
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_ON,
        FINGERPRINT_LED_COLOR_BLUE,
        FINGERPRINT_LED_COLOR_BLUE,
        0
    );
  
    delay(2000);
  
    // 成功：绿色闪烁3次
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_FLASHING,
        FINGERPRINT_LED_COLOR_GREEN,
        FINGERPRINT_LED_COLOR_GREEN,
        3
    );
  
    delay(2000);
  
    // 失败：红色闪烁5次
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_FLASHING,
        FINGERPRINT_LED_COLOR_RED,
        FINGERPRINT_LED_COLOR_RED,
        5
    );
  
    delay(3000);
  
    // 关闭LED
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_OFF,
        FINGERPRINT_LED_COLOR_OFF,
        FINGERPRINT_LED_COLOR_OFF,
        0
    );
}
```

### 图像质量检测

```cpp
void imageQualityCheck() {
    // 获取指纹图像
    if (fingerprint2.PS_GetImage() == FINGERPRINT_OK) {
        Serial.println("图像获取成功");
  
        // 检查图像质量
        uint8_t imageArea, imageQuality;
        if (fingerprint2.PS_GetImageInfo(imageArea, imageQuality) == FINGERPRINT_OK) {
            Serial.printf("图像面积: %d%%\n", imageArea);
            Serial.printf("图像质量: %s\n", imageQuality == 0 ? "合格" : "不合格");
      
            if (imageQuality != 0) {
                Serial.println("图像质量不佳，请重新按压手指");
                // 设置红色LED提示
                fingerprint2.PS_ControlBLN(
                    FINGERPRINT_LED_FLASHING,
                    FINGERPRINT_LED_COLOR_RED,
                    FINGERPRINT_LED_COLOR_RED,
                    2
                );
                return;
            }
      
            if (imageArea < 50) { // 如果图像面积小于50%
                Serial.println("指纹面积不足，请完整按压");
                // 设置黄色LED提示
                fingerprint2.PS_ControlBLN(
                    FINGERPRINT_LED_FLASHING,
                    FINGERPRINT_LED_COLOR_YELLOW,
                    FINGERPRINT_LED_COLOR_YELLOW,
                    2
                );
                return;
            }
      
            // 图像质量良好
            Serial.println("图像质量良好，可以进行处理");
            fingerprint2.PS_ControlBLN(
                FINGERPRINT_LED_FLASHING,
                FINGERPRINT_LED_COLOR_GREEN,
                FINGERPRINT_LED_COLOR_GREEN,
                1
            );
        }
    }
}
```

## 错误处理和状态码

库定义了完整的状态码系统，主要包括：

- `FINGERPRINT_OK` (0x00): 操作成功
- `FINGERPRINT_PACKETRECIEVEERR` (0x01): 数据包接收错误
- `FINGERPRINT_NOFINGER` (0x02): 传感器上没有手指
- `FINGERPRINT_IMAGEFAIL` (0x03): 录入指纹图像失败
- `FINGERPRINT_IMAGEMESS` (0x06): 指纹图像太乱而生成不了特征
- `FINGERPRINT_FEATUREFAIL` (0x07): 特征生成失败
- `FINGERPRINT_NOMATCH` (0x08): 指纹不匹配
- `FINGERPRINT_NOTFOUND` (0x09): 没有搜索到指纹
- `FINGERPRINT_ENROLLMISMATCH` (0x0A): 合并特征失败

```cpp
void handleFingerprintError(fingerprint_status_t status) {
    switch (status) {
        case FINGERPRINT_OK:
            Serial.println("操作成功");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println("请按压手指");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("图像获取失败，请重试");
            break;
        case FINGERPRINT_NOMATCH:
            Serial.println("指纹不匹配");
            break;
        case FINGERPRINT_NOTFOUND:
            Serial.println("指纹未找到");
            break;
        default:
            Serial.printf("未知错误，状态码: 0x%02X\n", status);
            break;
    }
}
```

## 故障排除

### 常见问题

#### 1. 传感器初始化失败

```cpp
if (!fingerprint2.begin()) {
    Serial.println("初始化失败，请检查:");
    Serial.println("1. 串口连接是否正确");
    Serial.println("2. TX/RX引脚是否正确");
    Serial.println("3. 供电是否正常");
    Serial.println("4. 模块地址和密码是否正确");
}
```

#### 2. 指纹注册失败

```cpp
fingerprint_status_t status = fingerprint2.PS_AutoEnroll(id, 6, FINGERPRINT_AUTO_ENROLL_DEFAULT);
if (status != FINGERPRINT_OK) {
    switch (status) {
        case FINGERPRINT_NOFINGER:
            Serial.println("请按压手指到传感器上");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("图像获取失败，请清洁传感器");
            break;
        case FINGERPRINT_ENROLLMISMATCH:
            Serial.println("多次录入不匹配，请重新开始");
            break;
        default:
            Serial.printf("注册失败，错误码: 0x%02X\n", status);
            break;
    }
}
```

#### 3. 指纹识别失败

```cpp
uint16_t matchedID = 0;
fingerprint_status_t status = fingerprint2.PS_AutoIdentify(3, 0, FINGERPRINT_AUTO_VERIFY_DEFAULT, matchedID);
if (status != FINGERPRINT_OK) {
    switch (status) {
        case FINGERPRINT_NOFINGER:
            Serial.println("请按压手指到传感器上");
            break;
        case FINGERPRINT_NOTFOUND:
            Serial.println("指纹未找到，请先注册");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("图像质量差，请重新按压");
            break;
        default:
            Serial.printf("识别失败，错误码: 0x%02X\n", status);
            break;
    }
}
```

```##

- **当前版本**: 1.1.0
- **作者**: M5Stack
- **维护者**: M5Stack
- **许可证**: MIT

## 更多信息

- [M5Stack官网](http://M5Stack.com)
- [项目仓库](https://github.com/m5stack/M5Unit-Fingerprint2)
- [硬件文档](http://M5Stack.com)

---

**注意**: 使用前请确保正确连接硬件并配置相应的引脚。建议在生产环境中实施适当的错误处理和安全措施。
```

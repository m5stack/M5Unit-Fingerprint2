# M5Unit-Fingerprint2 Library

## Overview

M5Unit-Fingerprint2 is an Arduino library for controlling and managing the M5Stack Unit-Fingerprint2 fingerprint recognition module. The library provides a complete fingerprint recognition API, supporting fingerprint enrollment, verification, deletion, system configuration, and other functions, suitable for various application scenarios requiring biometric authentication.

### Key Features

- **Fingerprint Management**: Support for fingerprint enrollment, verification, deletion, and template management
- **Auto Recognition**: Provides automatic enrollment and automatic identification template support
- **LED Control**: Supports multiple LED light effects, including breathing light, flashing light, etc.
- **System Configuration**: Supports system parameter reading and configuration (baud rate not supported)
- **Template Management**: Supports template upload and download
- **Thread Safety**: Uses FreeRTOS mutex to ensure safe operations in multi-threaded environments
- **Complete Error Handling**: Provides detailed status codes and error messages
- **Image Processing**: Supports fingerprint image acquisition and quality detection
- **Notepad Function**: Supports read/write notepad function for storing user-defined data

### Supported Platforms

- **ESP32 Series**
- **M5Stack Series Devices** (Recommended)
- **Arduino Platform**
- **ESP-IDF Framework** (No plans yet)

## Quick Start

### Basic Usage

```cpp
#include <M5Unified.h>
#include <M5UnitFingerprint2.hpp>

// Create fingerprint sensor object
M5UnitFingerprint2 fingerprint2(0x00000000, 0xFFFFFFFF, &Serial1, 2, 1);

void setup() {
    Serial.begin(115200);
    M5.begin();
  
    // Initialize fingerprint sensor
    if (fingerprint2.begin()) {
        Serial.println("Fingerprint sensor initialized successfully");
    } else {
        Serial.println("Fingerprint sensor initialization failed");
        return;
    }
  
    // Set sleep time
    fingerprint2.PS_SetSleepTime(60); // 60 seconds
  
    // Set work mode to always-on mode
    fingerprint2.PS_SetWorkMode(1);
  
    // Save configuration to Flash
    fingerprint2.PS_SaveConfigurationToFlash(0); // Save sleep time
    fingerprint2.PS_SaveConfigurationToFlash(1); // Save work mode
}

void loop() {
    M5.update();
  
    // Check if button is pressed
    if (M5.BtnA.wasPressed()) {
        enrollFingerprint(1); // Enroll fingerprint with ID 1
    }
  
    if (M5.BtnB.wasPressed()) {
        verifyFingerprint(); // Verify fingerprint
    }
  
    if (M5.BtnC.wasPressed()) {
        deleteFingerprint(1); // Delete fingerprint with ID 1
    }
  
    delay(100);
}

// Enroll fingerprint function
void enrollFingerprint(uint16_t id) {
    Serial.println("Starting fingerprint enrollment...");
  
    // Use auto-enroll function
    fingerprint_status_t status = fingerprint2.PS_AutoEnroll(
        id,                                    // Fingerprint ID
        6,                                     // Enrollment count
        FINGERPRINT_AUTO_ENROLL_DEFAULT,       // Default flags
        nullptr,                               // Parameter 1
        nullptr,                               // Parameter 2
        nullptr                                // Callback function
    );
  
    if (status == FINGERPRINT_OK) {
        Serial.printf("Fingerprint ID %d enrolled successfully\n", id);
    } else {
        Serial.printf("Fingerprint enrollment failed, error code: %d\n", status);
    }
}

// Verify fingerprint function
void verifyFingerprint() {
    Serial.println("Starting fingerprint verification...");
  
    uint16_t matchedID = 0;
    fingerprint_status_t status = fingerprint2.PS_AutoIdentify(
        3,                                     // Security level (1-5)
        0,                                     // Search start ID
        FINGERPRINT_AUTO_VERIFY_DEFAULT,       // Default flags
        matchedID,                             // Matched ID
        nullptr                                // Callback function
    );
  
    if (status == FINGERPRINT_OK) {
        Serial.printf("Fingerprint verified successfully, matched ID: %d\n", matchedID);
    } else {
        Serial.printf("Fingerprint verification failed, error code: %d\n", status);
    }
}

// Delete fingerprint function
void deleteFingerprint(uint16_t id) {
    fingerprint_status_t status = fingerprint2.PS_DeletChar(id, 1);
  
    if (status == FINGERPRINT_OK) {
        Serial.printf("Fingerprint ID %d deleted successfully\n", id);
    } else {
        Serial.printf("Fingerprint deletion failed, error code: %d\n", status);
    }
}
```

### Debug Output Activation

In debug mode, you can enable detailed log output. You need to define the following macro in your code:

```cpp
#define M5_MODULE_DEBUG_SERIAL Serial0
```

### M5Stack CoreS3 Example

```cpp
#include <M5Unified.h>
#include <M5UnitFingerprint2.hpp>

M5UnitFingerprint2 fingerprint2(0x00000000, 0xFFFFFFFF, &Serial1, 2, 1);
M5Canvas canvas(&M5.Lcd);

void setup() {
    Serial.begin(115200);
    M5.begin();
  
    // Create canvas for display
    canvas.createSprite(320, 240);
    canvas.fillScreen(0x0000);
    canvas.setTextColor(0xFFFF, 0x0000);
    canvas.setTextSize(2);
  
    // Display title
    canvas.drawString("Unit-Fingerprint2 Demo", 2, 0);
    canvas.drawLine(0, 40, 320, 40, 0xFFFF);
    canvas.drawString("A:Enroll B:Verify C:Delete", 2, 50);
    canvas.pushSprite(0, 0);
  
    // Initialize fingerprint sensor
    if (fingerprint2.begin()) {
        canvas.drawString("Sensor Ready!", 2, 80);
        canvas.pushSprite(0, 0);
  
        // Configure sensor
        fingerprint2.PS_SetSleepTime(60);
        fingerprint2.PS_SetWorkMode(1);
        fingerprint2.PS_SaveConfigurationToFlash(0);
        fingerprint2.PS_SaveConfigurationToFlash(1);
  
        // Set LED to blue breathing light
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
    // Set LED to yellow, indicating waiting for enrollment
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
        // Set LED to green, indicating success
        fingerprint2.PS_ControlBLN(
            FINGERPRINT_LED_FLASHING, 
            FINGERPRINT_LED_COLOR_GREEN, 
            FINGERPRINT_LED_COLOR_GREEN, 
            3
        );
        return true;
    } else {
        // Set LED to red, indicating failure
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
    // Set LED to blue, indicating waiting for verification
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
        // Set LED to green, indicating verification success
        fingerprint2.PS_ControlBLN(
            FINGERPRINT_LED_FLASHING, 
            FINGERPRINT_LED_COLOR_GREEN, 
            FINGERPRINT_LED_COLOR_GREEN, 
            3
        );
        return true;
    } else {
        // Set LED to red, indicating verification failure
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

## API Reference

### Initialization and Configuration

#### `M5UnitFingerprint2(HardwareSerial* serialPort, int txPin, int rxPin, uint32_t address)`

Constructor, creates an M5UnitFingerprint2 instance

- **Parameters**:
  - `serialPort` - Serial port object pointer
  - `txPin` - TX pin number
  - `rxPin` - RX pin number
  - `address` - Module address, default is 0xFFFFFFFF

#### `bool begin()`

Initialize the module and set up serial communication

- **Return**: `true` on success, `false` on failure

### MCU Control Commands

#### `fingerprint_status_t PS_SetSleepTime(uint8_t SleepTime)`

Set sleep time

- **Parameters**:
  - `SleepTime` - Sleep time (10-254 seconds)
- **Return**: Operation status code

#### `fingerprint_status_t PS_GeTSleepTime(uint8_t &SleepTime)`

Get sleep time

- **Parameters**:
  - `SleepTime` - Returned sleep time
- **Return**: Operation status code

#### `fingerprint_status_t PS_SetWorkMode(uint8_t WorkMode)`

Set work mode

- **Parameters**:
  - `WorkMode` - Work mode (0: timed sleep mode, 1: always-on mode)
- **Return**: Operation status code

#### `fingerprint_status_t PS_GetWorkMode(uint8_t &WorkMode)`

Get work mode

- **Parameters**:
  - `WorkMode` - Returned work mode
- **Return**: Operation status code

#### `fingerprint_status_t PS_SaveConfigurationToFlash(uint8_t SaveOptions)`

Save configuration to Flash

- **Parameters**:
  - `SaveOptions` - Save options (0: save sleep time, 1: save work mode)
- **Return**: Operation status code

### Fingerprint Operation Commands

#### `fingerprint_status_t PS_GetImage()`

Capture fingerprint image (for verification)

- **Return**: Operation status code

#### `fingerprint_status_t PS_GetEnrollImage()`

Capture fingerprint image (for enrollment)

- **Return**: Operation status code

#### `fingerprint_status_t PS_GenChar(uint8_t BufferID)`

Generate character file

- **Parameters**:
  - `BufferID` - Buffer ID (identifies which finger press)
- **Return**: Operation status code

#### `fingerprint_status_t PS_Match(uint16_t &compareScores)`

Precisely compare two fingerprint features

- **Parameters**:
  - `compareScores` - Returned comparison score
- **Return**: Operation status code

#### `fingerprint_status_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, uint16_t &PageID, uint16_t &MatchScore)`

Search fingerprint

- **Parameters**:
  - `BufferID` - Buffer ID, default is 1
  - `StartPage` - Start page
  - `PageNum` - Number of pages to search
  - `PageID` - Returned page ID
  - `MatchScore` - Returned match score
- **Return**: Operation status code

#### `fingerprint_status_t PS_StoreChar(uint8_t BufferID, uint16_t PageID)`

Store template

- **Parameters**:
  - `BufferID` - Buffer ID, default is 1
  - `PageID` - Location number in fingerprint library
- **Return**: Operation status code

#### `fingerprint_status_t PS_DeletChar(uint16_t PageID, uint16_t Num)`

Delete template

- **Parameters**:
  - `PageID` - Template number in fingerprint library
  - `Num` - Number to delete
- **Return**: Operation status code

#### `fingerprint_status_t PS_Empty()`

Clear fingerprint library

- **Return**: Operation status code

### Automation Commands

#### `fingerprint_status_t PS_AutoEnroll(uint16_t ID, uint8_t enrollCount, fingerprint_auto_enroll_flags_t flags, uint8_t* param1, uint8_t* param2, PS_AutoEnrollCallback_t callback)`

Auto-enroll template

- **Parameters**:
  - `ID` - Fingerprint ID number
  - `enrollCount` - Enrollment count
  - `flags` - Input parameter flags
  - `param1` - Return parameter 1 pointer (optional)
  - `param2` - Return parameter 2 pointer (optional)
  - `callback` - Callback function (optional)
- **Return**: Operation status code

#### `fingerprint_status_t PS_AutoIdentify(uint8_t securityLevel, uint16_t ID, fingerprint_auto_verify_flags_t flags, uint16_t& PageID, PS_AutoIdentifyCallback_t callback)`

Auto-verify fingerprint

- **Parameters**:
  - `securityLevel` - Security level (1-5)
  - `ID` - Search start ID number
  - `flags` - Input parameter flags
  - `PageID` - Returned matched page number
  - `callback` - Callback function (optional)
- **Return**: Operation status code

### LED Control

#### `fingerprint_status_t PS_ControlBLN(fingerprint_led_control_mode_t mode, fingerprint_led_color_t startColor, fingerprint_led_color_t endColor, uint8_t loopCount)`

Control LED effects

- **Parameters**:
  - `mode` - LED control mode
  - `startColor` - Start color
  - `endColor` - End color
  - `loopCount` - Loop count (applies to breathing and flashing modes)
- **Return**: Operation status code

### System Management

#### `fingerprint_status_t PS_ReadSysPara(PS_ReadSysPara_BasicParams &RawData)`

Read system parameters

- **Parameters**:
  - `RawData` - Returned 16-byte system parameters
- **Return**: Operation status code

#### `fingerprint_status_t PS_ValidTemplateNum(uint16_t &ValidNum)`

Get valid template count

- **Parameters**:
  - `ValidNum` - Returned valid template count
- **Return**: Operation status code

#### `fingerprint_status_t PS_WriteReg(fingerprint_register_id_t RegID, uint8_t Value)`

Write register

- **Parameters**:
  - `RegID` - Register ID enumeration
  - `Value` - Register value
- **Return**: Operation status code

## Data Structures

### fingerprint_led_control_mode_t

LED control mode enumeration

```cpp
typedef enum {
    FINGERPRINT_LED_BREATHING = 1,  // Normal breathing light
    FINGERPRINT_LED_FLASHING  = 2,  // Flashing light
    FINGERPRINT_LED_ON        = 3,  // Always-on light
    FINGERPRINT_LED_OFF       = 4,  // Always-off light
    FINGERPRINT_LED_FADE_IN   = 5,  // Fade-in light
    FINGERPRINT_LED_FADE_OUT  = 6   // Fade-out light
} fingerprint_led_control_mode_t;
```

### fingerprint_led_color_t

LED color enumeration

```cpp
typedef enum {
    FINGERPRINT_LED_COLOR_OFF    = 0x00,  // All off
    FINGERPRINT_LED_COLOR_BLUE   = 0x01,  // Blue light
    FINGERPRINT_LED_COLOR_GREEN  = 0x02,  // Green light
    FINGERPRINT_LED_COLOR_CYAN   = 0x03,  // Cyan light (Blue+Green)
    FINGERPRINT_LED_COLOR_RED    = 0x04,  // Red light
    FINGERPRINT_LED_COLOR_PURPLE = 0x05,  // Purple light (Blue+Red)
    FINGERPRINT_LED_COLOR_YELLOW = 0x06,  // Yellow light (Green+Red)
    FINGERPRINT_LED_COLOR_WHITE  = 0x07   // White light (Blue+Green+Red)
} fingerprint_led_color_t;
```

### fingerprint_auto_enroll_flags_t

Auto-enrollment flags enumeration

```cpp
typedef enum {
    FINGERPRINT_AUTO_ENROLL_DEFAULT              = 0x0000, // Default settings
    FINGERPRINT_AUTO_ENROLL_NO_STATUS_RETURN     = (1 << 2), // Don't return current status during enrollment
    FINGERPRINT_AUTO_ENROLL_ALLOW_OVERWRITE_ID   = (1 << 3), // Allow overwriting existing ID
    FINGERPRINT_AUTO_ENROLL_FORBID_DUPLICATES    = (1 << 4), // Forbid duplicate fingerprint enrollment
    FINGERPRINT_AUTO_ENROLL_NO_LIFT_REQUIRED     = (1 << 5), // No need to lift finger between captures
} fingerprint_auto_enroll_flags_t;
```

### PS_ReadSysPara_BasicParams

System parameters structure

```cpp
struct PS_ReadSysPara_BasicParams {
    uint16_t status_register;     // Status register
    uint16_t temp_size;           // Fingerprint template size
    uint16_t data_size;           // Fingerprint library size
    uint16_t score_level;         // Security level
    uint32_t device_addr;         // Device address
    uint16_t packet_size;         // Packet size
    uint16_t baud_rate;           // Serial baud rate
} __attribute__((packed));
```

## Advanced Usage

### Fingerprint Library Management

```cpp
M5UnitFingerprint2 fingerprint2(0x00000000, 0xFFFFFFFF, &Serial1, 2, 1);

void setup() {
    fingerprint2.begin();
  
    // Get system information
    PS_ReadSysPara_BasicParams sysParams;
    if (fingerprint2.PS_ReadSysPara(sysParams) == FINGERPRINT_OK) {
        Serial.printf("Fingerprint library capacity: %d\n", sysParams.data_size);
        Serial.printf("Security level: %d\n", sysParams.score_level);
        Serial.printf("Device address: 0x%08X\n", sysParams.device_addr);
    }
  
    // Get enrolled fingerprint count
    uint16_t validCount = 0;
    if (fingerprint2.PS_ValidTemplateNum(validCount) == FINGERPRINT_OK) {
        Serial.printf("Enrolled fingerprint count: %d\n", validCount);
    }
}

void manageFingerprints() {
    // Read index table to see which positions are occupied (only first 100 positions are valid)
    uint8_t indexTable[32] = {0};
  
    if (fingerprint2.PS_ReadIndexTable(indexTable) == FINGERPRINT_OK) {
        Serial.println("Fingerprint library occupancy status (only first 100 positions shown):");

        // Index table always returns 32 bytes, but only first 100 bits are valid (template ID 0-99)
        for (int templateID = 0; templateID < 100; ++templateID) {
            int byteIndex = templateID / 8;
            int bitIndex  = templateID % 8;   // LSB first

            bool isOccupied = (indexTable[byteIndex] & (1 << bitIndex)) != 0;
            if (isOccupied) {
                Serial.printf("Position %d: Occupied\n", templateID);
            }
        }
    }
}

// Batch delete fingerprints
void clearAllFingerprints() {
    if (fingerprint2.PS_Empty() == FINGERPRINT_OK) {
        Serial.println("Fingerprint library cleared");
    } else {
        Serial.println("Failed to clear fingerprint library");
    }
}

// Delete fingerprints in specified range
void deleteRangeFingerprints(uint16_t startID, uint16_t count) {
    if (fingerprint2.PS_DeletChar(startID, count) == FINGERPRINT_OK) {
        Serial.printf("Deleted fingerprints %d to %d\n", startID, startID + count - 1);
    } else {
        Serial.printf("Failed to delete fingerprint range\n");
    }
}
```

> **Tip**: `PS_ReadIndexTable` currently reads index table 0 only, and automatically clears data after bit 100 (keeping only 0-99, 100 template positions total). For extended logic, simply count or filter when iterating through the first 100 positions.

### Template Data Management

```cpp
void templateOperations() {
    // Upload fingerprint template
    uint8_t templateBuffer[1024] = {0}; // Template buffer
    uint32_t actualSize = 0;
  
    // Auto-upload template (from device)
    if (fingerprint2.PS_UploadTemplateAuto(templateBuffer, sizeof(templateBuffer), actualSize) == FINGERPRINT_OK) {
        Serial.printf("Template uploaded successfully, size: %d bytes\n", actualSize);
  
        // Can save template data to file or send to server
        saveTemplateToFile(templateBuffer, actualSize);
    }
  
    // Download template to device
    if (loadTemplateFromFile(templateBuffer, actualSize)) {
        if (fingerprint2.PS_DownloadTemplateAuto(templateBuffer, actualSize) == FINGERPRINT_OK) {
            Serial.println("Template downloaded successfully");
        }
    }
}

bool saveTemplateToFile(const uint8_t* data, uint32_t size) {
    // Implement save to file logic
    // This is just an example
    Serial.printf("Saving %d bytes of template data to file\n", size);
    return true;
}

bool loadTemplateFromFile(uint8_t* buffer, uint32_t& size) {
    // Implement load from file logic
    // This is just an example
    Serial.println("Loading template data from file");
    size = 512; // Example size
    return true;
}
```

### Notepad Functions

```cpp
void notepadOperations() {
    // Write notepad data
    const char* userData = "User123-Admin-2025";
    uint8_t notepadData[32] = {0};
    memcpy(notepadData, userData, min(strlen(userData), 32));
  
    if (fingerprint2.PS_WriteNotepad(0, notepadData, strlen(userData)) == FINGERPRINT_OK) {
        Serial.println("Notepad write successful");
    }
  
    // Read notepad data
    uint8_t readData[32] = {0};
    if (fingerprint2.PS_ReadNotepad(0, readData) == FINGERPRINT_OK) {
        Serial.printf("Notepad content: %s\n", (char*)readData);
    }
}
```

### Advanced LED Control

```cpp
void advancedLEDControl() {
    // Show white breathing light at startup
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_BREATHING,
        FINGERPRINT_LED_COLOR_WHITE,
        FINGERPRINT_LED_COLOR_WHITE,
        0 // Infinite loop
    );
  
    delay(3000);
  
    // Enrollment mode: Yellow always-on
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_ON,
        FINGERPRINT_LED_COLOR_YELLOW,
        FINGERPRINT_LED_COLOR_YELLOW,
        0
    );
  
    delay(2000);
  
    // Verification mode: Blue always-on
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_ON,
        FINGERPRINT_LED_COLOR_BLUE,
        FINGERPRINT_LED_COLOR_BLUE,
        0
    );
  
    delay(2000);
  
    // Success: Green flashing 3 times
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_FLASHING,
        FINGERPRINT_LED_COLOR_GREEN,
        FINGERPRINT_LED_COLOR_GREEN,
        3
    );
  
    delay(2000);
  
    // Failure: Red flashing 5 times
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_FLASHING,
        FINGERPRINT_LED_COLOR_RED,
        FINGERPRINT_LED_COLOR_RED,
        5
    );
  
    delay(3000);
  
    // Turn off LED
    fingerprint2.PS_ControlBLN(
        FINGERPRINT_LED_OFF,
        FINGERPRINT_LED_COLOR_OFF,
        FINGERPRINT_LED_COLOR_OFF,
        0
    );
}
```

### Image Quality Check

```cpp
void imageQualityCheck() {
    // Capture fingerprint image
    if (fingerprint2.PS_GetImage() == FINGERPRINT_OK) {
        Serial.println("Image captured successfully");
  
        // Check image quality
        uint8_t imageArea, imageQuality;
        if (fingerprint2.PS_GetImageInfo(imageArea, imageQuality) == FINGERPRINT_OK) {
            Serial.printf("Image area: %d%%\n", imageArea);
            Serial.printf("Image quality: %s\n", imageQuality == 0 ? "Acceptable" : "Unacceptable");
      
            if (imageQuality != 0) {
                Serial.println("Poor image quality, please press finger again");
                // Set red LED indicator
                fingerprint2.PS_ControlBLN(
                    FINGERPRINT_LED_FLASHING,
                    FINGERPRINT_LED_COLOR_RED,
                    FINGERPRINT_LED_COLOR_RED,
                    2
                );
                return;
            }
      
            if (imageArea < 50) { // If image area is less than 50%
                Serial.println("Insufficient fingerprint area, please press completely");
                // Set yellow LED indicator
                fingerprint2.PS_ControlBLN(
                    FINGERPRINT_LED_FLASHING,
                    FINGERPRINT_LED_COLOR_YELLOW,
                    FINGERPRINT_LED_COLOR_YELLOW,
                    2
                );
                return;
            }
      
            // Good image quality
            Serial.println("Good image quality, can proceed with processing");
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

## Error Handling and Status Codes

The library defines a complete status code system, mainly including:

- `FINGERPRINT_OK` (0x00): Operation successful
- `FINGERPRINT_PACKETRECIEVEERR` (0x01): Packet receive error
- `FINGERPRINT_NOFINGER` (0x02): No finger on sensor
- `FINGERPRINT_IMAGEFAIL` (0x03): Failed to capture fingerprint image
- `FINGERPRINT_IMAGEMESS` (0x06): Fingerprint image too messy to generate features
- `FINGERPRINT_FEATUREFAIL` (0x07): Feature generation failed
- `FINGERPRINT_NOMATCH` (0x08): Fingerprints do not match
- `FINGERPRINT_NOTFOUND` (0x09): No fingerprint found
- `FINGERPRINT_ENROLLMISMATCH` (0x0A): Failed to merge features

```cpp
void handleFingerprintError(fingerprint_status_t status) {
    switch (status) {
        case FINGERPRINT_OK:
            Serial.println("Operation successful");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println("Please press finger");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Image capture failed, please retry");
            break;
        case FINGERPRINT_NOMATCH:
            Serial.println("Fingerprints do not match");
            break;
        case FINGERPRINT_NOTFOUND:
            Serial.println("Fingerprint not found");
            break;
        default:
            Serial.printf("Unknown error, status code: 0x%02X\n", status);
            break;
    }
}
```

## Troubleshooting

### Common Issues

#### 1. Sensor Initialization Failed

```cpp
if (!fingerprint2.begin()) {
    Serial.println("Initialization failed, please check:");
    Serial.println("1. Is serial connection correct");
    Serial.println("2. Are TX/RX pins correct");
    Serial.println("3. Is power supply normal");
    Serial.println("4. Are module address and password correct");
}
```

#### 2. Fingerprint Enrollment Failed

```cpp
fingerprint_status_t status = fingerprint2.PS_AutoEnroll(id, 6, FINGERPRINT_AUTO_ENROLL_DEFAULT);
if (status != FINGERPRINT_OK) {
    switch (status) {
        case FINGERPRINT_NOFINGER:
            Serial.println("Please press finger on sensor");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Image capture failed, please clean sensor");
            break;
        case FINGERPRINT_ENROLLMISMATCH:
            Serial.println("Multiple captures don't match, please restart");
            break;
        default:
            Serial.printf("Enrollment failed, error code: 0x%02X\n", status);
            break;
    }
}
```

#### 3. Fingerprint Recognition Failed

```cpp
uint16_t matchedID = 0;
fingerprint_status_t status = fingerprint2.PS_AutoIdentify(3, 0, FINGERPRINT_AUTO_VERIFY_DEFAULT, matchedID);
if (status != FINGERPRINT_OK) {
    switch (status) {
        case FINGERPRINT_NOFINGER:
            Serial.println("Please press finger on sensor");
            break;
        case FINGERPRINT_NOTFOUND:
            Serial.println("Fingerprint not found, please enroll first");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Poor image quality, please press again");
            break;
        default:
            Serial.printf("Recognition failed, error code: 0x%02X\n", status);
            break;
    }
}
```

## Version Information

- **Current Version**: 1.1.0
- **Author**: M5Stack
- **Maintainer**: M5Stack
- **License**: MIT

## More Information

- [M5Stack Official Website](http://M5Stack.com)
- [Project Repository](https://github.com/m5stack/M5Unit-Fingerprint2)
- [Hardware Documentation](http://M5Stack.com)

---

**Note**: Please ensure hardware is properly connected and pins are correctly configured before use. It is recommended to implement proper error handling and security measures in production environments.

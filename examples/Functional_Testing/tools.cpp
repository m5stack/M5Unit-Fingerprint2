#include "tools.hpp"

extern M5Canvas canvas;

// 线程控制相关变量 / Thread control related variables
TaskHandle_t fingerDetectionTaskHandle = NULL;
bool fingerDetectionEnabled            = true;  // 指纹检测线程是否启用 / Whether finger detection thread is enabled
bool touchDetectionEnabled             = true;  // 触摸检测是否启用 / Whether touch detection is enabled
int fingerPresentCount                 = 0;     // 手指存在的连续计数 / Consecutive count of finger presence

bool displayFingerprintImage(M5UnitFingerprint2& fp2, uint32_t maxBufferSize, M5Canvas& canvas,
                                  int displayX, int displayY, uint8_t rotation)
{
    // 获取指纹图像 / Get fingerprint image
    if (fp2.PS_GetImage() != FINGERPRINT_OK) {
        Serial.println("Failed to get fingerprint image.");
        canvas.setCursor(2, 60);
        canvas.printf("Failed to get image");
        return false;
    }

    // 分配内存 / Allocate memory
    uint8_t* imageBuffer = (uint8_t*)malloc(maxBufferSize);
    if (imageBuffer == nullptr) {
        Serial.println("Failed to allocate memory for image buffer.");
        canvas.setCursor(2, 60);
        canvas.printf("Memory allocation failed");
        return false;
    }

    // 上传图像 / Upload image
    uint32_t actualImageSize    = 0;
    fingerprint_status_t result = fp2.PS_UpImage(imageBuffer, maxBufferSize, actualImageSize);

    if (result == FINGERPRINT_OK) {
        Serial.printf("Image uploaded successfully, size: %d bytes\r\n", actualImageSize);

        // 在画布上显示图像大小信息 / Display image size information on canvas
        canvas.fillRect(0, 41, 320, 240, 0x0000);
        canvas.setCursor(2, 60);
        canvas.printf("Image size: %d bytes", actualImageSize);

        // 显示指纹图像 (80x208, 4bit per pixel) 支持不同旋转角度 / Display fingerprint image (80x208, 4bit per pixel) with different rotation angles support
        if (actualImageSize > 0) {
            const int imageWidth  = 80;
            const int imageHeight = 208;

            // 遍历图像数据 / Iterate through image data
            for (int y = 0; y < imageHeight && y < 220; y++) {     // 限制高度避免超出屏幕 / Limit height to avoid exceeding screen
                for (int x = 0; x < imageWidth && x < 200; x++) {  // 限制宽度避免超出屏幕 / Limit width to avoid exceeding screen
                    // 计算像素在缓冲区中的位置 (每字节包含2个像素) / Calculate pixel position in buffer (each byte contains 2 pixels)
                    int byteIndex = (y * imageWidth + x) / 2;
                    if (byteIndex < actualImageSize) {
                        // 获取4bit像素值 (0-15) / Get 4bit pixel value (0-15)
                        uint8_t pixelValue;
                        if ((y * imageWidth + x) % 2 == 0) {
                            // 偶数像素取低4位 / Even pixels take low 4 bits
                            pixelValue = imageBuffer[byteIndex] & 0x0F;
                        } else {
                            // 奇数像素取高4位 / Odd pixels take high 4 bits
                            pixelValue = (imageBuffer[byteIndex] & 0xF0) >> 4;
                        }

                        // 将4bit值(0-15)转换为RGB565 (白色到黑色的渐变) / Convert 4bit value (0-15) to RGB565 (white to black gradient)
                        // 0=黑色, 15=白色 / 0=black, 15=white
                        uint8_t grayValue = pixelValue * 17;  // 0-255范围 / 0-255 range
                        uint16_t rgb565   = ((grayValue >> 3) << 11) | ((grayValue >> 2) << 5) | (grayValue >> 3);

                        // 根据rotation参数计算旋转后的像素位置 / Calculate rotated pixel position based on rotation parameter
                        int drawX, drawY;
                        switch (rotation) {
                            case 0:  // 不旋转 / No rotation
                                drawX = displayX + x;
                                drawY = displayY + y;
                                break;
                            case 1:  // 顺时针90度 / Clockwise 90 degrees
                                drawX = displayX + imageHeight - 1 - y;
                                drawY = displayY + x;
                                break;
                            case 2:  // 180度 / 180 degrees
                                drawX = displayX + imageWidth - 1 - x;
                                drawY = displayY + imageHeight - 1 - y;
                                break;
                            case 3:  // 逆时针90度 (或顺时针270度) / Counterclockwise 90 degrees (or clockwise 270 degrees)
                                drawX = displayX + y;
                                drawY = displayY + imageWidth - 1 - x;
                                break;
                            default:  // 默认顺时针90度 / Default clockwise 90 degrees
                                drawX = displayX + imageHeight - 1 - y;
                                drawY = displayY + x;
                                break;
                        }

                        // 在画布上绘制像素 / Draw pixel on canvas
                        canvas.drawPixel(drawX, drawY, rgb565);
                    }
                }
            }
        }

        // 释放内存 / Free memory
        free(imageBuffer);
        return true;

    } else {
        Serial.println("Failed to upload image.");
        canvas.setCursor(2, 60);
        canvas.printf("Failed to upload image");

        // 释放内存 / Free memory
        free(imageBuffer);
        return false;
    }
}

bool displaySystemParameters(M5UnitFingerprint2& fp2, M5Canvas& canvas, int displayX, int displayY,
                                  bool clearArea)
{
    // 读取系统参数 / Read system parameters
    PS_ReadSysPara_BasicParams sysParams;
    fingerprint_status_t result = fp2.PS_ReadSysPara(sysParams);

    if (result != FINGERPRINT_OK) {
        Serial.println("Failed to read system parameters.");
        canvas.setCursor(displayX, displayY);
        canvas.printf("Failed to read sys params");
        return false;
    }

    // 如果需要清除显示区域 / If need to clear display area
    if (clearArea) {
        canvas.fillRect(displayX, displayY, 320 - displayX, 200, 0x0000);  // 清除显示区域 / Clear display area
    }

    // 显示系统参数 / Display system parameters
    int lineHeight = 20;  // 每行的高度 / Height per line
    int currentY   = displayY;

    canvas.setCursor(displayX, currentY);
    canvas.printf("Status Reg: 0x%04X", sysParams.status_register);
    currentY += lineHeight;

    canvas.setCursor(displayX, currentY);
    canvas.printf("Template Size: %d bytes", sysParams.temp_size);
    currentY += lineHeight;

    canvas.setCursor(displayX, currentY);
    canvas.printf("Data Size: %d bytes", sysParams.data_size);
    currentY += lineHeight;

    canvas.setCursor(displayX, currentY);
    canvas.printf("Score Level: %d", sysParams.score_level);
    currentY += lineHeight;

    canvas.setCursor(displayX, currentY);
    canvas.printf("Device Addr: 0x%08X", sysParams.device_addr);
    currentY += lineHeight;

    canvas.setCursor(displayX, currentY);
    // 0:32 bytes, 1:64 bytes, 2:128 bytes, 3:256 bytes
    canvas.printf("Packet Size: ");
    switch (sysParams.packet_size) {
        case 0:
            canvas.printf("32 bytes");
            break;
        case 1:
            canvas.printf("64 bytes");
            break;
        case 2:
            canvas.printf("128 bytes");
            break;
        case 3:
            canvas.printf("256 bytes");
            break;
        default:
            canvas.printf("Unknown");
            break;
    }
    currentY += lineHeight;

    canvas.setCursor(displayX, currentY);
    canvas.printf("Baud Rate: %d bps", sysParams.baud_rate * 9600);

    Serial.println("System parameters displayed successfully.");
    return true;
}

bool displayMenu(M5Canvas& canvas, int displayX, int displayY)
{
    // 清除显示区域
    canvas.fillRect(displayX, displayY, 320, 240, 0x0000);

    // 获取可用显示区域的宽度和高度
    int availableWidth  = canvas.width() - displayX;
    int availableHeight = canvas.height() - displayY;

    // 计算菜单布局参数 (2x2布局)
    int margin = 10;  // 边界间隙
    int gapX   = 15;  // 水平方框之间的间隙
    int gapY   = 15;  // 垂直方框之间的间隙

    // 计算每个方框的宽度和高度 (考虑边界和间隙)
    int boxWidth  = (availableWidth - 2 * margin - gapX) / 2;
    int boxHeight = (availableHeight - 2 * margin - gapY) / 2;

    // 确保方框不会太小
    if (boxWidth < 80) {
        boxWidth = 80;
        margin   = (availableWidth - 2 * boxWidth - gapX) / 2;
        if (margin < 5) margin = 5;
    }
    if (boxHeight < 50) {
        boxHeight = 50;
    }

    // 计算四个方框的位置
    int topLeftX     = displayX + margin;
    int topLeftY     = displayY + margin;
    int topRightX    = topLeftX + boxWidth + gapX;
    int topRightY    = topLeftY;
    int bottomLeftX  = topLeftX;
    int bottomLeftY  = topLeftY + boxHeight + gapY;
    int bottomRightX = topRightX;
    int bottomRightY = bottomLeftY;

    // 设置文本颜色和背景色
    uint16_t boxColor  = 0x071F;  // 深蓝色边框
    uint16_t fillColor = 0x18E3;  // 浅蓝色填充
    uint16_t textColor = 0xFFFF;  // 白色文字

    canvas.setTextColor(textColor);
    canvas.setTextSize(2);
    canvas.setTextDatum(middle_center);

    // 绘制左上方框 (Auto Enroll)
    canvas.fillRoundRect(topLeftX, topLeftY, boxWidth, boxHeight, 8, fillColor);
    canvas.drawRoundRect(topLeftX, topLeftY, boxWidth, boxHeight, 8, boxColor);
    canvas.drawString("Auto", topLeftX + boxWidth / 2, topLeftY + boxHeight / 2 - 10);
    canvas.drawString("Enroll", topLeftX + boxWidth / 2, topLeftY + boxHeight / 2 + 10);

    // 绘制右上方框 (Auto Identify)
    canvas.fillRoundRect(topRightX, topRightY, boxWidth, boxHeight, 8, fillColor);
    canvas.drawRoundRect(topRightX, topRightY, boxWidth, boxHeight, 8, boxColor);
    canvas.drawString("Auto", topRightX + boxWidth / 2, topRightY + boxHeight / 2 - 10);
    canvas.drawString("Identify", topRightX + boxWidth / 2, topRightY + boxHeight / 2 + 10);

    // 绘制左下方框 (Up/Down Template)
    canvas.fillRoundRect(bottomLeftX, bottomLeftY, boxWidth, boxHeight, 8, fillColor);
    canvas.drawRoundRect(bottomLeftX, bottomLeftY, boxWidth, boxHeight, 8, boxColor);
    canvas.drawString("Up/Down", bottomLeftX + boxWidth / 2, bottomLeftY + boxHeight / 2 - 10);
    canvas.drawString("Template", bottomLeftX + boxWidth / 2, bottomLeftY + boxHeight / 2 + 10);

    // 绘制右下方框 (System Parameters)
    canvas.fillRoundRect(bottomRightX, bottomRightY, boxWidth, boxHeight, 8, fillColor);
    canvas.drawRoundRect(bottomRightX, bottomRightY, boxWidth, boxHeight, 8, boxColor);
    canvas.drawString("System", bottomRightX + boxWidth / 2, bottomRightY + boxHeight / 2 - 10);
    canvas.drawString("Parameters", bottomRightX + boxWidth / 2, bottomRightY + boxHeight / 2 + 10);

    // 重置文本对齐方式
    canvas.setTextDatum(top_left);

    Serial.printf(
        "Menu displayed: TopLeft(%d,%d), TopRight(%d,%d), BottomLeft(%d,%d), BottomRight(%d,%d), Box size: %dx%d\r\n",
        topLeftX, topLeftY, topRightX, topRightY, bottomLeftX, bottomLeftY, bottomRightX, bottomRightY, boxWidth,
        boxHeight);

    return true;
}

bool initializeFingerprintSensor(M5UnitFingerprint2& fp2, M5Canvas& canvas, int maxRetries, int retryDelay)
{
    Serial.println("Starting fingerprint sensor initialization...");

    int retryCount = 0;
    while (maxRetries == 0 || retryCount < maxRetries) {  // maxRetries为0时无限重试
        if (maxRetries == 0) {
            Serial.printf("Initialization attempt %d (infinite retry mode)\n", retryCount + 1);
        } else {
            Serial.printf("Initialization attempt %d of %d\n", retryCount + 1, maxRetries);
        }

        // 检查指纹传感器是否连接成功
        uint8_t ModuleStatus;
    if (fp2.PS_GetFingerprintModuleStatus(ModuleStatus) == FINGERPRINT_OK) {
            Serial.printf("Module status: 0x%02X\n", ModuleStatus);

            // 启动指纹传感器
            if (fp2.PS_ActivateFingerprintModule() == FINGERPRINT_OK) {
                Serial.println("Fingerprint module activated successfully.");

                // 获取版本号
                uint8_t FwVersion;
                if (fp2.PS_GetFirmwareVersion(FwVersion) == FINGERPRINT_OK) {
                    Serial.printf("Fp2 is active. -> Firmware FwVersion: 0x%02X\r\n", FwVersion);
                    canvas.setCursor(2, 20);
                    canvas.printf("Fp2 is active. -> FW:0x%02X ", FwVersion);
                    canvas.pushSprite(0, 0);  // 将画布内容显示到屏幕上

                    // 获取芯片序列号
                    uint8_t ChipSN[32] = {0};  // 创建一个32字节的缓冲区来存储芯片序列号
                    if (fp2.PS_GetChipSN(ChipSN) == FINGERPRINT_OK) {
                        Serial.print("Chip SN: ");
                        for (int i = 0; i < 32; i++) {
                            Serial.printf("%02X", ChipSN[i]);
                        }
                        Serial.println("");
                        Serial.println("Chip SN acquired successfully.");
                    } else {
                        Serial.println("Failed to get chip SN.");
                    }

                    // 握手指令
                    if (fp2.PS_HandShake() == FINGERPRINT_OK) {
                        Serial.println("Handshake successful.");
                    } else {
                        Serial.println("Failed to perform handshake.");
                    }

                    // 校验传感器
                    if (fp2.PS_CheckSensor() == FINGERPRINT_OK) {
                        Serial.println("Sensor check passed.");
                    } else {
                        Serial.println("Sensor check failed.");
                    }

                    Serial.println("Fingerprint sensor initialization completed successfully!");
                    return true;  // 完成所有初始化检查，返回成功

                } else {
                    Serial.println("Failed to get firmware FwVersion.");
                    canvas.drawString("Failed to get fw FwVersion.", 2, 20);
                    canvas.pushSprite(0, 0);
                }
            } else {
                Serial.println("Fp2 sensor is not active.");
                canvas.drawString("Fp2 sensor is not active.", 2, 20);
                canvas.pushSprite(0, 0);
            }
        } else {
            Serial.println("Fp2 sensor not connected.");
            canvas.drawString("Fp2 sensor not connected.", 2, 20);
            canvas.pushSprite(0, 0);
        }

        retryCount++;
        if (maxRetries == 0 || retryCount < maxRetries) {
            Serial.printf("Retrying in %d ms...\n", retryDelay);
            delay(retryDelay);  // 等待后重试
        }
    }

    // 只有在限制重试次数模式下才会到达这里
    Serial.printf("Fingerprint sensor initialization failed after %d attempts.\n", maxRetries);
    canvas.drawString("Fp2 init failed", 2, 20);
    canvas.pushSprite(0, 0);
    return false;  // 达到最大重试次数，返回失败
}

bool PS_AutoEnroll_callback(uint16_t ID, fingerprint_status_t confirmationCode, uint8_t param1, uint8_t param2,
                            int packetCount)
{
    fingerprint_auto_enroll_stage_t stage = (fingerprint_auto_enroll_stage_t)param1;

    switch (stage) {
        case FINGERPRINT_AUTO_ENROLL_LEGAL_CHECK:
            // Serial.printf("AutoEnroll [%d]: Stage LEGAL_CHECK, code=0x%02X\n", packetCount, confirmationCode);
            canvas.setCursor(2, 60);
            canvas.fillRect(2, 60, 320, 20, 0x0000);  // 清除之前的内容
            canvas.printf("[#%d]: LEGAL_CHECK(0x%02X)", packetCount, confirmationCode);
            break;

        case FINGERPRINT_AUTO_ENROLL_GET_IMAGE:
            if (confirmationCode == FINGERPRINT_OK) {
                // Serial.printf("AutoEnroll [%d]: Image acquired\n", packetCount);
                canvas.setCursor(2, 80);
                canvas.fillRect(2, 80, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: T:%d GetImage ok", packetCount, (packetCount + 1) / 3);
            } else {
                // Serial.printf("AutoEnroll [%d]: Failed to get image, code=0x%02X\n", packetCount, confirmationCode);
                canvas.setCursor(2, 80);
                canvas.fillRect(2, 80, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: T:%d GetImage failed(0x%02X)", packetCount, (packetCount + 1) / 3,
                              confirmationCode);
            }
            break;

        case FINGERPRINT_AUTO_ENROLL_GEN_CHAR:
            if (confirmationCode == FINGERPRINT_OK) {
                // Serial.printf("AutoEnroll [%d]: Characteristic generated\n", packetCount);
                canvas.setCursor(2, 100);
                canvas.fillRect(2, 100, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: T:%d GenChar ok", packetCount, packetCount / 3);
            } else {
                // Serial.printf("AutoEnroll [%d]: GenChar failed, code=0x%02X\n", packetCount, confirmationCode);
                canvas.setCursor(2, 100);
                canvas.fillRect(2, 100, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: T:%d GenChar failed(0x%02X)", packetCount, (packetCount + 1) / 3,
                              confirmationCode);
            }
            break;

        case FINGERPRINT_AUTO_ENROLL_CHECK_LIFT:
            // Serial.printf("AutoEnroll [%d]: Stage CHECK_LIFT, status=0x%02X\n", packetCount, param2);
            canvas.setCursor(2, 120);
            canvas.fillRect(2, 120, 320, 20, 0x0000);  // 清除之前的内容
            if (confirmationCode == FINGERPRINT_OK) {
                canvas.printf("[#%d]: T:%d CheckLift ok", packetCount, (packetCount - 1) / 3);
            } else {
                canvas.printf("[#%d]: T:%d CheckLift failed(0x%02X)", packetCount, (packetCount + 1) / 3,
                              confirmationCode);
            }
            break;

        case FINGERPRINT_AUTO_ENROLL_MERGE_TEMPLATE:
            if (confirmationCode == FINGERPRINT_OK) {
                // Serial.printf("AutoEnroll [%d]: Template merged\n", packetCount);
                canvas.setCursor(2, 140);
                canvas.fillRect(2, 140, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Merge ok", packetCount);
            } else {
                // Serial.printf("AutoEnroll [%d]: Merge failed, code=0x%02X\n", packetCount, confirmationCode);
                canvas.setCursor(2, 140);
                canvas.fillRect(2, 140, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Merge failed(0x%02X)", packetCount, confirmationCode);
            }
            break;

        case FINGERPRINT_AUTO_ENROLL_VERIFY:
            if (confirmationCode == FINGERPRINT_OK) {
                // Serial.printf("AutoEnroll [%d]: Template verified\n", packetCount);
                canvas.setCursor(2, 160);
                canvas.fillRect(2, 160, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Verify ok", packetCount);
            } else {
                // Serial.printf("AutoEnroll [%d]: Verify failed, code=0x%02X\n", packetCount, confirmationCode);
                canvas.setCursor(2, 160);
                canvas.fillRect(2, 160, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Verify failed(0x%02X)", packetCount, confirmationCode);
            }
            break;

        case FINGERPRINT_AUTO_ENROLL_STORE_TEMPLATE:
            if (confirmationCode == FINGERPRINT_OK) {
                // Serial.printf("AutoEnroll complete! ID=%d stored.\n", ID);
                canvas.setCursor(2, 180);
                canvas.fillRect(2, 180, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Store ok, ID=%d", packetCount, ID);
            } else {
                // Serial.printf("AutoEnroll [%d]: Store failed, code=0x%02X\n", packetCount, confirmationCode);
                canvas.setCursor(2, 180);
                canvas.fillRect(2, 180, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Store failed(0x%02X)", packetCount, confirmationCode);
            }
            break;

        default:
            // Serial.printf("AutoEnroll [%d]: Unknown stage %d\n", packetCount, param1);
            canvas.setCursor(2, 200);
            canvas.fillRect(2, 200, 320, 20, 0x0000);  // 清除之前的内容
            canvas.printf("[#%d]: Unknown stage %d", packetCount, param1);
            break;
    }

    canvas.pushSprite(0, 0);  // 刷新画布

    // 返回 true 继续自动注册流程，false 则中断
    return (confirmationCode == FINGERPRINT_OK);
}

// PS_AutoIdentify 回调函数
bool PS_AutoIdentify_callback(uint8_t securityLevel, fingerprint_status_t confirmationCode, uint8_t param,
                              uint16_t matchedID, uint16_t matchScore, int packetCount)
{
    ps_auto_identify_param_t stage = (ps_auto_identify_param_t)param;

    switch (stage) {
        case PS_AUTO_IDENTIFY_PARAM_LEGAL_CHECK:
            // Serial.printf("AutoIdentify [%d]: Stage LEGAL_CHECK, code=0x%02X\n", packetCount, confirmationCode);
            canvas.setCursor(2, 60);
            canvas.fillRect(2, 60, 320, 20, 0x0000);  // 清除之前的内容
            canvas.printf("[#%d]: LEGAL_CHECK(0x%02X)", packetCount, confirmationCode);
            break;

        case PS_AUTO_IDENTIFY_PARAM_GET_IMAGE:
            if (confirmationCode == FINGERPRINT_OK) {
                // Serial.printf("AutoIdentify [%d]: Image acquired\n", packetCount);
                canvas.setCursor(2, 80);
                canvas.fillRect(2, 80, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: GetImage ok", packetCount);
            } else {
                // Serial.printf("AutoIdentify [%d]: Failed to get image, code=0x%02X\n", packetCount,
                // confirmationCode);
                canvas.setCursor(2, 80);
                canvas.fillRect(2, 80, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: GetImage failed(0x%02X)", packetCount, confirmationCode);
            }
            break;

        case PS_AUTO_IDENTIFY_PARAM_VERIFY:
            if (confirmationCode == FINGERPRINT_OK) {
                // Serial.printf("AutoIdentify [%d]: Verification successful, ID=%d, Score=%d\n", packetCount,
                // matchedID, matchScore);
                canvas.setCursor(2, 100);
                canvas.fillRect(2, 100, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Match!", packetCount);
                canvas.setCursor(2, 120);
                canvas.fillRect(2, 120, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("ID=%d Score=%d", matchedID, matchScore);
            } else {
                // Serial.printf("AutoIdentify [%d]: Verification failed, code=0x%02X\n", packetCount,
                // confirmationCode);
                canvas.setCursor(2, 100);
                canvas.fillRect(2, 100, 320, 20, 0x0000);  // 清除之前的内容
                canvas.printf("[#%d]: Verify failed(0x%02X)", packetCount, confirmationCode);
            }
            break;

        default:
            // Serial.printf("AutoIdentify [%d]: Unknown stage %d\n", packetCount, param);
            canvas.setCursor(2, 120);
            canvas.fillRect(2, 120, 320, 20, 0x0000);  // 清除之前的内容
            canvas.printf("[#%d]: Unknown stage %d", packetCount, param);
            break;
    }

    canvas.pushSprite(0, 0);  // 刷新画布

    // 返回 true 继续自动识别流程，false 则中断
    return (confirmationCode == FINGERPRINT_OK || stage == PS_AUTO_IDENTIFY_PARAM_LEGAL_CHECK);
}

// 触摸检测函数
void handleTouchInput(M5Canvas& canvas, M5UnitFingerprint2& fp2)
{
    if (!touchDetectionEnabled) {
        return;  // 触摸检测被禁用时直接返回
    }

    // 检查触摸输入
    if (M5.Touch.getCount()) {
        auto touch = M5.Touch.getDetail();
        if (touch.wasPressed()) {
            int touchX = touch.x;
            int touchY = touch.y;

            // 计算菜单区域参数 (与displayMenu函数中的计算保持一致)
            int displayX        = 0;
            int displayY        = 45;
            int availableWidth  = canvas.width() - displayX;
            int availableHeight = canvas.height() - displayY;

            int margin = 20;  // 边界间隙
            int gapX   = 15;  // 水平方框之间的间隙
            int gapY   = 15;  // 垂直方框之间的间隙

            // 计算每个方框的宽度和高度
            int boxWidth  = (availableWidth - 2 * margin - gapX) / 2;
            int boxHeight = (availableHeight - 2 * margin - gapY) / 2;

            // 确保方框不会太小
            if (boxWidth < 80) {
                boxWidth = 80;
                margin   = (availableWidth - 2 * boxWidth - gapX) / 2;
                if (margin < 5) margin = 5;
            }
            if (boxHeight < 50) {
                boxHeight = 50;
            }

            // 计算四个方框的位置
            int topLeftX     = displayX + margin;
            int topLeftY     = displayY + margin;
            int topRightX    = topLeftX + boxWidth + gapX;
            int topRightY    = topLeftY;
            int bottomLeftX  = topLeftX;
            int bottomLeftY  = topLeftY + boxHeight + gapY;
            int bottomRightX = topRightX;
            int bottomRightY = bottomLeftY;

            // 检查是否点击了左上方框 (Auto Enroll)
            if (touchX >= topLeftX && touchX <= topLeftX + boxWidth && touchY >= topLeftY &&
                touchY <= topLeftY + boxHeight) {
                Serial.println("Auto Enroll button clicked!");

                // 立刻销毁指纹检测线程
                if (fingerDetectionTaskHandle != NULL) {
                    Serial.println("Terminating finger detection task due to button press.");
                    vTaskDelete(fingerDetectionTaskHandle);
                    fingerDetectionTaskHandle = NULL;
                    fingerDetectionEnabled    = false;
                    fingerPresentCount        = 0;
                }

                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                canvas.pushSprite(0, 0);                   // 刷新画布

                // 这里可以添加Auto Enroll的具体功能
                // 例如：调用PS_AutoEnroll函数
                uint16_t ls_id         = 9;
                uint8_t ls_enrollCount = 3;
                uint8_t ls_param_1     = 0;
                uint8_t ls_param_2     = 0;
                Serial.printf("Starting Auto Enroll with ID: %d, Count: %d\n", ls_id, ls_enrollCount);

                // 自动录取指纹的过程中不需要抬起手指
                // fingerprint_auto_enroll_flags_t flags =
                // (fingerprint_auto_enroll_flags_t)(FINGERPRINT_AUTO_ENROLL_ALLOW_OVERWRITE_ID |
                //                                   FINGERPRINT_AUTO_ENROLL_NO_LIFT_REQUIRED);

                // 自动录取指纹的过程中需要抬起手指
                fingerprint_auto_enroll_flags_t flags =
                    (fingerprint_auto_enroll_flags_t)(FINGERPRINT_AUTO_ENROLL_ALLOW_OVERWRITE_ID);
                fp2.PS_AutoEnroll(ls_id, ls_enrollCount, flags, &ls_param_1, &ls_param_2,
                                           PS_AutoEnroll_callback);

                // 等待5s
                delay(5000);
                // 回到菜单
                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                displayMenu(canvas, 0, 45);                // 重新显示菜单
                canvas.pushSprite(0, 0);                   // 刷新画布
                // 重启指纹检测线程
                fingerDetectionEnabled = true;  // 重新启用指纹检测
                if (fingerDetectionTaskHandle == NULL) {
                    Serial.println("Restarting finger detection task.");
                    FingerDetectionParams params = { .fp2 = &fp2, .canvas = &canvas };
                    xTaskCreate(fingerDetectionTask, "FingerDetectionTask", 4096, &params, 1,
                                &fingerDetectionTaskHandle);
                }
            }
            // 检查是否点击了右上方框 (Auto Identify)
            else if (touchX >= topRightX && touchX <= topRightX + boxWidth && touchY >= topRightY &&
                     touchY <= topRightY + boxHeight) {
                Serial.println("Auto Identify button clicked!");

                // 立刻销毁指纹检测线程
                if (fingerDetectionTaskHandle != NULL) {
                    Serial.println("Terminating finger detection task due to button press.");
                    vTaskDelete(fingerDetectionTaskHandle);
                    fingerDetectionTaskHandle = NULL;
                    fingerDetectionEnabled    = false;
                    fingerPresentCount        = 0;
                }

                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                canvas.pushSprite(0, 0);                   // 刷新画布

                // Auto Identify 功能
                uint16_t matchedPageID = 0;
                fp2.PS_AutoIdentify(0, 0xFFFF, FINGERPRINT_AUTO_VERIFY_DEFAULT, matchedPageID,
                                             PS_AutoIdentify_callback);

                // 等待5s
                delay(5000);
                // 回到菜单
                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                displayMenu(canvas, 0, 45);                // 重新显示菜单
                canvas.pushSprite(0, 0);                   // 刷新画布
                // 重启指纹检测线程
                fingerDetectionEnabled = true;  // 重新启用指纹检测
                if (fingerDetectionTaskHandle == NULL) {
                    Serial.println("Restarting finger detection task.");
                    FingerDetectionParams params = { .fp2 = &fp2, .canvas = &canvas };
                    xTaskCreate(fingerDetectionTask, "FingerDetectionTask", 4096, &params, 1,
                                &fingerDetectionTaskHandle);
                }
            }
            // 检查是否点击了左下方框 (but3)
            else if (touchX >= bottomLeftX && touchX <= bottomLeftX + boxWidth && touchY >= bottomLeftY &&
                     touchY <= bottomLeftY + boxHeight) {

                // 立刻销毁指纹检测线程
                if (fingerDetectionTaskHandle != NULL) {
                    Serial.println("Terminating finger detection task due to button press.");
                    vTaskDelete(fingerDetectionTaskHandle);
                    fingerDetectionTaskHandle = NULL;
                    fingerDetectionEnabled    = false;
                    fingerPresentCount        = 0;
                }

                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                canvas.pushSprite(0, 0);                   // 刷新画布

                // Serial.println("=== Demo: Upload Template Auto (Automatic) ===");
                uint8_t* templateBuffer = (uint8_t*)malloc(16384);
                if (templateBuffer != nullptr) {
                    uint32_t totalSize = 0;  // 实际接收的总大小

                    // 先加载模板到缓冲区
                    fp2.PS_LoadChar(2, 9);

                    Serial.println("Starting automatic template upload...");
                    canvas.setCursor(2, 40);
                    canvas.printf("Auto uploading template...");
                    canvas.pushSprite(0, 0);

                    // 使用自动上传函数
                    fingerprint_status_t result =
                        fp2.PS_UploadTemplateAuto(templateBuffer,                 // 缓冲区
                                                           16384,                          // 缓冲区大小
                                                           totalSize,                      // 返回的实际大小
                                                           templateUploadProgressCallback  // 进度回调函数
                        );

                    // 画一条横线
                    canvas.drawLine(0, 130, 320, 130, 0xFFFF);  // 在画布上画一条横线

                    canvas.setCursor(2, 140);
                    canvas.printf("Auto download template...");
                    canvas.pushSprite(0, 0);

                    result = fp2.PS_DownloadTemplateAuto(templateBuffer,                   // 缓冲区
                                                                  totalSize,                        // 返回的实际大小
                                                                  templateDownloadProgressCallback  // 进度回调函数
                    );

                    result = fp2.PS_StoreChar(1, 10);

                    free(templateBuffer);
                } else {
                    Serial.println("Memory allocation failed for template buffer");
                    canvas.setCursor(2, 160);
                    canvas.printf("Memory allocation failed");
                    canvas.pushSprite(0, 0);
                }

                delay(5000);
                // 回到菜单
                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                displayMenu(canvas, 0, 45);                // 重新显示菜单
                canvas.pushSprite(0, 0);                   // 刷新画布
                
                // 重启指纹检测线程
                fingerDetectionEnabled = true;  // 重新启用指纹检测
                if (fingerDetectionTaskHandle == NULL) {
                    Serial.println("Restarting finger detection task.");
                    FingerDetectionParams params = { .fp2 = &fp2, .canvas = &canvas };
                    xTaskCreate(fingerDetectionTask, "FingerDetectionTask", 4096, &params, 1,
                                &fingerDetectionTaskHandle);
                }
            }
            // 检查是否点击了右下方框 (but4)
            else if (touchX >= bottomRightX && touchX <= bottomRightX + boxWidth && touchY >= bottomRightY &&
                     touchY <= bottomRightY + boxHeight) {

                // 立刻销毁指纹检测线程
                if (fingerDetectionTaskHandle != NULL) {
                    Serial.println("Terminating finger detection task due to button press.");
                    vTaskDelete(fingerDetectionTaskHandle);
                    fingerDetectionTaskHandle = NULL;
                    fingerDetectionEnabled    = false;
                    fingerPresentCount        = 0;
                }

                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                canvas.pushSprite(0, 0);                   // 刷新画布

                displaySystemParameters(fp2, canvas, 2, 60);
                canvas.pushSprite(0, 0);                   // 刷新画布

                delay(5000);
                // 回到菜单
                canvas.fillRect(0, 41, 320, 240, 0x0000);  // 清除画布
                displayMenu(canvas, 0, 45);                // 重新显示菜单
                canvas.pushSprite(0, 0);                   // 刷新画布

                // 重启指纹检测线程
                fingerDetectionEnabled = true;  // 重新启用指纹检测
                if (fingerDetectionTaskHandle == NULL) {
                    Serial.println("Restarting finger detection task.");
                    FingerDetectionParams params = { .fp2 = &fp2, .canvas = &canvas };
                    xTaskCreate(fingerDetectionTask, "FingerDetectionTask", 4096, &params, 1,
                                &fingerDetectionTaskHandle);
                }
            }

            // 延时防止重复触发
            delay(200);
        }
    }
}

// 指纹检测线程函数
void fingerDetectionTask(void* parameter)
{
    Serial.println("Finger detection task started.");

    // 从参数中提取指纹传感器和画布对象
    FingerDetectionParams* params = (FingerDetectionParams*)parameter;
    M5UnitFingerprint2* fp2       = params->fp2;
    M5Canvas* canvas              = params->canvas;

    while (fingerDetectionEnabled) {
        // 使用PS_GetImage检测手指是否存在
        fingerprint_status_t result = fp2->PS_GetImage();

        if (result == FINGERPRINT_OK) {
            fingerPresentCount++;
            Serial.printf("Finger detected! Count: %d/5\n", fingerPresentCount);

            // 如果持续5秒检测到手指存在
            if (fingerPresentCount >= 5) {
                Serial.println(
                    "Finger present for 5 seconds, entering displayFingerprintImage and disabling touch detection.");

                // 禁用触摸检测
                touchDetectionEnabled = false;

                // 进入指纹图像显示
                if (displayFingerprintImage(*fp2, 1024 * 16, *canvas, 56, 100, 1)) {
                    Serial.println("Fingerprint image displayed successfully.");
                } else {
                    Serial.println("Failed to display fingerprint image.");
                }

                // 重置计数器并重新启用触摸检测
                fingerPresentCount = 0;
                // touchDetectionEnabled = true;

                // 重新显示菜单
                // displayMenu(*canvas, 0, 45);

                // 刷新屏幕
                canvas->pushSprite(0, 0);
            }
        } else {
            // 没有检测到手指，重置计数器
            if (fingerPresentCount > 0) {
                Serial.println("Finger removed, resetting count.");
                fingerPresentCount = 0;
            }
        }

        // 每1秒检测一次
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    Serial.println("Finger detection task terminated.");
    fingerDetectionTaskHandle = NULL;
    vTaskDelete(NULL);
}

// 模板上传进度回调函数
bool templateUploadProgressCallback(uint16_t currentSize, uint32_t totalSize, int packetCount, bool isComplete)
{
    // 进度回调函数
    if (!isComplete) {
        // 更新显示进度
        canvas.setCursor(2, 60);
        canvas.fillRect(2, 60, 320, 20, 0x0000);
        canvas.printf("Packet #%3.d: %d bytes", packetCount, currentSize);
        canvas.setCursor(2, 80);
        canvas.fillRect(2, 80, 320, 20, 0x0000);
        canvas.printf("total:%d", totalSize);
        canvas.pushSprite(0, 0);

        Serial.printf("Progress: Packet #%d, current: %d bytes, total: %d bytes\n", packetCount, currentSize,
                      totalSize);
    } else {
        // 上传完成
        canvas.setCursor(2, 100);
        canvas.fillRect(2, 100, 320, 20, 0x0000);
        canvas.printf("Up ALL: %d bytes", totalSize);
        canvas.pushSprite(0, 0);

        Serial.println("Upload completed!");
    }
    return true;  // 继续上传
}

// 模板下载进度回调函数
bool templateDownloadProgressCallback(uint16_t currentSize, uint32_t totalSize, int packetCount, bool isComplete)
{
    // 进度回调函数
    if (!isComplete) {
        // 更新显示进度
        canvas.setCursor(2, 160);
        canvas.fillRect(2, 160, 320, 20, 0x0000);
        canvas.printf("Packet #%3.d: %d bytes", packetCount, currentSize);
        canvas.setCursor(2, 180);
        canvas.fillRect(2, 180, 320, 20, 0x0000);
        canvas.printf("total:%d", totalSize);
        canvas.pushSprite(0, 0);

        Serial.printf("Progress: Packet #%d, current: %d bytes, total: %d bytes\n", packetCount, currentSize,
                      totalSize);
    } else {
        // 下载完成
        canvas.setCursor(2, 200);
        canvas.fillRect(2, 200, 320, 20, 0x0000);
        canvas.printf("Down ALL: %d bytes", totalSize);
        canvas.pushSprite(0, 0);

        Serial.println("Download completed!");
    }
    return true;  // 继续下载
}
# Fingerprint2 demo 使用教程 / Fingerprint2 Demo User Guide

## 该demo只适用于M5Stack CoreS3/-SE/-Lite / This demo is only compatible with M5Stack CoreS3/-SE/-Lite

### 1. 启动后进入主界面 / Main Interface After Startup

![](image/c7966a249406f2c3787723ffaf52321e_5znPDWlM4M.jpg)

左上角：指纹录入 右上角：指纹验证 / Top-left: Fingerprint Enrollment | Top-right: Fingerprint Verification

左下角：模板上传下载 右下角：查看系统寄存器 / Bottom-left: Template Upload/Download | Bottom-right: View System Registers

在主界面长按指纹模块，将自动显示指纹图像 / Long press the fingerprint module on the main interface to automatically display the fingerprint image

![](image/405f90f810b9df23e6342eb817719da1_9fVbsB2DBR.jpg)

### 2. 录入指纹 / Fingerprint Enrollment

### 2.1 点击左上角打开指纹录入 / Click the top-left corner to open fingerprint enrollment

![](image/9ff5a42911243c068079683c7f63047c_59o0AbeBqO.jpg)

### 2.2 放下手指，第一次 / Place your finger, first time

![](image/93aa26424bf5a55ed2dfbac6dbdee0e7_yEaVi31FvO.jpg)

### 2.3 然后松开手指（程序可配置为不需要抬起手指） / Then lift your finger (program can be configured to not require lifting)

![](image/be1d9abec41647e5d3df1eeb99d8b07e__lAO0LxlcS.jpg)

### 2.4 再次放下手指 / Place your finger again

![](image/6ee0f4c5dbc73bf98ac481ee71d99a03_Vk7iwVAGkW.jpg)

### 2.5 然后再次抬起手指 / Then lift your finger again

![](image/a23df0ee89d0b4054aaf6433f2e19bcf_A0K0fLM58c.jpg)

### 2.6 再次放下手指（本程序设置为3次，程序可自定义） / Place your finger again (this program is set to 3 times, customizable)

![](image/f942f1ec1c5a96a2143a65b6c89793f1_5vdkAnFVzV.jpg)

显示Store ok即为录入成功，程序也通过次判断是否成功。 / Display "Store ok" indicates successful enrollment, the program also judges success through this.

## 3. 指纹验证 / Fingerprint Verification

### 3.1 点击右上角，打开指纹验证界面 / Click the top-right corner to open fingerprint verification interface

![](image/b51e0d202a2253f2643294babb40d7e6_hLvo787FLs.jpg)

### 3.2 放下手指，若失败 / Place your finger, if failed

![](image/2e1b1c49b966672851616c96ae84e51c_UAdZkIWGDK.jpg)

### 3.3 若成功，会显示存储的页号和比对分数 / If successful, the stored page number and match score will be displayed

![](image/bbdd61d68542809fba61ef4eccc2c2f1_2ZZBSIk8sU.jpg)

## 4. 模板上传下载 / Template Upload/Download

点击左下角，进入模板上传下载预览页面 / Click the bottom-left corner to enter the template upload/download preview page

![](image/922dd028d2d5b379ddd67eff9259795c_Cb_o5Nsqj_.jpg)

该页面主要演示模板的上传下载的数据包传输过程，具体实现可参考源码。 / This page mainly demonstrates the packet transmission process of template upload and download, please refer to the source code for specific implementation.

## 5. 模块内部的系统寄存器显示 / Module Internal System Register Display

点击右下角，显示内部的寄存器信息 / Click the bottom-right corner to display internal register information

![](image/24ae26b4eab9c8fd73872a0290dbc610_c6u3_bGYj5.jpg)

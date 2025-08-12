# Fingerprint2 demo 使用教程

## 该demo只适用于M5Stack CoreS3/-SE/-Lite

### 1.启动后进入主界面

![](image/c7966a249406f2c3787723ffaf52321e_5znPDWlM4M.jpg)

左上角：指纹录入 右上角：指纹验证

左下角：模板上传下载 右下角：查看系统寄存器

在主界面长按指纹模块，将自动显示指纹图像

![](image/405f90f810b9df23e6342eb817719da1_9fVbsB2DBR.jpg)

### 2.录入指纹

### 2.1点击左上角打开指纹录入

![](image/9ff5a42911243c068079683c7f63047c_59o0AbeBqO.jpg)

### 2.2放下手指，第一次

![](image/93aa26424bf5a55ed2dfbac6dbdee0e7_yEaVi31FvO.jpg)

### 2.3然后松开手指（程序可配置为不需要抬起手指）

![](image/be1d9abec41647e5d3df1eeb99d8b07e__lAO0LxlcS.jpg)

### 2.4再次放下手指

![](image/6ee0f4c5dbc73bf98ac481ee71d99a03_Vk7iwVAGkW.jpg)

### 2.5然后再次抬起手指

![](image/a23df0ee89d0b4054aaf6433f2e19bcf_A0K0fLM58c.jpg)

### 2.6再次放下手指（本程序设置为3次，程序可自定义）

![](image/f942f1ec1c5a96a2143a65b6c89793f1_5vdkAnFVzV.jpg)

显示Store ok即为录入成功，程序也通过次判断是否成功。

## 3指纹验证

### 3.1点击右上角，打开指纹验证界面

![](image/b51e0d202a2253f2643294babb40d7e6_hLvo787FLs.jpg)

### 3.2放下手指，若失败

![](image/2e1b1c49b966672851616c96ae84e51c_UAdZkIWGDK.jpg)

### 3.3若成功，会显示存储的页号和比对分数

![](image/bbdd61d68542809fba61ef4eccc2c2f1_2ZZBSIk8sU.jpg)

## 4模板上传下载

点击左下角，进入模板上传下载预览页面

![](image/922dd028d2d5b379ddd67eff9259795c_Cb_o5Nsqj_.jpg)

该页面主要演示模板的上传下载的数据包传输过程，具体实现可参考源码。

## 5模块内部的系统寄存器显示

点击右下角，显示内部的寄存器信息

![](image/24ae26b4eab9c8fd73872a0290dbc610_c6u3_bGYj5.jpg)

注：不能在使用m5unit-fingerprint2-library的情况下，调整数据包的大小，该库的一些操作没有对数据包大小进行自适应。 -截止 25/8/6

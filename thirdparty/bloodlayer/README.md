# bloodlayer（太原 testOpencv C API）

对接文档：`d:\太原\testOpencv\docs\移植说明.md`

| 文件 | 说明 |
|------|------|
| `include/bloodlayer_api.h` | C 接口头文件 |
| `lib/bloodlayer.lib` | MSVC 导入库 |
| `bin/bloodlayer.dll` | 识别库 |
| `bin/opencv_world3416.dll` | DLL 依赖（与宿主 OpenCV 453 并存） |
| `bloodlayer_stub.cpp` | 无正式库时的占位桩（当前工程已改为链正式库，不再编译） |

宿主测高入口：`dilag/testopcv.cpp` → `bloodlayer_detect_interface`（UTF-8 路径）。

正式库来源：`D:\太原\testOpencv\bin`（`bloodlayer.dll` / `bloodlayer.lib`）。

Visual Studio（`Coagulation.vcxproj`）：

- 链接 `thirdparty\bloodlayer\lib\bloodlayer.lib`
- 不要定义 `BLOODLAYER_STATIC`
- 构建后把 `bin` 下 DLL 拷到 `release\` / `debug\`

若直接跑其它输出目录，请手动复制 `bin` 下两个 DLL。

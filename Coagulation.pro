#-------------------------------------------------
#
# Project created by QtCreator 2019-12-06T11:20:49
#
#-------------------------------------------------

# ========== Qt 模块配置 ==========
QT += core gui
QT += concurrent
QT += serialport serialbus
QT += widgets printsupport
QT += multimedia
QT += multimediawidgets
QT += sql
QT += network
QT += opengl          # 将 opengl 单独列出，避免重复 core/gui
# QT += core gui opengl   # 已合并到上面，删除重复
# QT += printsupport      # 重复，已在第7行添加

# ========== 编译配置 ==========
CONFIG += c++14

# ========== 调试信息生成（MSVC Release）==========
msvc {
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O2
} else {
    # MinGW
    QMAKE_LFLAGS_RELEASE += -mthreads
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
}

# ========== 链接库 ==========
LIBS += -lDbgHelp

# ========== 编译器额外调试选项 ==========

# DEFINES += BLOOD_STATIC   # blood_analyzer 未接入工程，暂不启用

# ========== MSVC 特定设置 ==========
msvc {
    QMAKE_LFLAGS += /HEAP:"100000000"",4096"
}

# ========== 第三方库包含 ==========
include(./QsLog/QsLog.pri)

# ========== 编码设置 ==========
# 本机 MSVC 14.00.23026（2015 RTM）不支持 /utf-8、/source-charset
# 这些开关会被当成未知选项忽略，反而容易让 UTF-8 中文注释触发 C1071
# 如已安装 VS2015 Update 2+，可自行打开下面开关
# msvc {
#     QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8 /utf-8
# }

# ========== 系统库链接 ==========
LIBS += -lopengl32 -lglu32
LIBS += -lsetupapi -lcfgmgr32
LIBS += -lQt5Multimedia -lQt5MultimediaWidgets

# ========== Qt 版本判断 ==========
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# ========== 应用程序配置 ==========
RC_ICONS = Picture/logo.ico
TARGET = Coagulation
TEMPLATE = app

# ========== 预处理器定义 ==========
DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += QT_DEPRECATED_WARNINGS
# DEFINES += QT_MESSAGELOGCONTEXT   # 重复，已注释

# ========== 第三方库路径 ==========
# 使用工程相对路径，避免依赖本机 C:\qtproject 绝对路径
msvc: LIBS += $$PWD/libusb.lib
!msvc: LIBS += $$PWD/libusb.a

# MSVC 编码设置（备用，已注释）
# msvc {
#     QMAKE_CXXFLAGS += /utf-8
#     QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
# }



# 指定库文件目录，并链接 QZXing3 库
# $$PWD 代表项目文件所在目录
LIBS += -L$$PWD/lib -L$$PWD/includeZXing -L$$PWD/release -lQZXing3

# 使用生成二维码功能需要加这一句
DEFINES += ENABLE_ENCODER_GENERIC



# ========== 自定义 DLL 链接 ==========
# UsbCodeDispose：由 C:\USBdispose\UsbCodeDispose 用 MSVC2015 x64 编译，产物在 deps/UsbCodeDispose
# 勿使用 vsdll 下 MinGW 版 UsbCodeDispose.dll（会导致“无法定位程序输入点”）
INCLUDEPATH += $$PWD/deps/UsbCodeDispose/include
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/deps/UsbCodeDispose/lib/debug -lUsbCodeDispose
    USBCODEDISPOSE_DLL = $$PWD/deps/UsbCodeDispose/bin/debug/UsbCodeDispose.dll
} else {
    LIBS += -L$$PWD/deps/UsbCodeDispose/lib/release -lUsbCodeDispose
    USBCODEDISPOSE_DLL = $$PWD/deps/UsbCodeDispose/bin/release/UsbCodeDispose.dll
}
win32 {
    # 编译后把 DLL 拷到输出目录，避免跑起来又加载到错误版本
    QMAKE_POST_LINK += $$quote(cmd /c copy /y \"$$shell_path($$USBCODEDISPOSE_DLL)\" \"$$shell_path($$OUT_PWD)\\\") $$escape_expand(\\n\\t)
}
LIBS += -L$$PWD -L$$PWD/vsdll -lMachineTaskAssignment
# LIBS += -L$$PWD/vsdll -lTaskDispose#     # 未使用，已注释
# LIBS += -L$$PWD/vsdll -lFloorPlanofInterface  # 未使用，已注释

# ========== OpenCV 配置 ==========
INCLUDEPATH += \
    C:\opencv3.4.1\opencv\build\include\
    C:\opencv3.4.1\opencv\build\include\opencv2

# MSVC x64 使用官方预编译 world 库；MinGW 仍用 building 下的 .dll.a
msvc {
    CONFIG(debug, debug|release) {
        LIBS += -LC:\opencv3.4.1\opencv\build\x64\vc14\lib -lopencv_world453d
    } else {
        LIBS += -LC:\opencv3.4.1\opencv\build\x64\vc14\lib -lopencv_world453
    }
} else {
    LIBS += \
        C:\opencv3.4.1\opencv\building\lib\libopencv_calib3d453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_core453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_dnn453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_features2d453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_flann453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_gapi453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_highgui453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_imgcodecs453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_imgproc453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_ml453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_objdetect453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_photo453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_stitching453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_ts453.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_video453.dll.a \
        C:\opencv3.4.1\opencv\building\lib\libopencv_videoio453.dll.a
}

# ========== bloodlayer 血液分层识别库（太原 testOpencv C API）==========
INCLUDEPATH += $$PWD/thirdparty/bloodlayer/include
LIBS += -L$$PWD/thirdparty/bloodlayer/lib -lbloodlayer
win32 {
    CONFIG(debug, debug|release) {
        BLOODLAYER_OUT = $$OUT_PWD/debug
    } else {
        BLOODLAYER_OUT = $$OUT_PWD/release
    }
    BLOODLAYER_BIN = $$PWD/thirdparty/bloodlayer/bin
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $$shell_path($$BLOODLAYER_BIN/bloodlayer.dll) $$shell_path($$BLOODLAYER_OUT) $$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $$shell_path($$BLOODLAYER_BIN/opencv_world3416.dll) $$shell_path($$BLOODLAYER_OUT) $$escape_expand(\\n\\t))
}

LIBS += -lwinmm -ladvapi32

# ========== suoweidll 动态库链接 ==========
# 优先工程目录内的 import lib；兼容旧的旁路 build 目录
win32: LIBS += -L$$PWD -lsuoweidll
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/release/ -lsuoweidll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/debug/ -lsuoweidll

INCLUDEPATH += $$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/release
DEPENDPATH += $$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/release

# ========== 禁用严格字符串检查（解决 MSVC 编码警告）==========
win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

# ========== 源文件 ==========
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    QRoundProgressBar.cpp \
    patient.cpp \
    qualitycontrol.cpp \
    calibrate.cpp \
    graphplot.cpp \
    qcchartpage.cpp \
    qcstartaction.cpp \
    result.cpp \
    testing.cpp \
    delegate.cpp \
    aboutmachine.cpp \
    machinesetting.cpp \
    machinesetting_gripperair.cpp \
    json.cpp \
    analyticaljson.cpp \
    loginui.cpp \
    addusername.cpp \
    alarm.cpp \
    customplot.cpp \
    holegridwidget.cpp \
    plotwidget.cpp \
    crc/JQChecksum.cpp \
    cglobal.cpp \
    customcreatsql.cpp \
    customhighdata.cpp \
    inquire_sql_info.cpp \
    ringsmapprogressbar.cpp \
    test_module_class.cpp \
    custom_text_colors.cpp \
    height_data.cpp \
    monitor_traytest.cpp \
    configureprojectitem.cpp \
    ini_file.cpp \
    correct_data.cpp \
    modbus_customclass.cpp \
    testheight_module.cpp \
    replacethetesttubetray.cpp \
    workthreadreadtsakfile.cpp \
    usb_initconnect.cpp \
    usblistener.cpp \
    virtualkeyboard.cpp \
    progressbar.cpp \
    tubemappintpostable.cpp \
    sendusbcommand.cpp \
    sampledataprocess.cpp \
    pmessagebox.cpp \
    canceltaskconfigure.cpp \
    suoweiserialport.cpp \
    timeoutthread.cpp \
    qtimerthread.cpp \
    kettle.cpp \
    channelvaluepanel.cpp \
    displaychanneldata.cpp \
    machinereadrunnable.cpp \
    qcommboxdelegate.cpp \
    tubecontinuedoing.cpp \
    legacytaskcontinue.cpp \
    quiutils.cpp \
    instrumentcoordinatetable.cpp \
    genericfunctions.cpp \
    instrumentserialportmessage.cpp \
    thermometerwidget.cpp \
    circleprogress.cpp \
    displaylogstext.cpp \
    instrumentalarmprompt.cpp \
    equipmenttaskactive.cpp \
    thetestmoduleprotocol.cpp \
    maincontrolboardprotocol.cpp \
    passparameterinterface.cpp \
    mydatabase.cpp \
    functionclass.cpp \
    QSimpleLed.cpp \
    mycustomcurve.cpp \
    notifybarcode.cpp \
    opencvfindheigh.cpp \
    threadremindertsettube.cpp \
    transformeddata.cpp \
    loadequipmentpos.cpp \
    treezetablewidget.cpp \
    commandexceptional.cpp \
    testprojectprocess.cpp \
    structinstance.cpp \
    ccrashstack.cpp \
    timerthread.cpp \
    custombars.cpp \
    controlthemoduletemp.cpp \
    vWaterProgressBar.cpp \
    dilag/loginmaininterface.cpp \
    dilag/mylableborder.cpp \
    dilag/mybordercontainer.cpp \
    operclass/fullyautomatedplatelets.cpp \
    custom_style/animationprocessbar.cpp \
    dilag/handoff_staff.cpp \
    warn_interface.cpp \
    operclass/controldimming.cpp \
    dilag/connecttoinstrument.cpp \
    dilag/tipcustomwidget.cpp \
    FunctionWidget/functioncustomwidget.cpp \
    threadAddSample/mythreadaddsample.cpp \
    threadAddSample/consumableswrite.cpp \
    PrintReport/printthereport.cpp \
    custom_style/freezetablewidget.cpp \
    custom_style/widgetdelegate.cpp \
    custom_style/custombutton.cpp \
    operclass/performancetimer.cpp \
    modulethread/obtainmainboarddata.cpp \
    modulethread/moduletimerthread.cpp \
    operclass/ccreatedump.cpp \
    globaldata.cpp \
    InquireDataClass/querydatathread.cpp \
    custom_style/customprogresscontrols.cpp \
    dilag/testopcv.cpp \
    # dilag/bloodlayerdetector.cpp \  # 已改接 thirdparty/bloodlayer C API
    SqlOperator/sqloperator.cpp \
    dilag/batchaddsample.cpp \
    Custommovements/performanceverification.cpp \
    modulethread/testqualitycontrol.cpp \
    modulethread/structqualitycontrol.cpp \
    dilag/pedata.cpp \
    Machinewidget/bloodpinparaset.cpp \
    custom_style/frozentablewidget.cpp \
    dilag/customfixtableview.cpp \
#    custom_style/customtablemodel.cpp \
    SerialPortException.cpp \
    dilag/custompppvalue.cpp \
    CalibratCoordinateLoc/coordinatepposit.cpp \
    suoweiAggregationRateCalculator/aggregationratecalculator.cpp \
    suoweiFileManager/filemanager.cpp \
    customTitleui/framelesswidgethelper.cpp \
    suoweiFileManager/hospitalreportprinter.cpp \
    datalprocessor.cpp \
    verifycoordinates.cpp \
    styledcomparisondialog.cpp \
    camerawindow.cpp \
    opencvFindRBC/customtitlebar.cpp \
    CalibratCoordinateLoc/circlewidget.cpp \
    PrintReport/printtemplate.cpp \
    PrintReport/aggregationanalyzer.cpp \
    custom_style/custommessagebox.cpp \
    Custommovements/customChartWidget/medicalchartwidget.cpp \
    LIS/lisapiservice.cpp \
    pumppositionmanager.cpp \
    dilag/parameterdialog.cpp \
    LIS/curveimagegenerator.cpp \
    dilag/creatreagentcurve.cpp \
    qrcodereader.cpp
    # ========== LIS系统相关文件 ==========

# ========== 头文件 ==========
HEADERS += \
    mainwindow.h \
    qcustomplot.h \
    QRoundProgressBar.h \
    patient.h \
    qualitycontrol.h \
    calibrate.h \
    pairframetablewidget.h \
    graphplot.h \
    qcchartpage.h \
    qcstartaction.h \
    result.h \
    testing.h \
    delegate.h \
    aboutmachine.h \
    machinesetting.h \
    json.h \
    analyticaljson.h \
    loginui.h \
    addusername.h \
    alarm.h \
    customplot.h \
    holegridwidget.h \
    plotwidget.h \
    crc/JQChecksum.h \
    cglobal.h \
    customcreatsql.h \
    customhighdata.h \
    inquire_sql_info.h \
    custom_struct.h \
    ringsmapprogressbar.h \
    test_module_class.h \
    custom_text_colors.h \
    height_data.h \
    monitor_traytest.h \
    configureprojectitem.h \
    ini_file.h \
    correct_data.h \
    modbus_customclass.h \
    testheight_module.h \
    replacethetesttubetray.h \
    workthreadreadtsakfile.h \
    usb_initconnect.h \
    usblistener.h \
    virtualkeyboard.h \
    progressbar.h \
    tubemappintpostable.h \
    sendusbcommand.h \
    sampledataprocess.h \
    pmessagebox.h \
    canceltaskconfigure.h \
    suoweiserialport.h \
    timeoutthread.h \
    qtimerthread.h \
    kettle.h \
    channelvaluepanel.h \
    displaychanneldata.h \
    machinereadrunnable.h \
    qcommboxdelegate.h \
    tubecontinuedoing.h \
    legacytaskcontinue.h \
    quiutils.h \
    instrumentcoordinatetable.h \
    genericfunctions.h \
    instrumentserialportmessage.h \
    connectionutil.h \
    thermometerwidget.h \
    circleprogress.h \
    displaylogstext.h \
    instrumentalarmprompt.h \
    equipmenttaskactive.h \
    thetestmoduleprotocol.h \
    maincontrolboardprotocol.h \
    passparameterinterface.h \
    mydatabase.h \
    functionclass.h \
    QSimpleLed.h \
    mycustomcurve.h \
    notifybarcode.h \
    opencvfindheigh.h \
    threadremindertsettube.h \
    transformeddata.h \
    loadequipmentpos.h \
    treezetablewidget.h \
    commandexceptional.h \
    testprojectprocess.h \
    structinstance.h \
    ccrashstack.h \
    timerthread.h \
    custombars.h \
    controlthemoduletemp.h \
    vWaterProgressBar.h \
    dilag/loginmaininterface.h \
    dilag/mybordercontainer.h \
    operclass/fullyautomatedplatelets.h \
    custom_style/animationprocessbar.h \
    dilag/handoff_staff.h \
    warn_interface.h \
    operclass/controldimming.h \
    dilag/connecttoinstrument.h \
    dilag/tipcustomwidget.h \
    FunctionWidget/functioncustomwidget.h \
    threadAddSample/mythreadaddsample.h \
    threadAddSample/consumableswrite.h \
    PrintReport/printthereport.h \
    custom_style/freezetablewidget.h \
    custom_style/widgetdelegate.h \
    custom_style/custombutton.h \
    operclass/performancetimer.h \
    modulethread/obtainmainboarddata.h \
    modulethread/moduletimerthread.h \
    operclass/ccreatedump.h \
    globaldata.h \
    InquireDataClass/querydatathread.h \
    custom_style/customprogresscontrols.h \
    dilag/testopcv.h \
    # dilag/bloodlayerdetector.h \  # 已改接 thirdparty/bloodlayer C API
    thirdparty/bloodlayer/include/bloodlayer_api.h \
    SqlOperator/sqloperator.h \
    dilag/batchaddsample.h \
    Custommovements/performanceverification.h \
    modulethread/testqualitycontrol.h \
    modulethread/structqualitycontrol.h \
    dilag/pedata.h \
    Machinewidget/bloodpinparaset.h \
    custom_style/frozentablewidget.h \
    dilag/customfixtableview.h \
    #custom_style/customtablemodel.h \
    SerialPortException.h \
    dilag/custompppvalue.h \
    aligndelegate.h \
    CalibratCoordinateLoc/coordinatepposit.h \
    suoweiAggregationRateCalculator/aggregationratecalculator.h \
    suoweiFileManager/filemanager.h \
    customTitleui/framelesswidgethelper.h \
    suoweiFileManager/hospitalreportprinter.h \
    datalprocessor.h \
    verifycoordinates.h \
    styledcomparisondialog.h \
    camerawindow.h \
    opencvFindRBC/customtitlebar.h \
    CalibratCoordinateLoc/circlewidget.h \
    PrintReport/printtemplate.h \
    PrintReport/aggregationanalyzer.h \
    custom_style/custommessagebox.h \
    Custommovements/customChartWidget/medicalchartwidget.h \
    LIS/lisapiservice.h \
    pumppositionmanager.h \
    dilag/parameterdialog.h \
    LIS/curveimagegenerator.h \
    dilag/creatreagentcurve.h \
    qrcodereader.h
# ========== LIS系统相关文件 ==========

# ========== UI 文件 ==========
FORMS += \
    mainwindow.ui \
    patient.ui \
    qualitycontrol.ui \
    calibrate.ui \
    graphplot.ui \
    result.ui \
    testing.ui \
    aboutmachine.ui \
    machinesetting.ui \
    loginui.ui \
    addusername.ui \
    alarm.ui \
    customplot.ui \
    plotwidget.ui \
    customhighdata.ui \
    inquire_sql_info.ui \
    height_data.ui \
    configureprojectitem.ui \
    correct_data.ui \
    replacethetesttubetray.ui \
    virtualkeyboard.ui \
    pmessagebox.ui \
    canceltaskconfigure.ui \
    tubecontinuedoing.ui \
    legacytaskcontinue.ui \
    displaylogstext.ui \
    instrumentalarmprompt.ui \
    notifybarcode.ui \
    commandexceptional.ui \
    controlthemoduletemp.ui \
    dilag/loginmaininterface.ui \
    dilag/handoff_staff.ui \
    warn_interface.ui \
    dilag/connecttoinstrument.ui \
    dilag/tipcustomwidget.ui \
    FunctionWidget/functioncustomwidget.ui \
    custom_style/custombutton.ui \
    custom_style/customprogresscontrols.ui \
    dilag/testopcv.ui \
    # replacethetesttubetray.ui \   # 重复，已注释（第573行附近）
    dilag/batchaddsample.ui \
    dilag/pedata.ui \
    Machinewidget/bloodpinparaset.ui \
    dilag/customfixtableview.ui \
    dilag/custompppvalue.ui \
    CalibratCoordinateLoc/coordinatepposit.ui

# ========== 资源文件 ==========
RESOURCES += \
    picture.qrc

DISTFILES += \
    LIS/LIS系统对接文档.md

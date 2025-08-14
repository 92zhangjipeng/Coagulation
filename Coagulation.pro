#-------------------------------------------------
#
# Project created by QtCreator 2019-12-06T11:20:49
#
#-------------------------------------------------
QT      += core gui
QT      += concurrent
QT      += serialport serialbus
QT      += widgets printsupport
QT      += multimedia
QT      += multimediawidgets
QT      += sql
QT      += network
QT      += core gui opengl
QT      += printsupport

CONFIG  += c++14



QMAKE_LFLAGS_RELEASE += /MAP
QMAKE_CFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /debug /opt:ref

QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


QMAKE_CFLAGS_RELEASE += -g
QMAKE_CXXFLAGS_RELEASE += -g
#禁止优化
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_LFLAGS_RELEASE = -mthreads -W
LIBS += -lDbgHelp
#加上下面两行，否则用vs调试时，提示“未找到xxx.exe"
QMAKE_CXXFLAGS += -g
QMAKE_CFLAGS += -g

msvc {
    QMAKE_LFLAGS +=/HEAP:"100000000"",4096"
}


#链接DbgHelp库
#LIBS += -lDbgHelp
#Release版也将生成“.pdb”后缀的调试信息文件
#QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG

#QMAKE_LFLAGS += /ignore:4099

include(./QsLog/QsLog.pri)

QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8 /utf-8
LIBS    +=-lopengl32 -lglu32
LIBS    += -lsetupapi -lcfgmgr32

msvc:QMAKE_CXXFLAGS += -source-charset:utf-8
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = logo.ico

TARGET    = Coagulation
TEMPLATE  = app
DEFINES  += QT_MESSAGELOGCONTEXT

LIBS += "C:\qtproject\Coagulation\libusb.a"



#LIBS += -LC:\qtproject\Coagulation\vsdll\ -lTaskDispose#
#LIBS += -LC:\qtproject\Coagulation\vsdll\ -lFloorPlanofInterface
LIBS += -LC:\qtproject\Coagulation\vsdll\ -lUsbCodeDispose

LIBS += -LC:\qtproject\Coagulation\vsdll\ -lMachineTaskAssignment

INCLUDEPATH +=  C:\opencv3.4.1\opencv\build\include\
                C:\opencv3.4.1\opencv\build\include\opencv2
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_calib3d453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_core453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_dnn453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_features2d453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_flann453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_gapi453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_highgui453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_imgcodecs453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_imgproc453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_ml453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_objdetect453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_photo453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_stitching453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_ts453.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_video453.dll.a
LIBS +=C:\opencv3.4.1\opencv\building\lib\libopencv_videoio453.dll.a

LIBS += -lwinmm


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_MESSAGELOGCONTEXT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    dosometingthread.cpp \
    qcustomplot.cpp \
    QRoundProgressBar.cpp \
    patient.cpp \
    qualitycontrol.cpp \
    cuestomtabstyle.cpp \
    calibrate.cpp \
    graphplot.cpp \
    result.cpp \
    testing.cpp \
    delegate.cpp \
    aboutmachine.cpp \
    machinesetting.cpp \
    json.cpp \
    analyticaljson.cpp \
    loginui.cpp \
    addusername.cpp \
    alarm.cpp \
    customplot.cpp \
    plotwidget.cpp \
    crc/JQChecksum.cpp \
    cglobal.cpp \
    customcreatsql.cpp \
    customhighdata.cpp \
    infovirtual.cpp \
    threadinfovirtualbasic.cpp \
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
    singletoncase.cpp \
    opencvfindheigh.cpp \
    threadremindertsettube.cpp \
    transformeddata.cpp \
    loadequipmentpos.cpp \
    treezetablewidget.cpp \
    commandexceptional.cpp \
    doqualitycontrol.cpp \
    operregistry.cpp \
    testprojectprocess.cpp \
    structinstance.cpp \
    ccrashstack.cpp \
    timerthread.cpp \
    custombars.cpp \
    controlmodulfucn_.cpp \
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
    SqlOperator/sqloperator.cpp \
    dilag/batchaddsample.cpp \
    Custommovements/performanceverification.cpp \
    modulethread/testqualitycontrol.cpp \
    modulethread/structqualitycontrol.cpp \
    dilag/pedata.cpp \
    Machinewidget/bloodpinparaset.cpp \
    custom_style/frozentablewidget.cpp \
    dilag/customfixtableview.cpp \
    custom_style/customtablemodel.cpp \
    SerialPortException.cpp

HEADERS += \
    mainwindow.h \
    dosometingthread.h \
    qcustomplot.h \
    QRoundProgressBar.h \
    patient.h \
    qualitycontrol.h \
    cuestomtabstyle.h \
    calibrate.h \
    graphplot.h \
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
    plotwidget.h \
    crc/JQChecksum.h \
    crc/vld.h \
    crc/vld_def.h \
    cglobal.h \
    customcreatsql.h \
    customhighdata.h \
    infovirtual.h \
    threadinfovirtualbasic.h \
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
    crc/dummy.h \
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
    singletoncase.h \
    opencvfindheigh.h \
    threadremindertsettube.h \
    transformeddata.h \
    loadequipmentpos.h \
    treezetablewidget.h \
    commandexceptional.h \
    doqualitycontrol.h \
    operregistry.h \
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
    SqlOperator/sqloperator.h \
    dilag/batchaddsample.h \
    Custommovements/performanceverification.h \
    modulethread/testqualitycontrol.h \
    modulethread/structqualitycontrol.h \
    dilag/pedata.h \
    Machinewidget/bloodpinparaset.h \
    custom_style/frozentablewidget.h \
    dilag/customfixtableview.h \
    custom_style/customtablemodel.h \
    custom_style/animationprocessbar.h \
    SerialPortException.h

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
    replacethetesttubetray.ui \
    dilag/batchaddsample.ui \
    dilag/pedata.ui \
    Machinewidget/bloodpinparaset.ui \
    dilag/customfixtableview.ui

RESOURCES += \
    picture.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/release/ -lsuoweidll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/debug/ -lsuoweidll
else:unix: LIBS += -L$$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/ -lsuoweidll

INCLUDEPATH += $$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/release
DEPENDPATH += $$PWD/../build-suoweidll-Desktop_Qt_5_10_0_MSVC2015_64bit-Release/release



win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings



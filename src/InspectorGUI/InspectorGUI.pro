QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# --- 3. 声明我们的源文件和头文件 ---

HEADERS  += \
    Core/CameraManager.h \
    Widgets/ControlPanel/CameraPanel.h \
    Widgets/ControlPanel/InspectPanel.h \
    mainwindow.h \
    # 核心逻辑
    Core/InspectorThread.h \
    Core/LogManager.h \
    Core/ImageConverter.h \
    # 自定义控件
    Widgets/ViewWidget/ImageView.h \
    Widgets/ViewWidget/CustomGraphicView.h \
    Widgets/ViewWidget/CustomImageItem.h \
    Widgets/LogWidget/LogWidget.h

SOURCES  += \
    Core/CameraManager.cpp \
    Widgets/ControlPanel/CameraPanel.cpp \
    Widgets/ControlPanel/InspectPanel.cpp \
    main.cpp \
    mainwindow.cpp \
    # 核心逻辑
    Core/InspectorThread.cpp \
    Core/LogManager.cpp \
    # 自定义控件
    Widgets/ViewWidget/ImageView.cpp \
    Widgets/ViewWidget/CustomGraphicView.cpp \
    Widgets/ViewWidget/CustomImageItem.cpp \
    Widgets/LogWidget/LogWidget.cpp

# --- 4. 【关键】链接外部库 (OpenCV) ---
# 解释: 我们的GUI程序本身虽然不直接运行算法，但它内部的线程(我们稍后创建)
#       需要将Qt的QImage转换为OpenCV的cv::Mat来传递给DLL，所以GUI项目也必须链接OpenCV。
#       【请注意!】您必须将下面的路径替换为您自己电脑上OpenCV的实际安装路径！
#配置opencv库
INCLUDEPATH += D:/opencv/build/include
Debug: {
LIBS += -lD:/opencv/build/x64/vc16/lib/opencv_world4120d
    }
Release: {
LIBS += -lD:/opencv/build/x64/vc16/lib/opencv_world4120
    }

# --- 5. 【关键】链接我们自己的后端引擎 (InspectorLib) ---
# a. 告诉编译器去哪里找 "Inspector.h" (我们的“服务菜单”)
#    $$PWD 指的是当前 .pro 文件所在的目录 (src/InspectorGUI/)
#    /../InspectorLib 指的是返回上一级(到src/)，然后进入InspectorLib/目录
INCLUDEPATH += $$PWD/../InspectorLib

# b. 告诉链接器去哪里找 "InspectorLib.lib" (我们的“路标”)
#    $$PWD/../.. 指的是返回两级 (到项目根目录 PartInspectorProject/)
#    /bin/InspectorLib.lib 指的是在根目录下的bin文件夹中找到那个 .lib 文件
Debug: {
    # 在Debug模式下，去 bin/Debug 文件夹找库
    LIBS += -L$$PWD/../../bin/Debug -lInspectorLib
}
Release: {
    # 在Release模式下，去 bin/Release 文件夹找库
    LIBS += -L$$PWD/../../bin/Release -lInspectorLib
}

# --- 6. 【新增!】链接海康相机SDK ---
# 解释: 您必须将下面的路径替换为您电脑上海康MVS SDK的实际安装路径！
#       这只是一个常见的示例路径。

# a. 告诉编译器去哪里找SDK的头文件
INCLUDEPATH += $$PWD/../../depends/HIKCarema/Includes

# b. 告诉链接器去链接SDK的.lib文件 (MvCamCtrl.lib 等)
#    -L 后面是库文件所在的文件夹路径
#    -l 后面是要链接的库的名称 (去掉lib前缀和.lib后缀，qmake会自动处理)
#    【请根据您的系统是32位还是64位选择正确的路径】
LIBS += $$PWD/../../depends/HIKCarema/Libraries/MvCameraControl.lib

#配置生成路径，将我们的结果输出产物输出到bin文件夹内，方便管理
debug_and_release {
    # 对于 Debug 模式
    Debug:DESTDIR = $$PWD/../../bin/Debug
    # 对于 Release 模式
    Release:DESTDIR = $$PWD/../../bin/Release
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#设置资源文件
RESOURCES += \
    Resources/resources.qrc

#设置翻译文件
TRANSLATIONS += InspectorGUI_zh_CN.ts
#设置图标
RC_FILE = appicon.rc

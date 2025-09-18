#include "mainwindow.h"

// --- 1. 包含必要的Qt头文件和我们自己的主窗口头文件 ---
#include <QApplication>
#include <QStyleFactory>  // 包含 QStyleFactory，用于设置漂亮的界面风格
#include <QFile>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    // a. 创建 QApplication 实例
    //    这是任何Qt GUI程序的第一步，也是最重要的一步。
    //    它负责管理整个应用程序的事件循环和全局设置。
    QApplication a(argc, argv);
    // b. (推荐) 设置一个跨平台的统一视觉风格
    //    "fusion" 是Qt提供的一种现代、美观的风格，能让您的应用在Windows/macOS/Linux上看起来都一样。
    //    这比使用操作系统的默认老旧样式要好得多。
    // a.setStyle(QStyleFactory::create("fusion"));

    // --- 加载翻译文件 ---
    QTranslator translator;
    // 我们假设您已经把 .qm 文件添加到了资源文件 .qrc 中，并放在了 Translations 目录下
    if (translator.load(":/translation/InspectorGUI_zh_CN.qm")) {
        a.installTranslator(&translator);
    } else {
        // 如果加载失败，在控制台打印一个警告，方便调试
        qWarning("Could not load translation file ':/Translations/InspectorGUI_zh_CN.qm'");
    }



    QFile styleFile(":/Styles/ModernDark.qss"); // 通过资源路径":/"访问qss文件
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        // 读取文件全部内容
        QString styleSheet = QLatin1String(styleFile.readAll());
        // 为整个应用程序设置样式
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }
    else
    {
        // 如果样式表加载失败，在控制台打印一个警告
        qWarning("Could not load stylesheet. Using default style.");
    }


    // c. 创建并显示我们的主窗口
    //    MainWindow w; 这行代码会创建 MainWindow 类的一个实例。
    //    在创建过程中，MainWindow的构造函数会被调用，所有UI的初始化工作都将在那里完成。
    MainWindow w;
    w.show(); // 调用 show() 函数，将窗口显示在屏幕上。

    // d. 启动Qt的事件循环
    //    a.exec() 是程序的心脏。程序会在这里进入一个无限循环，
    //    等待并处理用户的各种操作（点击鼠标、按键盘、改变窗口大小等）。
    //    只有当用户关闭主窗口时，这个循环才会结束，程序才会真正退出。
    return a.exec();
}

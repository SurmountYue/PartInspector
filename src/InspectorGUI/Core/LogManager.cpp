// src/InspectorGUI/Core/LogManager.cpp

#include "LogManager.h"
#include <QDateTime> // 用于获取当前系统时间
#include <QMutex>    // 用于实现线程安全的单例创建
#include <QMutexLocker> // 一个方便的RAII类，用于自动加锁和解锁
#include <QTextStream>  // 用于方便地构建格式化的字符串

// --- 1. 初始化静态成员变量 ---
// 在类定义的外部，对静态成员变量进行唯一的定义和初始化。
// 程序启动时，这个指针被设为 nullptr，表示单例实例尚未被创建。
LogManager* LogManager::m_instance = nullptr;

// --- 2. 定义全局的、自定义的消息处理器 ---

/**
 * @brief 一个全局函数，用作Qt日志系统的自定义消息处理器。
 * @param type 消息的级别 (Debug, Info, Warning, Critical, Fatal)。
 * @param context 消息的上下文信息（文件名、行号等，我们这里未使用）。
 * @param msg 原始的日志消息字符串（即 qDebug() << "..." 中的 "..." 部分）。
 * * @details 这个函数是整个日志重定向的核心。它的函数签名是Qt固定的。
 * qInstallMessageHandler 之后，所有Qt日志都会被导向这里。
 */
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Q_UNUSED() 是一个Qt宏，用来告诉编译器我们“故意”没有使用某个参数，
    // 从而避免编译器产生“未使用参数”的警告。
    Q_UNUSED(context);

    QString formattedMessage; // 创建一个空字符串来构建完整的日志条目
    QTextStream stream(&formattedMessage); // 使用 QTextStream 来方便地写入

    // a. 添加时间戳，格式为 "年-月-日 时:分:秒.毫秒"
    stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << " ";

    // b. 根据消息级别，添加一个可读的文本标签
    switch (type) {
    case QtDebugMsg:    stream << "[DEBUG]   "; break;
    case QtInfoMsg:     stream << "[INFO]    "; break;
    case QtWarningMsg:  stream << "[WARNING] "; break;
    case QtCriticalMsg: stream << "[CRITICAL]"; break;
    case QtFatalMsg:    stream << "[FATAL]   "; break;
    }

    // c. 添加原始的日志消息内容
    stream << msg;

    // d. 【关键】通过LogManager单例，将格式化好的消息广播出去。
    //    这里不直接与任何UI控件交互，而是通过发射信号来解耦。
    emit LogManager::Instance()->newMessage(formattedMessage);
}


// --- 3. LogManager 类的成员函数实现 ---

// 私有构造函数的实现（函数体为空即可）
LogManager::LogManager(QObject *parent)
    : QObject(parent)
{
}

// 获取单例实例的函数
LogManager* LogManager::Instance()
{
    // 使用“双重检查锁定 (Double-Checked Locking)”模式来确保线程安全。
    if (m_instance == nullptr) // 第一次检查，在没有锁的情况下进行，可以避免每次调用都进入加锁流程，提高性能。
    {
        // 只有当实例不存在时，才进入这个可能耗时的加锁步骤。
        static QMutex mutex; // 静态互斥锁，确保在多次调用中是同一个锁。
        QMutexLocker locker(&mutex); // 创建一个锁的守护对象，它在构造时自动加锁，析构时（离开作用域）自动解锁。

        if (m_instance == nullptr) // 第二次检查，在持有锁的情况下进行。
        {
            // 这是为了防止一种情况：线程A通过了第一次检查，但在等待锁时，
            // 线程B已经抢先进入并创建了实例。如果没有第二次检查，线程A会再次创建一个实例，破坏单例。
            m_instance = new LogManager();
        }
    }
    return m_instance; // 返回唯一的实例
}

// 安装自定义消息处理器的函数
void LogManager::install()
{
    // 调用Qt的全局函数，将我们的 customMessageHandler 设置为新的消息处理器。
    qInstallMessageHandler(customMessageHandler);
}

// 卸载自定义消息处理器的函数
void LogManager::uninstall()
{
    // 传递 nullptr 给 qInstallMessageHandler 可以恢复Qt的默认处理器行为。
    qInstallMessageHandler(nullptr);
}

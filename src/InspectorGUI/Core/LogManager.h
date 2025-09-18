// src/InspectorGUI/Core/LogManager.h

#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QDebug> // 包含QDebug以支持Qt的日志系统

/**
 * @class LogManager
 * @brief 一个采用单例模式设计的全局日志管理器。
 *
 * @details 这个类的核心职责是：
 * 1. 确保在整个应用程序中只有一个日志管理实例存在（单例模式）。
 * 2. 提供一个全局访问点 `Instance()` 来获取这个唯一的实例。
 * 3. 提供 `install()` 方法来“劫持”Qt默认的日志输出流（如qDebug, qWarning等）。
 * 4. 将所有被劫持的日志消息，格式化后，通过 `newMessage` 信号发射出去。
 *
 * 这种设计的目的是将“日志的产生”与“日志的消费（如显示在UI上或写入文件）”完全解耦。
 */
class LogManager : public QObject
{
    Q_OBJECT // 启用元对象系统，这是使用信号与槽的前提

public:
    /**
     * @brief 获取 LogManager 的全局唯一实例。
     *
     * @details 这是访问单例对象的唯一公共方法。
     * 它采用了线程安全的懒汉式加载，只在第一次被调用时创建实例。
     * @return 返回指向 LogManager 唯一实例的指针。
     */
    static LogManager* Instance();

    /**
     * @brief 安装自定义日志处理器。
     *
     * @details 调用此函数后，所有通过 qDebug(), qWarning(), qInfo() 等产生的日志，
     * 都将被重定向到我们自定义的处理器中，而不是打印到控制台。
     * 通常在 main() 函数的开头调用一次。
     */
    void install();

    /**
     * @brief 卸载自定义日志处理器。
     *
     * @details 调用此函数后，Qt的日志系统将恢复其默认行为（通常是打印到控制台）。
     * 在应用程序退出前调用是一个好习惯，但非必需。
     */
    void uninstall();

signals:
    /**
     * @brief 当有新的日志消息被处理时，会发出此信号。
     * @param message 经过完整格式化（包含时间戳、级别、内容）的日志字符串。
     *
     * @details 任何关心日志消息的模块（如LogWidget）都可以连接到这个信号。
     */
    void newMessage(const QString& message);

private:
    // --- 单例模式的核心实现 ---

    /**
     * @brief 私有构造函数。
     * @param parent 父对象，遵循Qt的父子关系内存管理。
     * @details 将构造函数声明为私有，可以从语法上阻止任何外部代码
     * 通过 `new LogManager()` 的方式创建新的实例，从而保证了实例的唯一性。
     */
    explicit LogManager(QObject *parent = nullptr);

    /**
     * @brief 禁用拷贝构造函数。
     * @details `= delete` 是 C++11 的语法，明确禁止了通过拷贝来创建新的实例，
     * 例如 `LogManager anotherManager = *LogManager::Instance();` 将会导致编译错误。
     * 这是保证单例唯一性的重要补充。
     */
    LogManager(const LogManager&) = delete;

    /**
     * @brief 禁用赋值运算符。
     * @details 同样，`= delete` 也禁止了通过赋值来复制实例，
     * 例如 `*anotherManager = *LogManager::Instance();` 将会导致编译错误。
     */
    LogManager& operator=(const LogManager&) = delete;

    /**
     * @brief 指向全局唯一实例的静态指针。
     * @details `static` 关键字确保了这个指针在所有 LogManager 对象（虽然只有一个）
     * 之间是共享的，并且它的生命周期贯穿整个程序。
     * 它在 .cpp 文件中被初始化为 nullptr。
     */
    static LogManager* m_instance;
};

#endif // LOGMANAGER_H

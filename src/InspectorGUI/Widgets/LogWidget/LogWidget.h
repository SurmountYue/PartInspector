// src/InspectorGUI/Widgets/LogWidget/LogWidget.h

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

// --- 前向声明 ---
// 解释: 我们将使用这两个Qt控件的指针。在.h文件中使用前向声明是一种好习惯。
class QPlainTextEdit;
class QPushButton;

class LogWidget : public QWidget
{
    Q_OBJECT // 启用信号与槽机制

public:
    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget();

public slots:
    /**
     * @brief 一个公开的槽函数，用于从外部向日志窗口追加一条新消息。
     * @param message 要显示的日志文本。
     * @param level (可选) 可以根据消息级别改变文本颜色。
     */
    void appendMessage(const QString& message);

private slots:
    /**
     * @brief 当点击“清空日志”按钮时，此内部槽函数将被调用。
     */
    void onClearButtonClicked();

private:
    /**
     * @brief 辅助函数，用于创建和布局所有UI控件。
     */
    void setupUi();

    // --- 成员变量 ---
    QPlainTextEdit* m_logOutput; // 用于显示日志的文本区域
    QPushButton* m_clearButton;  // “清空日志”按钮
};

#endif // LOGWIDGET_H

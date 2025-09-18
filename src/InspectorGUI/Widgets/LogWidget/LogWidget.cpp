#include "LogWidget.h"

// --- 包含我们需要的Qt控件和布局的头文件 ---
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout> // 垂直布局
#include <QHBoxLayout> // 水平布局

// --- 构造函数 ---
LogWidget::LogWidget(QWidget *parent)
    : QWidget(parent)
{
    // 调用我们自己定义的辅助函数来完成所有UI的创建和设置
    setupUi();
}

// --- 析构函数 ---
LogWidget::~LogWidget()
{
    // 因为所有子控件在创建时都设置了 this 作为parent，
    // 所以这里不需要手动 delete 任何东西，Qt会自动清理。
}

// --- 公共槽函数：追加消息 ---
void LogWidget::appendMessage(const QString& message)
{
    if (m_logOutput) {
        // 调用 QPlainTextEdit 的 appendPlainText 方法来在末尾追加一行新文本。
        m_logOutput->appendPlainText(message);
    }
}

// --- 私有槽函数：响应清空按钮点击 ---
void LogWidget::onClearButtonClicked()
{
    if (m_logOutput) {
        // 调用 QPlainTextEdit 的 clear 方法来清空所有文本。
        m_logOutput->clear();
    }
}

// --- 辅助函数：创建和布局UI ---
void LogWidget::setupUi()
{
    // --- 1. 创建控件实例 ---
    // a. 创建文本显示区域
    m_logOutput = new QPlainTextEdit(this);
    m_logOutput->setReadOnly(true); // 设置为只读，用户不能手动编辑日志

    // b. 创建清空按钮
    m_clearButton = new QPushButton(tr("Clear Log"), this);


    // --- 2. 创建布局 ---
    // a. 将按钮放在一个水平布局中，并让它靠右对齐
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // 添加一个“弹簧”，会把右边的控件推到最右边
    buttonLayout->addWidget(m_clearButton);

    // b. 创建一个主垂直布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this); // `this` 将布局设置给当前Widget
    mainLayout->setContentsMargins(0, 0, 0, 0); // 设置边距为0
    mainLayout->setSpacing(5); // 设置控件之间的间距为5像素

    // c. 将文本区域和按钮布局添加到主布局中
    mainLayout->addWidget(m_logOutput);   // 文本区域在上方，会占据大部分空间
    mainLayout->addLayout(buttonLayout); // 按钮布局在下方


    // --- 3. 连接信号与槽 ---
    // 将“清空”按钮的 clicked() 信号，连接到我们自己的 onClearButtonClicked() 槽函数上。
    connect(m_clearButton, &QPushButton::clicked, this, &LogWidget::onClearButtonClicked);
}

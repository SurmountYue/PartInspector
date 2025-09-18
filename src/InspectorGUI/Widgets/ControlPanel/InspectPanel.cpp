// src/InspectorGUI/Widgets/ControlPanel/InspectPanel.cpp

#include "InspectPanel.h"

// --- 包含我们需要的Qt控件和布局的完整头文件 ---
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout> // 垂直布局管理器
#include <QHBoxLayout> // 水平布局管理器
#include <QIcon>       // 用于处理图标
#include <QString>     // Qt的字符串类
#include <QTextStream> // 用于方便地构建字符串

// --- 构造函数 ---
InspectPanel::InspectPanel(QWidget *parent)
    : QWidget(parent) // 调用父类的构造函数
{
    // 调用我们自己定义的辅助函数来完成所有UI的创建、布局和连接
    setupUi();
}

// --- 析构函数 ---
InspectPanel::~InspectPanel()
{
    // 函数体为空。
    // 因为所有 new 出来的子控件 (m_loadImageButton 等) 在创建时都将 `this` (InspectPanel实例)
    // 设置为了它们的父对象(parent)。根据Qt的父子关系自动内存管理机制，
    // 当 InspectPanel 对象被销毁时，它的所有子控件都会被自动 `delete`，无需我们手动处理。
}

// --- 公共接口函数实现 ---

void InspectPanel::setInspectButtonEnabled(bool enabled)
{
    if (m_inspectButton) {
        // 直接调用内部按钮控件 QPushButton 的 setEnabled 公共函数
        m_inspectButton->setEnabled(enabled);
    }
}

void InspectPanel::clearResults()
{
    if (m_resultsText) {
        // 直接调用内部文本框控件 QTextEdit 的 clear 公共函数
        m_resultsText->clear();
    }
}

// --- 公共槽函数实现 ---
void InspectPanel::displayResults(const InspectorLib::MeasurementResults& results)
{
    // 这个函数负责将后台线程计算出的、结构化的 MeasurementResults 数据，
    // 转换成人类可读的字符串，并显示在文本框中。

    // 1. 清空上一次的文本，确保显示的是最新的结果。
    m_resultsText->clear();

    // 2. 使用 QTextStream 来方便地构建一个长字符串，比反复用 `+` 拼接更高效。
    QString resultString;
    QTextStream stream(&resultString);
    stream.setRealNumberPrecision(3); // 设置浮点数显示的小数点后精度为3位

    // 3. 格式化并逐一写入每一项测量数据
    stream << "===== Inspection Results =====\n\n";

    stream << "[Outer Bounding Box]:\n";
    stream << "\t- Center: (" << results.boundingBox.center.x << ", " << results.boundingBox.center.y << ")\n";
    stream << "\t- Size: " << results.boundingBox.size.width << " x " << results.boundingBox.size.height << "\n";
    stream << "\t- Angle: " << results.boundingBox.angle << " deg\n\n";

    stream << "[Small Circles Found]: " << results.circles.size() << "\n";
    for (size_t i = 0; i < results.circles.size(); ++i)
    {
        stream << "  - Circle " << i << ":\n";
        stream << "\t- Center: (" << results.circles[i].center.x << ", " << results.circles[i].center.y << ")\n";
        stream << "\t- Radius: " << results.circles[i].radius << "\n";
    }

    stream << "\n[Slot Found]:\n";
    stream << "\t- Center: (" << results.slot.center.x << ", " << results.slot.center.y << ")\n";
    stream << "\t- Length: " << results.slot.length << "\n";
    stream << "\t- Width: " << results.slot.width << " (Arc Radius: " << results.slot.width / 2.0 << ")\n";
    stream << "\t- Angle: " << results.slot.angle << " deg\n";

    // 4. 将最终构建好的完整字符串，一次性设置到文本框中。
    m_resultsText->setText(resultString);
}

// --- 私有辅助函数实现 ---
void InspectPanel::setupUi()
{
    // --- 1. 创建所有UI控件的实例 ---
    // `this` 参数将当前 InspectPanel 对象设置为这些控件的父对象。
    m_loadImageButton = new QPushButton(tr("Load Image"), this);
    m_inspectButton = new QPushButton(tr("Inspect"), this);
    m_resultsText = new QTextEdit(this);

    // 从资源系统(:/)加载图标并设置给按钮
    m_loadImageButton->setIcon(QIcon(":/Icons/load_image.png"));
    m_inspectButton->setIcon(QIcon(":/Icons/inspect.png"));

    // --- 2. 初始化控件状态 ---
    m_inspectButton->setEnabled(false); // 初始时，“执行测量”按钮是禁用的，因为还没有加载图片。
    m_resultsText->setReadOnly(true);   // 将结果文本框设为只读，防止用户误修改。

    // --- 3. 使用布局管理器来组织控件 ---
    // a. 将两个按钮放在一个水平布局(QHBoxLayout)中
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_loadImageButton);
    buttonLayout->addWidget(m_inspectButton);

    // b. 创建一个主垂直布局(QVBoxLayout)，它将成为本控件的顶级布局
    m_mainLayout = new QVBoxLayout(this); // `this` 参数会自动将布局应用到 InspectPanel 上
    m_mainLayout->setContentsMargins(10, 10, 10, 10); // 设置布局内边距
    m_mainLayout->setSpacing(10);                     // 设置布局内控件之间的间距

    // c. 将按钮布局和文本框按顺序添加到主垂直布局中
    m_mainLayout->addLayout(buttonLayout); // 按钮布局在最上方
    m_mainLayout->addWidget(m_resultsText);  // 文本框在下方，它会自动伸展以占据剩余空间

    // --- 4. 连接内部的信号与槽 ---
    // 解释: 我们将按钮的 clicked() 信号，连接到本类(this)自己定义的信号上。
    //       这是一种“信号转发”的设计模式。当按钮被点击，它会触发我们自己的信号发射出去，
    //       从而向外界（MainWindow）报告用户的操作请求。
    connect(m_loadImageButton, &QPushButton::clicked, this, &InspectPanel::loadImageRequested);
    connect(m_inspectButton, &QPushButton::clicked, this, &InspectPanel::inspectRequested);
}

// src/InspectorGUI/Widgets/ControlPanel/CameraPanel.cpp

#include "CameraPanel.h"

// --- 包含我们需要的所有Qt控件和布局的头文件 ---
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>     // 垂直布局
#include <QHBoxLayout>     // 水平布局
#include <QFormLayout>    // 表单布局，用于标签和输入框的对齐

// --- 构造函数 ---
CameraPanel::CameraPanel(QWidget *parent)
    : QWidget(parent)
{
    // 调用辅助函数来完成所有UI的创建、布局和连接
    setupUi();
    setupConnections();

    // 初始化UI状态
    setUiForConnectionStatus(false);
}

// --- 析构函数 ---
CameraPanel::~CameraPanel()
{
    // 所有子控件都设置了parent，Qt会自动处理内存释放，无需代码。
}


// --- 公共接口函数实现 ---

void CameraPanel::updateDeviceList(const QStringList& deviceList)
{
    // 在更新列表前，先断开之前的连接，防止旧的索引导致信号混乱
    disconnect(m_deviceComboBox, &QComboBox::currentIndexChanged, this, &CameraPanel::connectDeviceRequested);

    m_deviceComboBox->clear();       // 清空下拉列表
    m_deviceComboBox->addItems(deviceList); // 添加新的设备列表项

    // 重新连接信号，这样用户选择新的设备时才会发出请求
    connect(m_deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CameraPanel::connectDeviceRequested);
}

void CameraPanel::setUiForConnectionStatus(bool connected)
{
    // 根据连接状态，设置控件的启用/禁用
    m_deviceComboBox->setEnabled(!connected);
    m_searchButton->setEnabled(!connected);
    m_connectButton->setText(connected ? "Disconnect" : "Connect"); // 改变按钮文字

    m_singleShotButton->setEnabled(connected);
    m_continuousShotButton->setEnabled(connected);
    m_parameterGroupBox->setEnabled(connected);

    if (!connected) {
        m_statusLabel->setText("Status: Disconnected");
        m_continuousShotButton->setChecked(false); // 断开连接时，确保连续采集按钮弹起
    } else {
        m_statusLabel->setText("Status: Connected");
    }
}


// --- 辅助函数：创建和布局UI ---
void CameraPanel::setupUi()
{
    // --- 1. 创建顶层布局 ---
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // --- 2. 创建“设备连接”分组 ---
    m_deviceGroupBox = new QGroupBox("Device Connection", this);

    m_deviceComboBox = new QComboBox(this);
    m_searchButton = new QPushButton("Search", this);
    m_connectButton = new QPushButton("Connect", this);
    m_statusLabel = new QLabel("Status: No Device", this);

    QHBoxLayout* deviceTopLayout = new QHBoxLayout();
    deviceTopLayout->addWidget(m_deviceComboBox, 2); // 第2个参数是拉伸因子，让ComboBox更宽
    deviceTopLayout->addWidget(m_searchButton, 1);

    QHBoxLayout* deviceBottomLayout = new QHBoxLayout();
    deviceBottomLayout->addWidget(m_statusLabel);
    deviceBottomLayout->addStretch();
    deviceBottomLayout->addWidget(m_connectButton);

    QVBoxLayout* deviceGroupLayout = new QVBoxLayout();
    deviceGroupLayout->addLayout(deviceTopLayout);
    deviceGroupLayout->addLayout(deviceBottomLayout);
    m_deviceGroupBox->setLayout(deviceGroupLayout);


    // --- 3. 创建“采集控制”分组 ---
    m_acquisitionGroupBox = new QGroupBox("Acquisition Control", this);
    m_singleShotButton = new QPushButton("Single Shot", this);
    m_continuousShotButton = new QPushButton("Continuous Shot", this);
    m_continuousShotButton->setCheckable(true); // 将此按钮设置为“可切换”状态（像开关一样）

    QHBoxLayout* acquisitionLayout = new QHBoxLayout();
    acquisitionLayout->addStretch();
    acquisitionLayout->addWidget(m_singleShotButton);
    acquisitionLayout->addWidget(m_continuousShotButton);
    m_acquisitionGroupBox->setLayout(acquisitionLayout);


    // --- 4. 创建“参数设置”分组 ---
    m_parameterGroupBox = new QGroupBox("Parameters", this);
    m_exposureSpinBox = new QSpinBox(this);
    m_exposureSpinBox->setRange(10, 100000); // 设置曝光范围 10μs - 100ms
    m_exposureSpinBox->setSuffix(" us");     // 设置单位后缀

    m_gainSpinBox = new QDoubleSpinBox(this);
    m_gainSpinBox->setRange(0.0, 20.0);      // 设置增益范围 0 - 20dB
    m_gainSpinBox->setSingleStep(0.1);       // 设置步长
    m_gainSpinBox->setSuffix(" dB");         // 设置单位后缀

    // 使用QFormLayout可以完美地将标签和输入框对齐
    QFormLayout* parameterLayout = new QFormLayout();
    parameterLayout->addRow("Exposure:", m_exposureSpinBox);
    parameterLayout->addRow("Gain:", m_gainSpinBox);
    m_parameterGroupBox->setLayout(parameterLayout);


    // --- 5. 将所有分组框添加到主布局中 ---
    m_mainLayout->addWidget(m_deviceGroupBox);
    m_mainLayout->addWidget(m_acquisitionGroupBox);
    m_mainLayout->addWidget(m_parameterGroupBox);
    m_mainLayout->addStretch(); // 添加一个弹簧，将所有内容推到顶部
}


// --- 辅助函数：连接内部信号与槽 ---
void CameraPanel::setupConnections()
{
    // --- a. 转发“请求”信号 ---
    // 将内部按钮的点击信号，连接到我们自己定义的、要发射出去的信号上。
    connect(m_searchButton, &QPushButton::clicked, this, &CameraPanel::searchDevicesRequested);
    connect(m_singleShotButton, &QPushButton::clicked, this, &CameraPanel::singleShotRequested);
    connect(m_continuousShotButton, &QPushButton::toggled, this, &CameraPanel::continuousShotToggled);

    // 对于“连接”按钮，逻辑稍微复杂
    connect(m_connectButton, &QPushButton::clicked, this, [this](){
        // 使用Lambda函数来根据按钮文字判断是连接还是断开
        if (m_connectButton->text() == "Connect") {
            emit connectDeviceRequested(m_deviceComboBox->currentIndex());
        } else {
            emit disconnectDeviceRequested();
        }
    });

    // --- b. 连接参数调节的信号与槽 ---
    // 将SpinBox的值变化信号，直接连接到我们自己的信号上
    connect(m_exposureSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CameraPanel::exposureChanged);
    connect(m_gainSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CameraPanel::gainChanged);
}

// src/InspectorGUI/mainwindow.cpp

#include "mainwindow.h"

// --- 包含我们创建的所有模块的完整头文件 ---
// 解释: 在.cpp文件中，因为我们需要创建这些类的实例并调用它们的成员函数，
//       所以必须包含它们完整的.h头文件，而不能只用前向声明。
#include "Widgets/ViewWidget/ImageView.h"
#include "Widgets/LogWidget/LogWidget.h"
#include "Widgets/ControlPanel/InspectPanel.h"
#include "Widgets/ControlPanel/CameraPanel.h"
#include "Core/InspectorThread.h"
#include "Core/CameraManager.h"
#include "Core/LogManager.h"
#include "Core/ImageConverter.h"

// --- 包含所有需要的Qt类 ---
#include <QVBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar> // 用于在窗口底部显示状态信息

// --- 构造函数 ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) // 调用父类构造函数
{
    // 调用辅助函数，分步完成初始化，让构造函数保持整洁
    setupUi();          // 1. 创建和布局所有UI控件
    setupConnections(); // 2. 连接所有模块的信号和槽

    // 设置窗口的初始大小和标题
    resize(1280, 800);
    setWindowTitle(tr("Part Inspector Pro"));

    // 【关键】安装并启动我们的全局日志系统
    LogManager::Instance()->install();

    // 发出第一条日志，这条消息会被LogManager捕获，并通过信号发送给LogWidget显示
    qInfo("Application started successfully.");
}

// --- 析构函数 ---
MainWindow::~MainWindow()
{
    // 在程序退出前，卸载我们的日志处理器，恢复Qt默认行为。这是一个好习惯。
    LogManager::Instance()->uninstall();
    // 所有在setupUi中创建的、以this为parent的子控件和后台对象，
    // 都会在这里被Qt的父子关系系统自动、安全地delete，无需我们手动操作。
}


// =====================================================================
//  辅助函数 (Initialization Helpers)
// =====================================================================

void MainWindow::setupUi()
{
    // --- 1. 创建所有自定义模块的实例 ---
    //    我们将 this 指针作为parent传递，建立父子关系，实现自动内存管理。
    m_cameraPanel = new CameraPanel(this);
    m_inspectPanel = new InspectPanel(this);
    m_logWidget = new LogWidget(this);
    m_mainImageView = new ImageView(this);
    m_resultImageView = new ImageView(this);
    m_cameraManager = new CameraManager(this);     // 创建相机管理器
    m_inspectorThread = new InspectorThread(this); // 创建后台测量线程

    // --- 2. 使用 QSplitter 组合出灵活的、可拖拽的布局 ---

    // a. 组合左侧面板 (相机面板、检测面板、日志窗口，三者上下排列)
    m_leftSplitter = new QSplitter(Qt::Vertical, this);
    m_leftSplitter->addWidget(m_cameraPanel);
    m_leftSplitter->addWidget(m_inspectPanel);
    m_leftSplitter->addWidget(m_logWidget);
    m_leftSplitter->setStretchFactor(0, 0); // 设置初始拉伸因子，让相机和检测面板优先保持其建议大小
    m_leftSplitter->setStretchFactor(1, 0);
    m_leftSplitter->setStretchFactor(2, 1); // 让日志窗口在垂直方向上占据更多剩余空间

    // b. 组合右侧显示区 (主视图和结果视图，上下排列)
    m_rightSplitter = new QSplitter(Qt::Vertical, this);
    m_rightSplitter->addWidget(m_mainImageView);
    m_rightSplitter->addWidget(m_resultImageView);

    // c. 最终组合主窗口 (左侧面板和右侧显示区，左右排列)
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->addWidget(m_leftSplitter);
    m_mainSplitter->addWidget(m_rightSplitter);
    m_mainSplitter->setStretchFactor(0, 0); // 让左侧控制面板保持其建议宽度
    m_mainSplitter->setStretchFactor(1, 1); // 让右侧图像显示区占据所有剩余的水平空间

    // --- 3. 将最终的布局设置为主窗口的中心控件 ---
    this->setCentralWidget(m_mainSplitter);

    // --- 4. 创建一个状态栏，用于在窗口底部显示临时信息 ---
    this->statusBar()->showMessage(tr("Ready. Please connect a camera or load an image."));
}

void MainWindow::setupConnections()
{
    // --- 【“电路”连接中心】 ---
    // 解释: 这是整个应用程序的“神经中枢”。我们在这里将所有独立的模块连接起来。

    // 连接1: 全局日志系统
    // 将 LogManager(广播室) 的 newMessage 信号，连接到 LogWidget(公告屏) 的 appendMessage 槽。
    connect(LogManager::Instance(), &LogManager::newMessage, m_logWidget, &LogWidget::appendMessage);

    // 连接2: 相机面板的用户请求 -> MainWindow 的处理槽
    connect(m_cameraPanel, &CameraPanel::searchDevicesRequested, this, &MainWindow::onSearchDevicesRequested);
    connect(m_cameraPanel, &CameraPanel::connectDeviceRequested, this, &MainWindow::onConnectDeviceRequested);
    connect(m_cameraPanel, &CameraPanel::disconnectDeviceRequested, this, &MainWindow::onDisconnectDeviceRequested);
    connect(m_cameraPanel, &CameraPanel::singleShotRequested, this, &MainWindow::onSingleShotRequested);
    connect(m_cameraPanel, &CameraPanel::continuousShotToggled, this, &MainWindow::onContinuousShotToggled);
    connect(m_cameraPanel, &CameraPanel::exposureChanged, this, &MainWindow::onExposureChanged);
    connect(m_cameraPanel, &CameraPanel::gainChanged, this, &MainWindow::onGainChanged);

    // 连接3: 检测面板的用户请求 -> MainWindow 的处理槽
    connect(m_inspectPanel, &InspectPanel::loadImageRequested, this, &MainWindow::onImageLoadRequested);
    connect(m_inspectPanel, &InspectPanel::inspectRequested, this, &MainWindow::onInspectRequested);

    // 连接4: 相机管理器(硬件驱动)的反馈 -> MainWindow 的处理槽
    connect(m_cameraManager, &CameraManager::deviceListUpdated, this, &MainWindow::onDeviceListUpdated);
    connect(m_cameraManager, &CameraManager::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(m_cameraManager, &CameraManager::newFrameReady, this, &MainWindow::onNewFrameReady);
    connect(m_cameraManager, &CameraManager::errorOccurred, this, [this](const QString& msg){
        qCritical("CameraManager Error: %s", msg.toStdString().c_str());
    });

    // 连接5: 检测线程(算法)的反馈 -> MainWindow 的处理槽
    connect(m_inspectorThread, &InspectorThread::finishedInspection, this, &MainWindow::onInspectionFinished);
    connect(m_inspectorThread, &InspectorThread::errorOccurred, this, &MainWindow::onInspectionError);
}


// =====================================================================
//  槽函数实现 (Slot Implementations)
// =====================================================================

// --- 响应来自 InspectPanel 的槽 ---
void MainWindow::onImageLoadRequested()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.bmp)");
    if (filePath.isEmpty()) {
        qWarning("Image loading cancelled by user.");
        return;
    }

    m_currentCvImage = cv::imread(filePath.toStdString(), cv::IMREAD_GRAYSCALE);
    if (m_currentCvImage.empty()) {
        qCritical("Failed to load image from path: %s", filePath.toStdString().c_str());
        QMessageBox::critical(this, "Error", "Failed to load image file.");
        m_inspectPanel->setInspectButtonEnabled(false);
        return;
    }

    qInfo("Image loaded: %s", filePath.toStdString().c_str());
    m_currentImagePath = filePath;
    m_mainImageView->setImage(ImageConverter::cvMatToQImage(m_currentCvImage));
    m_resultImageView->setImage(QImage());
    m_inspectPanel->clearResults();
    m_inspectPanel->setInspectButtonEnabled(true);
}

void MainWindow::onInspectRequested()
{
    if (m_currentCvImage.empty()) {
        qWarning("Inspect requested but no image is available.");
        return;
    }
    m_inspectPanel->setInspectButtonEnabled(false);
    statusBar()->showMessage(tr("Inspecting, please wait..."));
    qInfo("Inspection started...");
    m_inspectorThread->inspectImage(m_currentCvImage);
}

// --- 响应来自 CameraPanel 的槽 ---
void MainWindow::onSearchDevicesRequested()
{
    qInfo("Searching for devices...");
    m_cameraManager->searchDevices();
}

void MainWindow::onConnectDeviceRequested(int index)
{
    qInfo("Connecting to device at index %d...", index);
    m_cameraManager->connectDevice(index);
}

void MainWindow::onDisconnectDeviceRequested()
{
    qInfo("Disconnecting device...");
    m_cameraManager->disconnectDevice();
}

void MainWindow::onSingleShotRequested()
{
    qInfo("Software trigger for single shot requested.");
    m_cameraManager->sendSoftwareTrigger();
}

void MainWindow::onContinuousShotToggled(bool checked)
{
    if (checked) {
        qInfo("Starting continuous grabbing...");
        m_cameraManager->startGrabbing();
    } else {
        qInfo("Stopping continuous grabbing...");
        m_cameraManager->stopGrabbing();
    }
}

void MainWindow::onExposureChanged(int value)
{
    m_cameraManager->setExposure(value);
    qInfo("Exposure set to %d us.", value);
}

void MainWindow::onGainChanged(double value)
{
    m_cameraManager->setGain(value);
    qInfo("Gain set to %.1f dB.", value);
}

// --- 响应来自后台线程的槽 ---
void MainWindow::onInspectionFinished(const InspectorLib::MeasurementResults& results, const QImage& resultImage)
{
    qInfo("Inspection finished successfully.");
    statusBar()->showMessage(tr("Inspection successful."), 5000); // 状态栏信息显示5秒
    m_resultImageView->setImage(resultImage);
    m_inspectPanel->displayResults(results);
    m_inspectPanel->setInspectButtonEnabled(true);
}

void MainWindow::onInspectionError(const QString& message)
{
    qCritical("Inspection Error: %s", message.toStdString().c_str());
    statusBar()->showMessage("Inspection failed!", 5000);
    QMessageBox::critical(this, "Inspection Error", message);
    m_inspectPanel->setInspectButtonEnabled(true);
}

// --- 响应来自相机管理器的槽 ---
void MainWindow::onDeviceListUpdated(const QStringList& deviceList)
{
    m_cameraPanel->updateDeviceList(deviceList);
}

void MainWindow::onConnectionStatusChanged(bool connected, const QString& message)
{
    m_cameraPanel->setUiForConnectionStatus(connected);
    qInfo() << message; // 使用 qInfo() 可以方便地输出QString
    statusBar()->showMessage(message, 5000);
}

void MainWindow::onNewFrameReady(const cv::Mat& frame)
{
    // 当收到新的一帧时 (可能来自连续采集)
    m_currentCvImage = frame; // 更新当前图像
    m_mainImageView->setImage(ImageConverter::cvMatToQImage(m_currentCvImage));

    // --- 【在这里添加这行代码!】 ---
    // 解释: 既然我们已经成功获取了一张新图，现在就应该允许用户对其进行测量。
    m_inspectPanel->setInspectButtonEnabled(true);
}

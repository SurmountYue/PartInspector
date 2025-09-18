// src/InspectorGUI/mainwindow.h (最终完整版)

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> // QMainWindow是Qt应用程序主窗口的标准基类
#include <QImage>
#include "Inspector.h" // 包含后端库头文件，以使用MeasurementResults

// --- 前向声明 (Forward Declarations) ---
// 解释: 在头文件中，我们只需要知道这些类的“名字”即可声明它们的指针。
//       使用前向声明而不是直接 #include 对应的头文件，是一种非常好的C++编程习惯，
//       它可以：
//       1. 大幅减少编译时间：因为包含此头文件的文件，无需再去解析所有这些被包含头文件的内容。
//       2. 避免循环依赖：防止 A.h 包含 B.h，同时 B.h 又包含 A.h 的情况发生。
class ImageView;
class LogWidget;
class InspectPanel;
class CameraPanel;
class InspectorThread;
class CameraManager;
class QSplitter;

/**
 * @class MainWindow
 * @brief 应用程序的主窗口类，扮演“总指挥”的角色。
 *
 * @details 它的核心职责是：
 * 1. 创建并拥有(own)所有的UI子模块和后台逻辑模块。
 * 2. 负责将这些UI模块通过布局和QSplitter组合成最终的用户界面。
 * 3. 作为所有模块间的“交通枢纽”，通过信号与槽机制，连接所有模块的通信。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT // 启用Qt元对象系统，这是使用信号与槽的强制要求

public:
    /**
     * @brief 构造函数。
     * @param parent 父对象指针，遵循Qt的内存管理模型。
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 析构函数。
     */
    ~MainWindow();

private slots:
    // --- 响应来自 InspectPanel (检测面板) 的信号 ---

    /**
     * @brief 当用户在InspectPanel中点击“加载图片”按钮时，此槽函数被调用。
     */
    void onImageLoadRequested();

    /**
     * @brief 当用户在InspectPanel中点击“执行测量”按钮时，此槽函数被调用。
     */
    void onInspectRequested();

    // --- 响应来自 CameraPanel (相机面板) 的信号 ---

    void onSearchDevicesRequested();
    void onConnectDeviceRequested(int index);
    void onDisconnectDeviceRequested();
    void onSingleShotRequested();
    void onContinuousShotToggled(bool checked);
    void onExposureChanged(int value);
    void onGainChanged(double value);

    // --- 响应来自后台线程 InspectorThread 的信号 ---

    /**
     * @brief 当后台测量成功完成时，此槽函数被调用。
     * @param results 测量结果数据包。
     * @param resultImage 带有可视化标记的结果图。
     */
    void onInspectionFinished(const InspectorLib::MeasurementResults& results, const QImage& resultImage);

    /**
     * @brief 当后台测量发生错误时，此槽函数被调用。
     * @param message 错误信息。
     */
    void onInspectionError(const QString& message);

    // --- 响应来自相机管理器 CameraManager 的信号 ---

    void onDeviceListUpdated(const QStringList& deviceList);
    void onConnectionStatusChanged(bool connected, const QString& message);
    void onNewFrameReady(const cv::Mat& frame); // 当相机传来新的一帧图像时被调用

private:
    // --- 私有辅助函数 ---
    void setupUi();           // 负责创建和布局所有UI控件
    void setupConnections();  // 负责连接所有模块的信号与槽

    // --- 核心数据成员 ---
    QString m_currentImagePath; // 存储当前从文件加载的图片路径
    cv::Mat m_currentCvImage;   // 存储当前加载或由相机采集的OpenCV格式图像，用于传递给测量线程

    // --- 后台逻辑对象指针 ---
    InspectorThread* m_inspectorThread; // 后台测量线程
    CameraManager* m_cameraManager;     // 相机硬件管理器

    // --- UI控件成员指针 ---
    // 左侧面板
    CameraPanel* m_cameraPanel;     // 相机控制面板
    InspectPanel* m_inspectPanel;   // 检测控制面板
    LogWidget* m_logWidget;         // 日志显示窗口
    QSplitter* m_leftSplitter;      // 用于上下分割左侧三个面板

    // 右侧显示区
    ImageView* m_mainImageView;    // 用于显示原图或实时视频
    ImageView* m_resultImageView;  // 用于显示测量结果图
    QSplitter* m_rightSplitter;    // 用于上下分割两个图像视图

    // 主布局
    QSplitter* m_mainSplitter;      // 用于左右分割主窗口
};

#endif // MAINWINDOW_H

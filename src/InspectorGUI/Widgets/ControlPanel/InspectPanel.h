// src/InspectorGUI/Widgets/ControlPanel/InspectPanel.h

#ifndef INSPECTPANEL_H
#define INSPECTPANEL_H

#include <QWidget>
#include "Inspector.h" // 【重要】包含我们后端库的头文件，因为我们需要用到 MeasurementResults 结构体

// --- 前向声明 ---
// 解释: 我们将在这个类的实现(.cpp)中使用这些Qt控件的指针。
//       在头文件中使用前向声明，而不是直接#include，有两个好处：
//       1. 减少编译依赖：如果这些被包含的头文件发生改变，只有.cpp文件需要重新编译，而包含本文件的其他文件则不受影响。
//       2. 加快编译速度：编译器只需要知道这些名字是合法的类名即可，无需加载它们的完整定义。
class QPushButton;
class QTextEdit;
class QVBoxLayout;


/**
 * @class InspectPanel
 * @brief 一个独立的UI面板，负责提供用户操作（加载、测量）的按钮和显示文本结果的区域。
 *
 * @details 这个类的设计遵循“职责分离”原则。它本身不执行文件操作或图像算法，
 * 而是通过发射“请求”信号，将这些任务“委托”给主窗口(MainWindow)来协调处理。
 * 它还提供了一个公共槽 `displayResults`，用于从外部接收并显示数据。
 */
class InspectPanel : public QWidget
{
    Q_OBJECT // 启用元对象系统，这是使用信号与槽的前提

public:
    /**
     * @brief 构造函数。
     * @param parent 父对象指针，用于Qt的自动内存管理。
     */
    explicit InspectPanel(QWidget *parent = nullptr);

    /**
     * @brief 析构函数。
     */
    ~InspectPanel();

    // --- 公共接口函数 ---

    /**
     * @brief 设置“执行测量”(Inspect)按钮的启用/禁用状态。
     * @details 这个函数被设计为由外部调用（例如，由MainWindow调用），
     * 以便在没有加载图像或正在处理时禁用按钮。
     * @param enabled true 为启用, false 为禁用。
     */
    void setInspectButtonEnabled(bool enabled);

    /**
     * @brief 清空结果显示文本框。
     * @details 这个函数也由外部调用，例如在加载新图片时，需要清空上一次的测量结果。
     */
    void clearResults();

public slots:
    /**
     * @brief 一个公开的槽函数，用于接收并格式化显示测量结果。
     * @param results 从后台线程传来的、包含所有测量数据的 MeasurementResults 结构体。
     */
    void displayResults(const InspectorLib::MeasurementResults& results);

signals:
    /**
     * @brief 当用户点击“加载图片”按钮时，发出此信号。
     * @details 主窗口将连接到这个信号。当信号发出时，主窗口会负责弹出文件选择对话框。
     * 这种“信号转发”的设计让 InspectPanel 无需关心文件对话框的具体实现。
     */
    void loadImageRequested();

    /**
     * @brief 当用户点击“执行测量”按钮时，发出此信号。
     * @details 主窗口将连接到这个信号。当信号发出时，主窗口会负责获取当前图像并启动后台线程进行处理。
     */
    void inspectRequested();


private:
    /**
     * @brief 辅助函数，在构造函数中被调用，负责创建和布局所有UI控件。
     */
    void setupUi();

    // --- 成员变量 (指向UI控件的指针) ---
    QPushButton* m_loadImageButton; // “加载图片”按钮
    QPushButton* m_inspectButton;   // “执行测量”按钮
    QTextEdit* m_resultsText;     // 用于显示文本测量结果的区域

    QVBoxLayout* m_mainLayout;    // 该控件的主布局
};

#endif // INSPECTPANEL_H

// src/InspectorGUI/Widgets/ControlPanel/CameraPanel.h

#ifndef CAMERAPANEL_H
#define CAMERAPANEL_H

#include <QWidget>

// --- 前向声明 ---
// 解释: 我们将使用这些Qt控件的指针。在.h文件中使用前向声明是一种好习惯。
class QGroupBox;
class QComboBox;
class QPushButton;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QVBoxLayout;

class CameraPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPanel(QWidget *parent = nullptr);
    ~CameraPanel();

    // --- 公共接口函数 (由MainWindow调用，用于更新UI状态) ---

    /**
     * @brief 更新UI界面上的设备列表下拉框。
     * @param deviceList 从CameraManager搜索到的设备名称列表。
     */
    void updateDeviceList(const QStringList& deviceList);

    /**
     * @brief 根据相机实际连接状态，更新UI按钮的启用/禁用状态。
     * @param connected true表示已连接, false表示未连接。
     */
    void setUiForConnectionStatus(bool connected);

signals:
    // --- 向外广播用户请求的信号 (由MainWindow监听) ---

    void searchDevicesRequested();       // 用户点击了“搜索”按钮
    void connectDeviceRequested(int index);   // 用户选择了某个设备并点击了“连接”按钮
    void disconnectDeviceRequested();    // 用户点击了“断开”按钮
    void singleShotRequested();          // 用户点击了“单张采集”按钮
    void continuousShotToggled(bool checked); // 用户切换了“连续采集”按钮的状态
    void exposureChanged(int value);     // 用户调整了曝光值
    void gainChanged(double value);      // 用户调整了增益值

private:
    // --- 辅助函数 ---
    void setupUi();           // 创建和布局所有UI控件
    void setupConnections();  // 连接内部的信号与槽

    // --- UI控件成员指针 ---

    // 整体布局
    QVBoxLayout* m_mainLayout;

    // “设备连接”分组
    QGroupBox* m_deviceGroupBox;
    QComboBox* m_deviceComboBox;
    QPushButton* m_searchButton;
    QPushButton* m_connectButton;
    QLabel* m_statusLabel;

    // “采集控制”分组
    QGroupBox* m_acquisitionGroupBox;
    QPushButton* m_singleShotButton;
    QPushButton* m_continuousShotButton;

    // “参数设置”分组
    QGroupBox* m_parameterGroupBox;
    QSpinBox* m_exposureSpinBox;
    QDoubleSpinBox* m_gainSpinBox;
};

#endif // CAMERAPANEL_H

// src/InspectorGUI/Core/CameraManager.h

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <opencv2/opencv.hpp>

// 【重要】包含海康相机的SDK头文件
// 这是我们项目中唯一一个需要直接和SDK打交道的文件。
#include "MvCameraControl.h"

class CameraManager : public QObject
{
    Q_OBJECT

public:
    explicit CameraManager(QObject *parent = nullptr);
    ~CameraManager();

    // --- 公共接口函数 (由MainWindow调用) ---
    void searchDevices();
    void connectDevice(int index);
    void disconnectDevice();
    void startGrabbing();
    void stopGrabbing();
    void sendSoftwareTrigger();
    void setExposure(int value);
    void setGain(double value);

signals:
    // --- 向外广播状态和数据的信号 (由MainWindow监听) ---
    void deviceListUpdated(const QStringList& deviceList); // 设备列表已更新
    void connectionStatusChanged(bool connected, const QString& message); // 连接状态已改变
    void newFrameReady(const cv::Mat& frame); // 【核心】已捕获到新的图像帧！
    void errorOccurred(const QString& message); // 发生了错误

private:
    // --- 内部辅助函数 ---
    /**
     * @brief 这是相机SDK的回调函数，将在一个独立的SDK线程中被调用。
     * @param pData 指向图像数据的指针。
     * @param pFrameInfo 包含图像帧信息的结构体。
     * @param pUser 传递给回调的用户自定义指针 (我们将用它来传递 this 指针)。
     */
    static void __stdcall frameCallback(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser);

    // --- 成员变量 ---
    void* m_cameraHandle = nullptr; // 指向相机实例的句柄，由SDK提供
    MV_CC_DEVICE_INFO_LIST m_deviceList; // 存储搜索到的设备列表
};

#endif // CAMERAMANAGER_H

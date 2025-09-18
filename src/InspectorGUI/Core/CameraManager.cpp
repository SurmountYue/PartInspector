// src/InspectorGUI/Core/CameraManager.cpp

#include "CameraManager.h"
#include <QDebug>

// --- 构造函数 ---
CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
{
    // 初始化时，确保设备列表结构体是干净的
    memset(&m_deviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
}

// --- 析构函数 ---
CameraManager::~CameraManager()
{
    // 确保在对象被销毁时，相机能被安全地断开和释放
    disconnectDevice();
}

// --- 搜索设备 ---
void CameraManager::searchDevices()
{
    // 调用海康SDK的枚举设备函数
    int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_deviceList);
    if (MV_OK != nRet)
    {
        qCritical("Failed to enumerate devices. Error code: %#x", nRet);
        emit errorOccurred("Failed to search for cameras.");
        return;
    }

    QStringList deviceNames;
    for (unsigned int i = 0; i < m_deviceList.nDeviceNum; i++)
    {
        // 从设备信息中提取一个可读的名字
        MV_CC_DEVICE_INFO* pDeviceInfo = m_deviceList.pDeviceInfo[i];
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE) {
            // 对于网口相机，使用用户自定义名或型号名
            QString name = (const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName;
            if (name.isEmpty()) {
                name = (const char*)pDeviceInfo->SpecialInfo.stGigEInfo.chModelName;
            }
            deviceNames.append(name);
        }
    }

    // 通过信号将找到的设备列表广播出去
    emit deviceListUpdated(deviceNames);
    qInfo("%d devices found.", m_deviceList.nDeviceNum);
}

// --- 连接设备 ---
void CameraManager::connectDevice(int index)
{
    if (index < 0 || index >= (int)m_deviceList.nDeviceNum) {
        emit errorOccurred("Invalid device index selected.");
        return;
    }
    if (m_cameraHandle != nullptr) {
        disconnectDevice(); // 先断开已有的连接
    }

    // 1. 根据索引选择设备，并创建句柄
    int nRet = MV_CC_CreateHandle(&m_cameraHandle, m_deviceList.pDeviceInfo[index]);
    if (MV_OK != nRet) {
        qCritical("Failed to create camera handle. Error: %#x", nRet);
        emit connectionStatusChanged(false, "Failed to create camera handle.");
        return;
    }

    // 2. 打开设备
    nRet = MV_CC_OpenDevice(m_cameraHandle);
    if (MV_OK != nRet) {
        MV_CC_DestroyHandle(m_cameraHandle);
        m_cameraHandle = nullptr;
        qCritical("Failed to open device. Error: %#x", nRet);
        emit connectionStatusChanged(false, "Failed to open camera. It might be in use.");
        return;
    }

    // 3. 【关键】注册图像数据回调函数
    // 我们将 this 指针作为用户自定义数据(pUser)传递给回调函数
    nRet = MV_CC_RegisterImageCallBackEx(m_cameraHandle, frameCallback, this);
    if (MV_OK != nRet) {
        qCritical("Failed to register image callback. Error: %#x", nRet);
        emit errorOccurred("Failed to register image callback.");
    }

    // 发射连接成功信号
    emit connectionStatusChanged(true, "Camera connected successfully.");
    qInfo("Camera connected.");
}

// --- 断开设备 ---
void CameraManager::disconnectDevice()
{
    if (m_cameraHandle == nullptr) return;

    MV_CC_StopGrabbing(m_cameraHandle); // 确保停止采集
    MV_CC_CloseDevice(m_cameraHandle);
    MV_CC_DestroyHandle(m_cameraHandle);
    m_cameraHandle = nullptr;

    emit connectionStatusChanged(false, "Camera disconnected.");
    qInfo("Camera disconnected.");
}

// --- 开始连续采集 ---
void CameraManager::startGrabbing()
{
    if (m_cameraHandle == nullptr) return;
    MV_CC_SetEnumValue(m_cameraHandle, "TriggerMode", MV_TRIGGER_MODE_OFF); // 设置为连续模式
    MV_CC_StartGrabbing(m_cameraHandle);
}

// --- 停止连续采集 ---
void CameraManager::stopGrabbing()
{
    if (m_cameraHandle == nullptr) return;
    MV_CC_StopGrabbing(m_cameraHandle);
}

// --- 发送软触发 ---
void CameraManager::sendSoftwareTrigger()
{
    if (m_cameraHandle == nullptr) return;
    MV_CC_SetEnumValue(m_cameraHandle, "TriggerMode", MV_TRIGGER_MODE_ON);      // 确保为触发模式
    MV_CC_SetEnumValue(m_cameraHandle, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE); // 设置为软触发
    MV_CC_StartGrabbing(m_cameraHandle); // 必须先开始采集，才能接收触发
    MV_CC_SetCommandValue(m_cameraHandle, "TriggerSoftware"); // 发送软触发命令
}

// --- 设置曝光 ---
void CameraManager::setExposure(int value)
{
    if (m_cameraHandle == nullptr) return;
    MV_CC_SetFloatValue(m_cameraHandle, "ExposureTime", (float)value);
}

// --- 设置增益 ---
void CameraManager::setGain(double value)
{
    if (m_cameraHandle == nullptr) return;
    MV_CC_SetFloatValue(m_cameraHandle, "Gain", (float)value);
}


// --- 静态回调函数的实现 ---
void __stdcall CameraManager::frameCallback(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
    // 【关键】pUser 就是我们注册回调时传递的 this 指针！
    // 我们把它转换回 CameraManager* 类型。
    CameraManager* manager = static_cast<CameraManager*>(pUser);
    if (manager == nullptr || pData == nullptr || pFrameInfo == nullptr) {
        return;
    }

    // 将SDK返回的图像数据，包装成一个OpenCV的Mat对象。
    // 这是一个高效的操作，它与原始数据共享内存，没有发生拷贝。
    cv::Mat frame(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pData);

    // 【关键】通过manager实例，发射信号，将图像帧传递出去。
    // 因为信号是在不同的线程（SDK的回调线程）中发出的，Qt的信号槽机制会自动
    // 处理线程间的切换，确保槽函数在主GUI线程中被安全地执行。
    emit manager->newFrameReady(frame.clone()); // .clone()创建一个深拷贝，确保主线程拿到的是独立的数据
}

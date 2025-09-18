// src/InspectorGUI/Core/InspectorThread.cpp

#include "InspectorThread.h"

// 包含我们后端库的头文件，因为我们将要调用它
#include "../../InspectorLib/Inspector.h"

// 这是一个辅助头文件，用于在 Qt 的 QImage 和 OpenCV 的 cv::Mat 之间进行转换
// 我们将在下一步创建这个文件
#include "ImageConverter.h"

#include <QDebug>

// --- 构造函数 ---
InspectorThread::InspectorThread(QObject *parent)
    : QThread(parent)
{
    // 在构造时，注册我们自定义的 MeasurementResults 类型。
    // 这样Qt的信号槽系统才能正确地在线程之间传递它。
    qRegisterMetaType<InspectorLib::MeasurementResults>("InspectorLib::MeasurementResults");
}

// --- 析构函数 ---
InspectorThread::~InspectorThread()
{
    // 等待线程执行完成再销毁，确保安全退出
    wait();
}

// --- 核心公共接口：接收任务 ---
void InspectorThread::inspectImage(const cv::Mat& imageToInspect)
{
    // 1. 将传入的图像复制一份，存储到成员变量中。
    //    这是一个重要的步骤，可以防止在主线程的图像被销毁后，
    //    后台线程访问一个无效的内存地址。
    m_imageToInspect = imageToInspect.clone();

    // 2. 调用 QThread::start() 来启动线程。
    //    【重要】永远不要直接调用 run()！
    //    调用 start() 会创建一个新的操作系统线程，然后在这个新线程中自动调用 run()。
    start();
}


// --- 核心实现：后台执行的函数 ---
void InspectorThread::run()
{
    // 解释: 这部分的所有代码，都在一个独立的后台线程中执行，
    //       完全不会影响主GUI线程的响应。

    // 1. 准备用于接收结果的“容器”
    InspectorLib::MeasurementResults results;
    cv::Mat resultCanvas; // 用于接收带标记的可视化结果图

    // 2. 【核心调用】在这里，我们调用了我们的DLL！
    uint32_t statusCode = InspectorLib::InspectPart(m_imageToInspect, results, resultCanvas);

    // 3. 检查执行状态
    if (statusCode == 0)
    {
        // --- 如果算法成功 ---

        // a. 将OpenCV的BGR格式的Mat，转换为Qt的RGB格式的QImage
        //    我们将在 ImageConverter.h 中实现这个辅助函数。
        QImage resultQImage = ImageConverter::cvMatToQImage(resultCanvas);

        // b. 发射“完成”信号，将结果安全地传递回主GUI线程
        emit finishedInspection(results, resultQImage);
    }
    else
    {
        // --- 如果算法失败 ---

        // a. 根据错误码，创建一个人类可读的错误信息字符串
        QString errorMessage = QString("Inspection failed with error code: %1").arg(statusCode);

        // b. 发射“错误”信号，将错误信息传递回主GUI线程
        emit errorOccurred(errorMessage);
    }
}

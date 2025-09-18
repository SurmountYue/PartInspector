// src/InspectorGUI/Core/InspectorThread.h

#ifndef INSPECTORTHREAD_H
#define INSPECTORTHREAD_H

#include <QThread>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "Inspector.h" // 包含头文件以使用 MeasurementResults

// 这是一个非常重要的Qt元类型声明。
// 因为我们想在信号和槽之间传递自定义的 MeasurementResults 结构体，
// 我们必须先用 Q_DECLARE_METATYPE 宏将其注册到Qt的元对象系统中。
Q_DECLARE_METATYPE(InspectorLib::MeasurementResults)

class InspectorThread : public QThread
{
    Q_OBJECT

public:
    explicit InspectorThread(QObject *parent = nullptr);
    ~InspectorThread();

    /**
     * @brief [核心] 启动线程进行测量。
     * @param imageToInspect 要进行测量的OpenCV图像。
     */
    void inspectImage(const cv::Mat& imageToInspect);

signals:
    /**
     * @brief 当测量完成时，发出此信号。
     * @param results 包含所有测量数据的结构体。
     * @param resultImage 绘制了可视化结果的图像。
     */
    void finishedInspection(const InspectorLib::MeasurementResults& results, const QImage& resultImage);

    /**
     * @brief 当测量过程中发生错误时，发出此信号。
     * @param message 错误信息字符串。
     */
    void errorOccurred(const QString& message);

protected:
    /**
     * @brief QThread的核心虚函数。
     * 当我们调用 start() 时，这个函数将在一个新的后台线程中被自动执行。
     */
    virtual void run() override;

private:
    cv::Mat m_imageToInspect; // 存储待处理的图像副本
};

#endif // INSPECTORTHREAD_H

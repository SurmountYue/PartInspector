// src/InspectorGUI/Core/ImageConverter.h

#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <QImage>
#include <QDebug>
#include <opencv2/opencv.hpp>

/**
 * @class ImageConverter
 * @brief 一个静态工具类，用于在 Qt 的 QImage 和 OpenCV 的 cv::Mat 之间进行转换。
 */
class ImageConverter {
public:

    /**
     * @brief 将 OpenCV 的 cv::Mat 转换为 Qt 的 QImage。
     * @param mat 输入的 cv::Mat 图像。支持 CV_8UC1 (灰度) 和 CV_8UC3 (BGR彩色)。
     * @return 转换后的 QImage。
     */
    static QImage cvMatToQImage(const cv::Mat& mat)
    {
        // 根据Mat的类型进行不同的转换
        switch (mat.type())
        {
        // Case 1: 3通道BGR彩色图 (CV_8UC3)
        case CV_8UC3:
        {
            // 创建一个与Mat共享内存的QImage
            // 【关键】Qt的RGB格式与OpenCV的BGR格式通道顺序相反！
            QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            // 使用 .rgbSwapped() 进行通道交换 (BGR -> RGB)，并返回一个深拷贝
            return image.rgbSwapped();
        }

        // Case 2: 1通道灰度图 (CV_8UC1)
        case CV_8UC1:
        {
            // 创建一个与Mat共享内存的QImage
            // Format_Grayscale8 完美匹配
            QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
            // 返回一个深拷贝，确保线程安全
            return image.copy();
        }

        // 其他未处理的格式
        default:
            qWarning("ImageConverter::cvMatToQImage() - Unsupported cv::Mat type: %d", mat.type());
            break;
        }
        return QImage(); // 如果格式不支持，返回一个空QImage
    }

    /**
     * @brief 将 Qt 的 QImage 转换为 OpenCV 的 cv::Mat。
     * @param image 输入的 QImage 图像。支持多种RGB和灰度格式。
     * @return 转换后的 cv::Mat。
     */
    static cv::Mat qImageToCvMat(const QImage& image)
    {
        cv::Mat mat;
        // 根据QImage的格式进行不同的转换
        switch (image.format())
        {
        // Case 1: 32位彩色图 (带Alpha通道或填充字节)
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
        {
            // 创建一个与QImage共享内存的Mat (4通道)
            cv::Mat temp(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            // 将4通道的BGRA图像转换为3通道的BGR图像
            cv::cvtColor(temp, mat, cv::COLOR_BGRA2BGR);
            break;
        }

        // Case 2: 24位彩色图 (RGB888)
        case QImage::Format_RGB888:
        {
            // 【关键】需要先进行通道交换 (RGB -> BGR)
            // .rgbSwapped() 会创建一个临时的、通道交换后的副本
            QImage swapped = image.rgbSwapped();
            // 创建一个与交换后的图像共享内存的Mat
            mat = cv::Mat(swapped.height(), swapped.width(), CV_8UC3, (void*)swapped.constBits(), swapped.bytesPerLine()).clone();
            break;
        }

        // Case 3: 8位灰度图
        case QImage::Format_Indexed8:
        case QImage::Format_Grayscale8:
        {
            // 创建一个与QImage共享内存的Mat
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine()).clone();
            break;
        }

        // 其他未处理的格式
        default:
            qWarning("ImageConverter::qImageToCvMat() - Unsupported QImage format: %d", image.format());
            mat = cv::Mat(); // 返回一个空Mat
            break;
        }
        return mat;
    }
};

#endif // IMAGECONVERTER_H

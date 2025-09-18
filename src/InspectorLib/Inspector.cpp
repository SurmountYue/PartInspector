// Inspector.cpp (最终完整算法版)

#include "Inspector.h"
#include <vector>

// 定义颜色常量 (BGR格式)
const cv::Scalar COLOR_BLUE(255, 0, 0);
const cv::Scalar COLOR_GREEN(0, 255, 0);
const cv::Scalar COLOR_RED(0, 0, 255);
const cv::Scalar COLOR_YELLOW(0, 255, 255);

namespace InspectorLib
{
    // 我们的核心函数实现
    uint32_t INSPECTOR_API InspectPart(const cv::Mat& srcImage,
        MeasurementResults& results,
        cv::Mat& resultImage)
    {
        // --- a. b. 防御性检查 (不变) ---
        if (srcImage.empty()) return 1;
        if (srcImage.channels() != 1) return 2;

        // --- c. 创建可视化画布 (不变) ---
        cv::cvtColor(srcImage, resultImage, cv::COLOR_GRAY2BGR);

        // --- d. 图像二值化 (不变) ---
        cv::Mat binaryImage;
        cv::threshold(srcImage, binaryImage, 50, 255, cv::THRESH_BINARY);

        // --- e. 轮廓发现 (不变) ---
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(binaryImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        // --- f. 【关键实现】第一遍循环：查找并锁定零件的外轮廓 ---
        // (这就是我们之前讨论过的，您提出的更健壮的算法)

        int partContourIdx = -1; // 用来存储零件外轮廓的索引
        double maxArea = 0.0;    // 用来存储最大面积

        for (int i = 0; i < contours.size(); i++)
        {
            if (hierarchy[i][3] == -1) // 这是一个顶层轮廓 (没有父亲)
            {
                double area = cv::contourArea(contours[i]);
                if (area > maxArea)
                {
                    maxArea = area;
                    partContourIdx = i; // 记录下这个面积最大的顶层轮廓
                }
            }
        }

        // 防御性检查：如果没找到任何顶层轮廓
        if (partContourIdx == -1)
        {
            return 3; // 返回错误码3，代表“未找到零件轮廓”
        }

        // --- g. 【关键实现】测量并绘制外轮廓 ---
        // 我们已经锁定了外轮廓是 contours[partContourIdx]。
        results.boundingBox = cv::minAreaRect(contours[partContourIdx]); // 计算最小外接旋转矩形

        // 【绘图】在画布上画出绿色的轮廓线
        cv::drawContours(resultImage, contours, partContourIdx, COLOR_GREEN, 2);

        // 【绘图】在画布上画出红色的矩形框
        cv::Point2f vertices[4];
        results.boundingBox.points(vertices);
        for (int i = 0; i < 4; i++)
        {
            cv::line(resultImage, vertices[i], vertices[(i + 1) % 4], COLOR_RED, 2);
        }

        // --- h. 【关键实现】第二遍循环：查找并测量所有内部孔洞 ---

        // 【重要】清空上次的结果列表，防止重复运行时数据累加
        results.circles.clear();

        for (int i = 0; i < contours.size(); i++)
        {
            // 关键筛选：只处理那些“父亲”是我们刚刚找到的零件外轮廓(partContourIdx)的轮廓
            if (hierarchy[i][3] == partContourIdx)
            {
                // 这确定是一个内孔！现在区分它是“圆”还是“槽口”。
                double area = cv::contourArea(contours[i]);
                double perimeter = cv::arcLength(contours[i], true); // true=闭合轮廓

                if (perimeter == 0) continue; // 避免除零错误

                double circularity = (4 * CV_PI * area) / (perimeter * perimeter);

                // 根据圆度和面积（面积是一个经验值，用于排除可能的噪点）来区分
                if (circularity > 0.85 && area > 50) // 假设圆度大于0.85的就是圆
                {
                    // --- 这是一个小圆孔 ---
                    CircleResult circleRes;
                    // 函数: cv::minEnclosingCircle 找到最小外接圆
                    cv::minEnclosingCircle(contours[i], circleRes.center, circleRes.radius);
                    results.circles.push_back(circleRes); // 添加到结果列表

                    // 【绘图】画出蓝色的圆
                    cv::circle(resultImage, circleRes.center, (int)circleRes.radius, COLOR_BLUE, 2);
                }
                else if (area > 1000) // 假设圆度较低且面积较大的是槽口
                {
                    // --- 这是那个大槽口 ---
                    cv::RotatedRect slotBox = cv::minAreaRect(contours[i]);
                    results.slot.center = slotBox.center;
                    results.slot.angle = slotBox.angle;
                    // 保证 length 始终是长边，width 始终是短边
                    if (slotBox.size.width > slotBox.size.height) {
                        results.slot.length = slotBox.size.width;
                        results.slot.width = slotBox.size.height;
                    }
                    else {
                        results.slot.length = slotBox.size.height;
                        results.slot.width = slotBox.size.width;
                    }

                    // 【绘图】画出黄色的槽口矩形
                    cv::Point2f slotVertices[4];
                    slotBox.points(slotVertices);
                    for (int j = 0; j < 4; j++) {
                        cv::line(resultImage, slotVertices[j], slotVertices[(j + 1) % 4], COLOR_YELLOW, 2);
                    }
                }
            }
        } // 内孔循环结束

        // --- i. 返回成功 ---
        return 0;
    }

} // namespace InspectorLib
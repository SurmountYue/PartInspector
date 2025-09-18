// Inspector.cpp (���������㷨��)

#include "Inspector.h"
#include <vector>

// ������ɫ���� (BGR��ʽ)
const cv::Scalar COLOR_BLUE(255, 0, 0);
const cv::Scalar COLOR_GREEN(0, 255, 0);
const cv::Scalar COLOR_RED(0, 0, 255);
const cv::Scalar COLOR_YELLOW(0, 255, 255);

namespace InspectorLib
{
    // ���ǵĺ��ĺ���ʵ��
    uint32_t INSPECTOR_API InspectPart(const cv::Mat& srcImage,
        MeasurementResults& results,
        cv::Mat& resultImage)
    {
        // --- a. b. �����Լ�� (����) ---
        if (srcImage.empty()) return 1;
        if (srcImage.channels() != 1) return 2;

        // --- c. �������ӻ����� (����) ---
        cv::cvtColor(srcImage, resultImage, cv::COLOR_GRAY2BGR);

        // --- d. ͼ���ֵ�� (����) ---
        cv::Mat binaryImage;
        cv::threshold(srcImage, binaryImage, 50, 255, cv::THRESH_BINARY);

        // --- e. �������� (����) ---
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(binaryImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        // --- f. ���ؼ�ʵ�֡���һ��ѭ�������Ҳ���������������� ---
        // (���������֮ǰ���۹��ģ�������ĸ���׳���㷨)

        int partContourIdx = -1; // �����洢���������������
        double maxArea = 0.0;    // �����洢������

        for (int i = 0; i < contours.size(); i++)
        {
            if (hierarchy[i][3] == -1) // ����һ���������� (û�и���)
            {
                double area = cv::contourArea(contours[i]);
                if (area > maxArea)
                {
                    maxArea = area;
                    partContourIdx = i; // ��¼�����������Ķ�������
                }
            }
        }

        // �����Լ�飺���û�ҵ��κζ�������
        if (partContourIdx == -1)
        {
            return 3; // ���ش�����3������δ�ҵ����������
        }

        // --- g. ���ؼ�ʵ�֡����������������� ---
        // �����Ѿ��������������� contours[partContourIdx]��
        results.boundingBox = cv::minAreaRect(contours[partContourIdx]); // ������С�����ת����

        // ����ͼ���ڻ����ϻ�����ɫ��������
        cv::drawContours(resultImage, contours, partContourIdx, COLOR_GREEN, 2);

        // ����ͼ���ڻ����ϻ�����ɫ�ľ��ο�
        cv::Point2f vertices[4];
        results.boundingBox.points(vertices);
        for (int i = 0; i < 4; i++)
        {
            cv::line(resultImage, vertices[i], vertices[(i + 1) % 4], COLOR_RED, 2);
        }

        // --- h. ���ؼ�ʵ�֡��ڶ���ѭ�������Ҳ����������ڲ��׶� ---

        // ����Ҫ������ϴεĽ���б���ֹ�ظ�����ʱ�����ۼ�
        results.circles.clear();

        for (int i = 0; i < contours.size(); i++)
        {
            // �ؼ�ɸѡ��ֻ������Щ�����ס������Ǹո��ҵ������������(partContourIdx)������
            if (hierarchy[i][3] == partContourIdx)
            {
                // ��ȷ����һ���ڿף������������ǡ�Բ�����ǡ��ۿڡ���
                double area = cv::contourArea(contours[i]);
                double perimeter = cv::arcLength(contours[i], true); // true=�պ�����

                if (perimeter == 0) continue; // ����������

                double circularity = (4 * CV_PI * area) / (perimeter * perimeter);

                // ����Բ�Ⱥ�����������һ������ֵ�������ų����ܵ���㣩������
                if (circularity > 0.85 && area > 50) // ����Բ�ȴ���0.85�ľ���Բ
                {
                    // --- ����һ��СԲ�� ---
                    CircleResult circleRes;
                    // ����: cv::minEnclosingCircle �ҵ���С���Բ
                    cv::minEnclosingCircle(contours[i], circleRes.center, circleRes.radius);
                    results.circles.push_back(circleRes); // ��ӵ�����б�

                    // ����ͼ��������ɫ��Բ
                    cv::circle(resultImage, circleRes.center, (int)circleRes.radius, COLOR_BLUE, 2);
                }
                else if (area > 1000) // ����Բ�Ƚϵ�������ϴ���ǲۿ�
                {
                    // --- �����Ǹ���ۿ� ---
                    cv::RotatedRect slotBox = cv::minAreaRect(contours[i]);
                    results.slot.center = slotBox.center;
                    results.slot.angle = slotBox.angle;
                    // ��֤ length ʼ���ǳ��ߣ�width ʼ���Ƕ̱�
                    if (slotBox.size.width > slotBox.size.height) {
                        results.slot.length = slotBox.size.width;
                        results.slot.width = slotBox.size.height;
                    }
                    else {
                        results.slot.length = slotBox.size.height;
                        results.slot.width = slotBox.size.width;
                    }

                    // ����ͼ��������ɫ�Ĳۿھ���
                    cv::Point2f slotVertices[4];
                    slotBox.points(slotVertices);
                    for (int j = 0; j < 4; j++) {
                        cv::line(resultImage, slotVertices[j], slotVertices[(j + 1) % 4], COLOR_YELLOW, 2);
                    }
                }
            }
        } // �ڿ�ѭ������

        // --- i. ���سɹ� ---
        return 0;
    }

} // namespace InspectorLib
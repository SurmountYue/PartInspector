// Inspector.h (����������)

#ifndef INSPECTOR_H
#define INSPECTOR_H

// --- ���� extern "C" �� (�������ܺ�) ---
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C 
#endif

// --- ���� DLL ������ (������Ϊ����S����CMakeƥ��) ---
#ifdef INSPECTOR_EXPORT
#define INSPECTOR_API __declspec(dllexport)
#else
#define INSPECTOR_API __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>
#include <vector>

// --- �����ռ俪ʼ ---
namespace InspectorLib
{
    // --- ���ؼ������������������Ľṹ�嶨�壡---

    /**
     * @brief �洢����Բ�ο׵Ĳ�������
     */
    struct CircleResult
    {
        cv::Point2f center; // Բ������
        float radius;       // �뾶
    };

    /**
     * @brief �洢�ۿ�(Slot)�Ĳ�������
     */
    struct SlotResult
    {
        cv::Point2f center; // �ۿڵļ�������
        float length;       // �ۿڵ��ܳ��ȣ����ߣ�
        float width;        // �ۿڵĿ��ȣ��̱ߣ�
        float angle;        // �ۿھ�������ˮƽ��������ת�Ƕ�
    };

    /**
     * @brief �������в��������ܽ�����
     */
    struct MeasurementResults
    {
        // a. �������ߴ�
        cv::RotatedRect boundingBox; // ��������������С������ת����

        // b. �ڲ�Բ��
        std::vector<CircleResult> circles; // һ����̬�б������ڴ洢�����ҵ���СԲ��

        // c. �ڲ��ۿ�
        SlotResult slot; // һ�������Ķ��������ڴ洢�Ǹ����ۿڵ���Ϣ

        // d. ����Բ�� (Ϊδ������Ԥ��)
        float arcRadius;

        // ���캯����ȷ���ڴ��� MeasurementResults ����ʱ������ֵ������ȷ��ʼ��
        MeasurementResults() : arcRadius(0.0f) {}
    };


    /**
     * @brief [���Ľӿں���] (���ֲ���)
     */
    uint32_t INSPECTOR_API InspectPart(const cv::Mat& srcImage,
        MeasurementResults& results,
        cv::Mat& resultImage);

} // ���������ռ� InspectorLib

#endif // INSPECTOR_H

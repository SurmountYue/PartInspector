// ExampleMain.cpp (InspectorLib �Ĳ���̨)

// --- 1. ������Ҫ��ͷ�ļ� ---
#include <iostream>             // �����ڿ���̨��ӡ�ı� (std::cout)
#include "Inspector.h"          // ���������Լ��Ŀ�ӿڣ�
#include <opencv2/opencv.hpp>   // ����OpenCV����Ϊmain����Ҳ��Ҫ���غ���ʾͼ��

// --- 2. ʹ�������ռ� ---
using namespace std;
using namespace cv;
using namespace InspectorLib; // Ҳʹ�������Լ��������ռ䣬�����Ͳ���д InspectorLib::InspectPart

/**
 * @brief ��������������ת���ε���Ϣ��ӡ������̨
 */
void printBox(const RotatedRect& box)
{
    cout << "\t - Center: (" << box.center.x << ", " << box.center.y << ")" << endl;
    cout << "\t - Size: " << box.size.width << " x " << box.size.height << endl;
    cout << "\t - Angle: " << box.angle << " degrees" << endl;
}

// --- 3. C++��������� ---
int main()
{
    cout << "Starting Inspector Test..." << endl;

    // --- a. ��������Ҫ���Ե�ͼƬ·�� ---
    // �������Ŀ�ִ���ļ��� bin/ Ŀ¼���У���ͼƬ����Ŀ��Ŀ¼�µ� images/ �ļ����С�
    string imagePath = "C:/Users/Administrator/Desktop/PartInspectorProject/images/bracket_tilted_02.png";

    // --- b. ���ز���ͼ�� ---
    // ����: cv::imread()���ԻҶ�ģʽ(IMREAD_GRAYSCALE)����ͼ�������������㷨��Ҫ�ĸ�ʽ��
    Mat testImage = imread(imagePath, IMREAD_GRAYSCALE);

    if (testImage.empty())
    {
        cout << "!!! FATAL ERROR: Could not load image from: " << imagePath << endl;
        return -1;
    }

    // --- c. ׼�������ڽ��ս���ġ������� ---
    MeasurementResults results;   // �������ǵ��ܽ���ṹ�壨�Ǹ����ձ�񡱣�
    Mat debugCanvas;            // ����һ���յ�Mat�������š��հ׻�������

    // --- d. �����ĵ��á�ִ�����ǵ��㷨���棡 ---
    // ���ǵ��ÿ⺯���������С���ͷ���嵽���������ϡ�
    uint32_t statusCode = InspectPart(testImage, results, debugCanvas);

    // --- e. ���ִ��״̬ ---
    if (statusCode != 0)
    {
        // ������ص�״̬�벻��0��˵���㷨�ڲ������ˡ�
        cout << "!!! ALGORITHM FAILED with error code: " << statusCode << endl;
        return -1;
    }

    // --- f. �������֤������ɹ������������ݴ�ӡ������̨ ---
    cout << "\n===== Inspection Results =====\n" << endl;

    cout << "[Outer Bounding Box]:" << endl;
    printBox(results.boundingBox);

    cout << "\n[Small Circles Found]: " << results.circles.size() << endl;
    for (size_t i = 0; i < results.circles.size(); i++)
    {
        cout << "  - Circle " << i << ":" << endl;
        cout << "\t - Center: (" << results.circles[i].center.x << ", " << results.circles[i].center.y << ")" << endl;
        cout << "\t - Radius: " << results.circles[i].radius << endl;
    }

    cout << "\n[Slot Found]:" << endl;
    cout << "\t - Center: (" << results.slot.center.x << ", " << results.slot.center.y << ")" << endl;
    cout << "\t - Length: " << results.slot.length << endl;
    cout << "\t - Width: " << results.slot.width << " (Arc Radius: " << results.slot.width / 2.0 << ")" << endl;
    cout << "\t - Angle: " << results.slot.angle << endl;

    cout << "\n===============================" << endl;


    // --- g. �����ӻ���֤����ʾ���ͼ�� ---
    cv::imshow("Source Image", testImage);      // ��ʾԭʼͼ��
    cv::imshow("Result Canvas", debugCanvas); // ��ʾ�����㷨���ƵĽ��ͼ��

    cout << "Inspection successful. Press any key to exit." << endl;
    cv::waitKey(0); // ��ͣ���򣬵ȴ��û��������Ա������ܿ���ͼ�񴰿ڡ�

    return 0; // �����˳�
}
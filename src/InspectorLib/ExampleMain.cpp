// ExampleMain.cpp (InspectorLib 的测试台)

// --- 1. 包含必要的头文件 ---
#include <iostream>             // 用于在控制台打印文本 (std::cout)
#include "Inspector.h"          // 包含我们自己的库接口！
#include <opencv2/opencv.hpp>   // 包含OpenCV（因为main函数也需要加载和显示图像）

// --- 2. 使用命名空间 ---
using namespace std;
using namespace cv;
using namespace InspectorLib; // 也使用我们自己的命名空间，这样就不用写 InspectorLib::InspectPart

/**
 * @brief 辅助函数：将旋转矩形的信息打印到控制台
 */
void printBox(const RotatedRect& box)
{
    cout << "\t - Center: (" << box.center.x << ", " << box.center.y << ")" << endl;
    cout << "\t - Size: " << box.size.width << " x " << box.size.height << endl;
    cout << "\t - Angle: " << box.angle << " degrees" << endl;
}

// --- 3. C++程序主入口 ---
int main()
{
    cout << "Starting Inspector Test..." << endl;

    // --- a. 定义我们要测试的图片路径 ---
    // 假设您的可执行文件在 bin/ 目录运行，而图片在项目根目录下的 images/ 文件夹中。
    string imagePath = "C:/Users/Administrator/Desktop/PartInspectorProject/images/bracket_tilted_02.png";

    // --- b. 加载测试图像 ---
    // 函数: cv::imread()，以灰度模式(IMREAD_GRAYSCALE)加载图像，这正是我们算法需要的格式。
    Mat testImage = imread(imagePath, IMREAD_GRAYSCALE);

    if (testImage.empty())
    {
        cout << "!!! FATAL ERROR: Could not load image from: " << imagePath << endl;
        return -1;
    }

    // --- c. 准备好用于接收结果的“容器” ---
    MeasurementResults results;   // 创建我们的总结果结构体（那个“空表格”）
    Mat debugCanvas;            // 创建一个空的Mat对象（那张“空白画布”）

    // --- d. 【核心调用】执行我们的算法引擎！ ---
    // 我们调用库函数，把所有“插头”插到“插座”上。
    uint32_t statusCode = InspectPart(testImage, results, debugCanvas);

    // --- e. 检查执行状态 ---
    if (statusCode != 0)
    {
        // 如果返回的状态码不是0，说明算法内部出错了。
        cout << "!!! ALGORITHM FAILED with error code: " << statusCode << endl;
        return -1;
    }

    // --- f. 【结果验证】如果成功，将测量数据打印到控制台 ---
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


    // --- g. 【可视化验证】显示结果图像 ---
    cv::imshow("Source Image", testImage);      // 显示原始图像
    cv::imshow("Result Canvas", debugCanvas); // 显示我们算法绘制的结果图！

    cout << "Inspection successful. Press any key to exit." << endl;
    cv::waitKey(0); // 暂停程序，等待用户按键，以便我们能看到图像窗口。

    return 0; // 正常退出
}
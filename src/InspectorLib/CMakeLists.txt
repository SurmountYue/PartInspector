# CMakeLists.txt (用于我们的 PartInspectorProject)

# --- S.1 工程创建 ---
CMAKE_MINIMUM_REQUIRED(VERSION 3.22) # 声明CMake的最低版本要求
PROJECT(InspectorLib)                # 定义项目名称
set(CMAKE_CXX_STANDARD 17)           # 告诉编译器使用 C++17 标准

# --- S.2 定义输出路径 ---
# 解释: 这组命令的目的是将所有生成的 .dll 和 .exe 文件都统一输出到项目根目录下的 "bin" 文件夹中。
#       ${CMAKE_CURRENT_SOURCE_DIR} 指的是当前 CMakeLists.txt 所在的目录 (即 src/InspectorLib/)
#       /../.. 会返回两级，到达项目根目录 (PartInspectorProject/)
#       /bin 指定输出到 bin 文件夹。
set(ZZ_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(OUTPUT_DIR "${ZZ_ROOT}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# --- S.3 寻找第三方库 (我们的核心依赖) ---
# 解释: 告诉CMake去系统中寻找已经安装好的OpenCV库。
#       REQUIRED 关键字表示，如果找不到OpenCV，就立即报错停止，因为它是我们的核心依赖。
FIND_PACKAGE(OpenCV REQUIRED)
# 解释: 成功找到后，CMake会创建一个 ${OpenCV_LIBS} 变量，其中包含了链接OpenCV所需的所有库文件信息。

# --- S.4 创建我们的算法动态库 (InspectorLib.dll) ---

# 1. 定义一个库目标
#    add_library: 创建一个库
#    InspectorLib: 我们库的名字
#    SHARED: 【关键】告诉CMake我们要创建的是一个动态共享库 (.dll)
#    源文件: Inspector.cpp 和 Inspector.h
add_library(InspectorLib SHARED Inspector.cpp Inspector.h)

# 2. 添加编译宏定义
#    【关键】这行代码会给 InspectorLib 这个目标添加一个预处理宏 INSPECTOR_LIB_EXPORTS。
#    这会激活我们 Inspector.h 头文件中的 #ifdef 分支，让 ZZ_API 变成 __declspec(dllexport)。
add_compile_definitions(INSPECTOR_EXPORT) 

# 3. 链接依赖库
#    告诉CMake，我们正在创建的 InspectorLib 库本身需要使用 OpenCV 的功能。
TARGET_LINK_LIBRARIES(InspectorLib ${OpenCV_LIBS})


# --- S.5 创建我们的测试程序 (ExampleMain.exe) ---

# 1. 定义一个可执行文件目标
#    add_executable: 创建一个 .exe 程序
#    ExampleMain: 程序的名字
#    源文件: ExampleMain.cpp
add_executable(ExampleMain ExampleMain.cpp)

# 2. 链接依赖库
#    告诉CMake，ExampleMain 这个程序需要链接两个东西：
#    1. ${OpenCV_LIBS}: 因为它需要用 cv::imread, cv::imshow 等函数。
#    2. InspectorLib:  【关键】它需要链接我们刚刚在上面定义的 InspectorLib 库！
#    CMake非常智能，它会自动明白：必须先成功生成 InspectorLib，然后才能去链接 ExampleMain。
TARGET_LINK_LIBRARIES(ExampleMain ${OpenCV_LIBS} InspectorLib)
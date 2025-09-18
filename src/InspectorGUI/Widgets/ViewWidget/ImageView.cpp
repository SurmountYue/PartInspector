// src/InspectorGUI/Widgets/ViewWidget/ImageView.cpp

#include "ImageView.h"
#include "CustomGraphicView.h" // 包含我们内部视图的完整定义

#include <QHBoxLayout> // 使用布局来填满窗口

// --- 构造函数 ---
ImageView::ImageView(QWidget *parent)
    : QWidget(parent)
{
    // 1. 创建我们内部的 CustomGraphicView 实例
    m_graphicView = new CustomGraphicView(this); // this 设置父子关系

    // 2. 创建一个布局
    QHBoxLayout* layout = new QHBoxLayout(this);
    // 3. 将我们的 CustomGraphicView 添加到布局中
    layout->addWidget(m_graphicView);
    // 4. 设置边距为0，让视图完全填满这个 "外壳" Widget
    layout->setContentsMargins(0, 0, 0, 0);

    // 5. 应用布局 (这行可以省略，因为构造时传入了this)
    // this->setLayout(layout);
}

// --- 析构函数 ---
ImageView::~ImageView()
{
    // 因为 m_graphicView 的父亲是 this (ImageView)，
    // 所以当 ImageView 被销毁时，m_graphicView 会被自动销毁。
    // 我们不需要在这里写任何代码。
}


// --- 核心接口实现 ---
void ImageView::setImage(const QImage& image)
{
    // 解释: 这个函数只是一个“传话筒”。
    //       它将收到的 image 对象，直接传递给我们内部的 m_graphicView 去真正地处理和显示。
    //       这就是“封装”的魅力：对外提供简单的接口，隐藏内部的复杂性。
    if (m_graphicView) {
        m_graphicView->setImage(image);
    }
}

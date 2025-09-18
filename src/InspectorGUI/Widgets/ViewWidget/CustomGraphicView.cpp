// src/InspectorGUI/Widgets/ViewWidget/CustomGraphicView.cpp

#include "CustomGraphicView.h"
#include "CustomImageItem.h" // 我们需要包含子控件的完整定义

#include <QWheelEvent>      // 包含鼠标滚轮事件
#include <QMouseEvent>      // 包含鼠标点击事件
#include <QPaintEvent>      // 包含重绘事件
#include <QResizeEvent>     // 包含窗口大小改变事件
#include <QGraphicsScene>   // 包含场景类
#include <QLabel>           // 包含标签类
#include <QHBoxLayout>      // 包含布局类
#include <QPainter>         // 包含绘图类

// --- 定义缩放的上下限 ---
const double ZOOM_MAX_FACTOR = 50.0; // 最大放大50倍
const double ZOOM_MIN_FACTOR = 0.02; // 最小缩小到0.02倍

// --- 构造函数 ---
CustomGraphicView::CustomGraphicView(QWidget *parent)
    : QGraphicsView(parent) // 调用父类构造函数
    , m_scene(nullptr)      // 使用成员初始化列表将指针初始化为安全状态
    , m_imageItem(nullptr)
    , m_pixelInfoLabel(nullptr)
    , m_zoomFactor(1.0)
{
    // 调用我们自己定义的辅助函数来完成所有初始化工作
    setupUi();
}

// --- 析构函数 ---
CustomGraphicView::~CustomGraphicView()
{
    // 因为 m_scene, m_imageItem, m_pixelInfoLabel 都在创建时
    // 设置了 this 作为它们的parent，所以这里我们不需要手动 delete 它们。
    // Qt的父子关系系统会自动处理内存释放。
}

// --- 核心接口：设置图像 ---
void CustomGraphicView::setImage(const QImage &image)
{
    if (image.isNull()) {
        // 如果传入的是一张空图，我们可以选择隐藏图元或显示一张默认图
        m_imageItem->setVisible(false);
        return;
    }

    m_imageItem->setVisible(true);

    // 1. 更新图元的Pixmap
    m_imageItem->setPixmap(QPixmap::fromImage(image));
    // 2. 记录图像的原始尺寸
    m_imageItem->w = image.width();
    m_imageItem->h = image.height();
    // 3. 将图元的位置重置到场景的原点(0,0)
    m_imageItem->setPos(0, 0);
    // 4. 让图像自适应窗口大小并居中显示
    fitImageToView();
}


// --- 事件处理：鼠标滚轮 (实现缩放) ---
void CustomGraphicView::wheelEvent(QWheelEvent *event)
{
    // 获取滚轮滚动的角度。正值代表向上(远离用户)，负值代表向下(朝向用户)。
    const int angle = event->angleDelta().y();
    double scaleFactor = 1.0;

    if (angle > 0) {
        // 向上滚动，放大
        scaleFactor = 1.15;
        if (m_zoomFactor * scaleFactor > ZOOM_MAX_FACTOR) {
            scaleFactor = ZOOM_MAX_FACTOR / m_zoomFactor;
        }
    } else {
        // 向下滚动，缩小
        scaleFactor = 1.0 / 1.15;
        if (m_zoomFactor * scaleFactor < ZOOM_MIN_FACTOR) {
            scaleFactor = ZOOM_MIN_FACTOR / m_zoomFactor;
        }
    }

    // 更新总的缩放系数
    m_zoomFactor *= scaleFactor;
    // 调用 QGraphicsView 内置的 scale 函数，以鼠标当前位置为中心进行缩放
    this->scale(scaleFactor, scaleFactor);
}

// --- 事件处理：鼠标双击 (实现自适应) ---
void CustomGraphicView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 如果是鼠标左键双击，就调用自适应函数
        fitImageToView();
    }
    // 调用父类的同名函数，以确保事件能被继续处理（如果需要）
    QGraphicsView::mouseDoubleClickEvent(event);
}

// --- 事件处理：重绘 (实现背景) ---
void CustomGraphicView::paintEvent(QPaintEvent *event)
{
    // 1. 先绘制我们的棋盘格背景
    QPainter painter(this->viewport());
    painter.drawTiledPixmap(this->viewport()->rect(), m_tilePixmap);

    // 2. 然后调用父类的paintEvent，让它继续绘制场景中的所有图元(我们的图片)
    QGraphicsView::paintEvent(event);
}

// --- 事件处理：窗口大小改变 ---
void CustomGraphicView::resizeEvent(QResizeEvent *event)
{
    // 每次窗口大小改变时，都自动调用一次自适应，确保图片保持最佳显示
    fitImageToView();
    // 调用父类函数
    QGraphicsView::resizeEvent(event);
}


// --- 辅助函数：UI初始化 ---
void CustomGraphicView::setupUi()
{
    // 1. 设置视图(View)的基本属性
    setRenderHint(QPainter::Antialiasing); // 抗锯齿，让图像更平滑
    setDragMode(QGraphicsView::ScrollHandDrag); // 设置鼠标左键拖拽模式为“手掌拖拽”
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // 设置缩放的中心点为鼠标所在位置
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 关闭滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 2. 创建场景(Scene)
    m_scene = new QGraphicsScene(this); // `this` 将view设置为scene的parent
    this->setScene(m_scene); // 将场景设置给视图

    // 3. 创建图片图元(Item)
    m_imageItem = new CustomImageItem();
    m_scene->addItem(m_imageItem); // 将图元添加到场景中

    // 4. 创建并设置左下角的像素信息标签
    m_pixelInfoLabel = new QLabel(this); // `this` 将view设置为label的parent
    m_pixelInfoLabel->setStyleSheet(
        "QLabel { "
        "   background-color: rgba(46, 52, 64, 0.8);" // 半透明深色背景
        "   color: #ECEFF4;"
        "   padding: 4px;"
        "   border-radius: 4px;"
        "}"
        );
    m_pixelInfoLabel->setText(" W:0, H:0 | X:0, Y:0 | R:0, G:0, B:0");
    m_pixelInfoLabel->adjustSize(); // 自动调整大小
    // 将标签放置在左下角 (move的坐标是相对于父控件左上角的)
    m_pixelInfoLabel->move(10, this->height() - m_pixelInfoLabel->height() - 10);

    // 5. 【关键连接】连接“智能图片”的信号和我们标签的槽
    //    当图片发出 pixelInfo 信号时，调用标签的 setText 槽函数来更新文本。
    connect(m_imageItem, &CustomImageItem::pixelInfo, m_pixelInfoLabel, &QLabel::setText);

    // 6. 创建棋盘格背景
    m_tilePixmap = QPixmap(32, 32);
    m_tilePixmap.fill(QColor(0x2E, 0x34, 0x40)); // 深色
    QPainter painter(&m_tilePixmap);
    painter.fillRect(0, 0, 16, 16, QColor(0x3B, 0x42, 0x52)); // 稍亮的方块
    painter.fillRect(16, 16, 16, 16, QColor(0x3B, 0x42, 0x52));
}


// --- 辅助函数：自适应图像 ---
void CustomGraphicView::fitImageToView()
{
    if (!m_imageItem || m_imageItem->pixmap().isNull()) {
        return; // 如果没有图片，则不执行任何操作
    }

    // 重置所有之前的缩放和位移变换
    this->resetTransform();

    // 获取图元(图片)和视图(窗口)的矩形区域
    QRectF itemRect = m_imageItem->boundingRect();
    QRectF viewRect = this->rect();

    if (itemRect.isEmpty()) return;

    // 计算能完整显示图片所需的缩放比例
    double scaleX = viewRect.width() / itemRect.width();
    double scaleY = viewRect.height() / itemRect.height();
    // 取较小的比例，以确保图片能被完整地显示在视图内
    double newScale = std::min(scaleX, scaleY) * 0.98; // 乘以0.98留出一点边距

    // 应用这个新的缩放比例
    this->scale(newScale, newScale);
    // 更新内部的缩放系数记录
    m_zoomFactor = newScale;

    // 将视图的中心对准图片的中心，实现居中显示
    this->centerOn(m_imageItem);
}

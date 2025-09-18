#include "CustomImageItem.h"

#include <QGraphicsSceneHoverEvent> // 包含鼠标悬停事件的头文件
#include <QPainter>               // 虽然这里没用，但自定义绘制通常需要它
#include <QImage>                 // 用于像素颜色值的获取
// --- 构造函数 ---
CustomImageItem::CustomImageItem(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent) // 调用父类的构造函数
{
    // 解释: setAcceptHoverEvents(true) 是一个“开关”。
    //       调用它之后，这个图元(Item)才会开始“感知”鼠标的悬停事件。
    //       如果不调用，下面的 hoverMoveEvent 函数将永远不会被触发。
    this->setAcceptHoverEvents(true);
}

// --- 核心功能：重写鼠标悬停事件处理函数 ---
void CustomImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    // 解释: 当鼠标在这个图元上移动时，这个函数就会被Qt自动调用。
    //       'event' 参数是一个包含了所有事件信息的对象。

    // 1. 获取鼠标在图元坐标系下的位置
    // 函数: event->pos() 返回一个 QPointF 对象，包含了鼠标的 (x, y) 坐标。
    QPointF mousePosition = event->pos();

    // 2. 准备变量来存储颜色和坐标
    int r, g, b; // 用于存储R, G, B颜色分量
    int x = mousePosition.x(); // 获取x坐标
    int y = mousePosition.y(); // 获取y坐标

    // 3. 边界检查 (防御性编程)
    // 解释: 确保坐标不会超出图像的边界，防止程序因访问无效像素而崩溃。
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= w) x = w - 1; // w 是我们在.h文件中定义的图像宽度
    if (y >= h) y = h - 1; // h 是我们在.h文件中定义的图像高度

    // 4. 获取鼠标位置的像素颜色
    // 语法: this->pixmap() 获取当前图元显示的 QPixmap 图像。
    //       .toImage() 将其转换为 QImage (QImage 提供了更方便的像素操作)。
    //       .pixelColor(x, y) 获取指定坐标的颜色。
    //       .getRgb(&r, &g, &b) 将颜色分解为 R, G, B 三个分量。
    this->pixmap().toImage().pixelColor(x, y).getRgb(&r, &g, &b);

    // 5. 格式化要显示的字符串信息
    // 语法: QString::arg() 是一个非常方便的格式化函数，它会按顺序替换掉 %1, %2, ... 这些占位符。
    QString infoText = QString("W:%1, H:%2 | X:%3, Y:%4 | R:%5, G:%6, B:%7")
                           .arg(w)         // %1 -> 图像总宽度
                           .arg(h)         // %2 -> 图像总高度
                           .arg(x)         // %3 -> 鼠标x坐标
                           .arg(y)         // %4 -> 鼠标y坐标
                           .arg(r)         // %5 -> 红色分量
                           .arg(g)         // %6 -> 绿色分量
                           .arg(b);        // %7 -> 蓝色分量

    // 6. 发射信号！
    // 语法: emit 是Qt特有的关键字，用于发射一个信号。
    // 作用: 我们将格式化好的字符串通过 pixelInfo 信号“广播”出去。
    //       任何连接到这个信号的槽函数（我们之后会在CustomGraphicView中连接）都会被触发。
    emit pixelInfo(infoText);
}

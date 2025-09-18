#ifndef CUSTOMIMAGEITEM_H
#define CUSTOMIMAGEITEM_H

#include <QWidget>
#include <QObject>
#include <QGraphicsPixmapItem>

// 解释:
// 我们的 CustomImageItem 类需要同时继承 QObject 和 QGraphicsPixmapItem。
// - 继承 QObject: 是为了能够使用Qt的信号与槽机制 (比如发出一个像素信息的信号)。
// - 继承 QGraphicsPixmapItem: 是为了让它成为一个可以被添加到 QGraphicsScene 中的、能够显示图片的图元(Item)。

class CustomImageItem : public QObject ,public QGraphicsPixmapItem

{
    Q_OBJECT
public:
    //构造函数
    explicit CustomImageItem(QGraphicsItem *parent = nullptr);
    // 用于记录图像的原始尺寸，方便在其他地方引用
    int w = 0;
    int h = 0;

signals:

    // 信号：当鼠标在图片上移动时，我们会发出这个信号。
    // 它会携带一个已经格式化好的字符串，包含了坐标和RGB值。
    void pixelInfo(const QString& info);

protected:
    // 解释: 我们将重写(override)父类的这个虚函数。
    //       当鼠标光标进入或在这个图元上移动时，这个函数就会被Qt自动调用。
    //       我们需要先在构造函数中调用 setAcceptHoverEvents(true) 来激活这个功能。
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
};

#endif // CUSTOMIMAGEITEM_H

#ifndef CUSTOMGRAPHICVIEW_H
#define CUSTOMGRAPHICVIEW_H

#include <QWidget>
#include <QGraphicsView>

// --- 前向声明 ---
// 解释: 我们将在这个类中使用这些类型的指针。
//       在.h文件中使用前向声明，可以加快编译速度
class CustomImageItem;
class QLabel;

class CustomGraphicView : public QGraphicsView
{
    Q_OBJECT // 启用元对象系统
public:
    explicit CustomGraphicView(QWidget *parent = nullptr);
    ~CustomGraphicView();

    //
    // @brief 对外提供的核心接口，用于设置或更新要显示的图像。
    // @param image 要显示的QImage图像。
    //
    void setImage(const QImage &image);
signals:
protected:
    // --- 重写事件处理函数 ---
    // 解释: 我们将重写 QGraphicsView 的这些虚函数，以实现我们自定义的交互逻辑。

    // 当鼠标滚轮滚动时被调用
    virtual void wheelEvent(QWheelEvent *event) override;
    // 当鼠标双击时被调用
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    // 当窗口需要重绘时被调用 (我们将用它来绘制棋盘格背景)
    virtual void paintEvent(QPaintEvent *event) override;
    // 当窗口大小改变时被调用
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    /**
     * @brief 辅助函数，用于初始化所有成员变量和设置。
     */
    void setupUi();

    /**
     * @brief 辅助函数，让图像自适应窗口大小。
     */
    void fitImageToView();

    // --- 成员变量 ---
    QGraphicsScene* m_scene;      // 场景：所有图元的“无限大画板”
    CustomImageItem* m_imageItem; // 我们的“智能图片”图元

    // 用于在左下角显示像素信息的标签
    QLabel* m_pixelInfoLabel;

    // 用于控制缩放的内部变量
    double m_zoomFactor = 1.0;

    // 用于绘制棋盘格背景的Pixmap
    QPixmap m_tilePixmap;
};

#endif // CUSTOMGRAPHICVIEW_H

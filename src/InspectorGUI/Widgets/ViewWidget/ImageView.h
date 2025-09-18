// src/InspectorGUI/Widgets/ViewWidget/ImageView.h

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QImage>   // 需要包含QImage，因为我们的公共接口会用到它

// --- 前向声明 ---
// 我们在这里包含了一个 CustomGraphicView 的指针。
class CustomGraphicView;

class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

    /**
     * @brief [核心公共接口] 设置要在此控件中显示的图像。
     * @param image 要显示的QImage。
     */
    void setImage(const QImage& image);

private:
    // 指向我们内部那个功能强大的视图控件的指针。
    CustomGraphicView* m_graphicView;
};

#endif // IMAGEVIEW_H

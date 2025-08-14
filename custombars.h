#ifndef CUSTOMBARS_H
#define CUSTOMBARS_H

#include "QCustomPlot.h"

// 头文件CustomBars.h
class CustomBars : public QCPBars
{
public:
    explicit CustomBars(QCPAxis *keyAxis, QCPAxis *valueAxis);

    Qt::Alignment textAligment() const { return mTextAlignment; }
    double spacing() const { return mSpacing; }
    QFont font() const { return mFont; }

    void setTextAlignment(Qt::Alignment alignment);
    void setSpacing(double spacing);
    void setFont(const QFont &font);


protected:
    Qt::Alignment mTextAlignment;   // 文字对齐方式
    double mSpacing;                // 文字与柱状图的间距，这里按像素大小
    QFont mFont;                    // 文字使用的字体

    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

};
#endif

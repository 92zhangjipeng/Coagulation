#pragma execution_character_set("utf-8")
#include "custombars.h"
#include "QsLog/include/QsLog.h"

// 源文件CustomBars.cpp
CustomBars::CustomBars(QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPBars (keyAxis, valueAxis),
      mTextAlignment(Qt::AlignCenter),
      mSpacing(5),
      mFont(QFont(QLatin1String("楷体"), 12))
{

}

void CustomBars::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
}

void CustomBars::setSpacing(double spacing)
{
    mSpacing = spacing;
}

void CustomBars::setFont(const QFont &font)
{
    mFont = font;
}

// 源文件CustomBars.cpp
void CustomBars::draw(QCPPainter *painter)
{
    if (!mKeyAxis || !mValueAxis) { QLOG_DEBUG() << Q_FUNC_INFO << "invalid key or value axis"; return; }
	if (mDataContainer->isEmpty()) return;

	QCPBarsDataContainer::const_iterator visibleBegin, visibleEnd;
	getVisibleDataBounds(visibleBegin, visibleEnd);

	// loop over and draw segments of unselected/selected data:
	QList<QCPDataRange> selectedSegments, unselectedSegments, allSegments;
	getDataSegments(selectedSegments, unselectedSegments);
	allSegments << unselectedSegments << selectedSegments;
	for (int i = 0; i < allSegments.size(); ++i)
	{
		bool isSelectedSegment = i >= unselectedSegments.size();
		QCPBarsDataContainer::const_iterator begin = visibleBegin;
		QCPBarsDataContainer::const_iterator end = visibleEnd;
		mDataContainer->limitIteratorsToDataRange(begin, end, allSegments.at(i));
		if (begin == end)
			continue;

		for (QCPBarsDataContainer::const_iterator it = begin; it != end; ++it)
		{
			// draw bar:
			if (isSelectedSegment && mSelectionDecorator)
			{
				mSelectionDecorator->applyBrush(painter);
				mSelectionDecorator->applyPen(painter);
			}
			else
			{
				painter->setBrush(mBrush);
				painter->setPen(mPen);
			}
			applyDefaultAntialiasingHint(painter);
			QRectF barRect = getBarRect(it->key, it->value);   //自己加的
			painter->drawPolygon(barRect);
			// 以上是拷贝的源码部分
			painter->drawPolygon(barRect);

			// 我们仅需在 painter->drawPolygon(barRect); 这行下增加以下的内容即可

			// 计算文字的位置
			painter->setFont(mFont);                     // 设置字体
            QString text = QString::number(it->value);   // 取得当前value轴的值，保留两位精度
            //QLOG_DEBUG()<<"显示数字==="<<it->value<<"ddd=="<<text<<endl;

			QRectF textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, text);  // 计算文字所占用的大小

			if (mKeyAxis.data()->orientation() == Qt::Horizontal) {    // 当key轴为水平轴的时候
				if (mKeyAxis.data()->axisType() == QCPAxis::atTop)     // 上轴，移动文字到柱状图下面
					textRect.moveTopLeft(barRect.bottomLeft() + QPointF(0, mSpacing));
				else                                                   // 下轴，移动文字到柱状图上面
					textRect.moveBottomLeft(barRect.topLeft() - QPointF(0, mSpacing));
				textRect.setWidth(barRect.width());
				painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
			}
			else {                                                  // 当key轴为竖直轴的时候
				if (mKeyAxis.data()->axisType() == QCPAxis::atLeft)   // 左轴，移动文字到柱状图右边
					textRect.moveTopLeft(barRect.topRight() + QPointF(mSpacing, 0));
				else                                                  // 右轴，移动文字到柱状图左边
					textRect.moveTopRight(barRect.topLeft() - QPointF(mSpacing, 0));
				textRect.setHeight(barRect.height());
				painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
			}

		}
	}
}

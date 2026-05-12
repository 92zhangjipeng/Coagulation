#pragma execution_character_set("utf-8")

#include "loginui.h"
#include "pmessagebox.h"
#include "ui_pmessagebox.h"
#include "QsLog/include/QsLog.h"

#include <operclass/fullyautomatedplatelets.h>

PMessageBox::PMessageBox(QWidget *parent, int indexTray) :QDialog(parent),
ui(new Ui::PMessageBox)
{
	ui->setupUi(this);
	bgmcColor.setRgb(230, 230, 230);
	setWindowFlags(Qt::Tool |
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint);
	ui->widget_TrayShow->installEventFilter(this);
	ui->label_titlename->setText(QString("试管盘%1提示").arg(indexTray));

	ui->label_showicon->setFixedSize(32, 32);
	// 修复图片资源管理问题：使用栈对象避免内存泄漏
	QPixmap pixmap(":/Picture/reminderalarm.png");
	if (!pixmap.isNull()) {
		QPixmap scaledPixmap = pixmap.scaled(ui->label_showicon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->label_showicon->setScaledContents(true);
		ui->label_showicon->setPixmap(scaledPixmap);
	}
	else {
		QLOG_WARN() << "Failed to load reminder alarm icon";
	}

	mIndextubeTray = indexTray - 1; //传入的是1开始
	testTubeTrayLastNum(mIndextubeTray);
	QLOG_DEBUG() << "提示测试管盘:" << mIndextubeTray;
}

PMessageBox::~PMessageBox()
{
	QLOG_DEBUG() << "析构试管盘" << mIndextubeTray + 1 << "提示框";
	// 修复内存管理：正确删除动态数组
	if (m_pdelegatesTube) {
		delete[] m_pdelegatesTube;
		m_pdelegatesTube = nullptr;
	}
	delete ui;
}

void PMessageBox::setindexTray(int index_)
{
	QString str = QString("试管盘%1提示").arg(index_);
	QUIUtils::QLabeldisplayIcon(ui->label_titlename, ":/Picture/reminderalarm.png", str, 5);
	mIndextubeTray = index_ - 1;
}

bool PMessageBox::isPartiallyVisible(QDialog* pwidget)
{
	QRect widgetRect = pwidget->rect();
	QPoint globalPos = pwidget->mapToGlobal(QPoint(0, 0));
	QRect screenrect = QApplication::desktop()->screenGeometry(globalPos);
	return screenrect.intersects(widgetRect);
}

void PMessageBox::testTubeTrayLastNum(const int alarmIndex)
{
	//mTrayState.clear();
	QVariantList TubeNumInfo;
	TubeNumInfo.clear();
	int starttube = alarmIndex * ONETRAY_TOTALTUBE;
	int endtube = (alarmIndex + 1)*ONETRAY_TOTALTUBE;

	QVector<int> Tubenumber;
	Tubenumber.clear();
	mnotuseTube.clear();
	for (int i = starttube; i < endtube; i++)
	{
		Tubenumber.push_back(i);
	}

	// 添加错误处理
	if (!FullyAutomatedPlatelets::pinstancesqlData()) {
		QLOG_ERROR() << "SQL data instance is null";
		return;
	}

	FullyAutomatedPlatelets::pinstancesqlData()->FindAllEmptyTube(TubeNumInfo);
	QVariant  signalTube;
	for (int i = 0; i < TubeNumInfo.size(); i++)
	{
		signalTube = TubeNumInfo.at(i);
		AllTubeInfo tempinfo = signalTube.value<AllTubeInfo>();/*将QVariant变成结构体*/
		int numbertube = tempinfo.TubeNumbers;
		if (Tubenumber.contains(numbertube))
		{
			int States = tempinfo.TubeStatus;
			if (States == TESTTUBES_FREETIME)
			{
				mnotuseTube.push_back(numbertube);
			}
		}
	}
	creatTrayPointLoc(ui->widget_TrayShow, mnotuseTube, starttube);
	return;
}

void PMessageBox::creatTrayPointLoc(QWidget *widgetRect, QList<int> &notUsedTubeList, const int indexhole)
{
    int nwidgetH = widgetRect->height();

   // 计算每行LED的实际高度（LED高度30 + 上下边距）
   const int ledHeight = 30;
   const int rowMargin = 10; // 每行上下边距总和
   int rowHeight = ledHeight + rowMargin;

   // 计算可用高度和间距
   int totalRows = 10; // 10行
   int totalLedHeight = totalRows * rowHeight;
   int lastHigh = (nwidgetH - totalLedHeight) / (totalRows - 1);

   // 确保间距不为负数
   if (lastHigh < 0) lastHigh = 0;

   QHBoxLayout *phLayout[10];
   QWidget *pwidget[10];

   for (int i = 0; i < 10; i++)
   {
       pwidget[i] = new QWidget();
       phLayout[i] = new QHBoxLayout();
       phLayout[i]->setSpacing(10);
       phLayout[i]->setContentsMargins(5, 2, 5, 2); // 设置边距
       pwidget[i]->setMinimumHeight(rowHeight);
       pwidget[i]->setMaximumHeight(rowHeight); // 固定高度
   }

   m_pdelegatesTube = new QSimpleLed[ONETRAY_TOTALTUBE]();

   for (int i = 0; i < ONETRAY_TOTALTUBE; i++)
   {
       m_pdelegatesTube[i].setFixedSize(30, 30);
       m_pdelegatesTube[i].setObjectName(QString::number(indexhole + i));
       m_pdelegatesTube[i].setColors(QSimpleLed::CUSTOM);

       if (notUsedTubeList.contains(indexhole + i))
       {
           m_pdelegatesTube[i].setStates(QSimpleLed::LEDSTATES::BLINK);
       }
       else
       {
           m_pdelegatesTube[i].setStates(QSimpleLed::LEDSTATES::OFF);
       }

       int index = m_pdelegatesTube[i].objectName().toInt() - indexhole;
       int rows = index / 6;
       phLayout[rows]->addWidget(&m_pdelegatesTube[i]);

       // 添加调试输出
       qDebug() << "LED" << i << "rows:" << rows << "index:" << index;
   }

   QVBoxLayout *vlayout = new QVBoxLayout;
   vlayout->setSpacing(lastHigh);
   vlayout->setContentsMargins(0, 0, 0, 0);

   for (int k = 0; k < 10; k++)
   {
       pwidget[k]->setLayout(phLayout[k]);
       vlayout->addWidget(pwidget[k]);
   }

   widgetRect->setLayout(vlayout);

   // 调试：输出widgetRect的实际高度
   qDebug() << "widgetRect height:" << widgetRect->height();
   qDebug() << "rowHeight:" << rowHeight << "lastHigh:" << lastHigh;

   update();

   if (notUsedTubeList.size() != 0)
   {
       QString reminderstr = tr("测试杯板,有未使用试杯全部弃用?");
       ui->label_remindertext->setText(reminderstr);
   }
   return;
}

void PMessageBox::mousePressEvent(QMouseEvent *event)
{
	event->accept();
	if (event->button() == Qt::LeftButton)
	{
		mouseStartPoint = event->pos();
	}
}

void PMessageBox::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint delta = event->pos() - mouseStartPoint;
		move(pos() + delta);
	}
}

void PMessageBox::mouseReleaseEvent(QMouseEvent *event)
{
	event->accept();
}

void PMessageBox::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
		QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// 绘制背景
	QRect rect = this->rect();
	painter.fillRect(rect, bgmcColor);

	// 绘制边框
	QPen pen(Qt::gray, 2);
	painter.setPen(pen);
	painter.drawRect(rect.adjusted(1, 1, -1, -1));
}

void PMessageBox::on_toolButton_OK_clicked()
{
	emit _makesureupdateTraytube(mIndextubeTray);
	accept();
}

void PMessageBox::on_toolButton_Cancel_clicked()
{
	emit _cancelupdateTraytube(mIndextubeTray);
	reject();
}

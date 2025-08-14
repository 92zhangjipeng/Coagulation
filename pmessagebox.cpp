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

    ui->label_showicon->setFixedSize(32,32);
    QPixmap *pixmap = new QPixmap(":/Picture/reminderalarm.png");
    pixmap->scaled(ui->label_showicon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_showicon->setScaledContents(true);
    ui->label_showicon->setPixmap(*pixmap);
    delete pixmap;
    pixmap = nullptr;



    mIndextubeTray = indexTray - 1; //传入的是1开始
    testTubeTrayLastNum(mIndextubeTray);
    QLOG_DEBUG()<<"提示测试管盘:"<<mIndextubeTray;
}

PMessageBox::~PMessageBox()
{
    QLOG_DEBUG()<<"析构试管盘"<<mIndextubeTray + 1<<"提示框";
    if(m_pdelegatesTube)
        delete [] m_pdelegatesTube;
    delete ui;
}

void PMessageBox::setindexTray(int index_)
{
    QString str = QString("试管盘%1提示").arg(index_);
    QUIUtils::QLabeldisplayIcon(ui->label_titlename,":/Picture/reminderalarm.png",str, 5);
    mIndextubeTray = index_ - 1;
}

bool PMessageBox::isPartiallyVisible(QDialog* pwidget)
{
    QRect widgetRect = pwidget->rect();
    QPoint globalPos = pwidget->mapToGlobal(QPoint(0,0));
    QRect screenrect = QApplication::desktop()->screenGeometry(globalPos);
    return screenrect.intersects(widgetRect) ;
}

void PMessageBox::testTubeTrayLastNum(const int alarmIndex)
{
	//mTrayState.clear();
    QVariantList TubeNumInfo;
    TubeNumInfo.clear();
    int starttube = alarmIndex * ONETRAY_TOTALTUBE;
    int endtube =  (alarmIndex + 1)*ONETRAY_TOTALTUBE;

    QVector<int> Tubenumber;
    Tubenumber.clear();
    mnotuseTube.clear();
    for(int i = starttube ; i < endtube ; i++)
    {
        Tubenumber.push_back(i);
    }
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllEmptyTube(TubeNumInfo);
    QVariant  signalTube;
    for(int i = 0 ; i < TubeNumInfo.size(); i++)
    {
        signalTube = TubeNumInfo.at(i);
        AllTubeInfo tempinfo = signalTube.value<AllTubeInfo>();/*将QVariant变成结构体*/
        int numbertube = tempinfo.TubeNumbers;
        if(Tubenumber.contains(numbertube))
        {
            int States = tempinfo.TubeStatus;
            if(States == TESTTUBES_FREETIME)
            {
                mnotuseTube.push_back(numbertube);
            }
        }
    }
    creatTrayPointLoc(ui->widget_TrayShow,mnotuseTube,starttube);
    return;
}

void PMessageBox::creatTrayPointLoc(QWidget *widgetRect, QList<int> &notUsedTubeList,const int indexhole)
{
    //int nwidgetW = widgetRect->width();
    int nwidgetH = widgetRect->height();
    int lastHigh = (nwidgetH - 380)/11;
    QHBoxLayout *phLayout[10]; //横布局
    QWidget *pwidget[10]; //每行孔
    for(int i = 0 ; i < 10 ; i++)
    {
        pwidget[i] = new QWidget();
        phLayout[i] = new QHBoxLayout();
        phLayout[i]->setSpacing(10);
        pwidget[i]->setMinimumHeight(40);
    }
    m_pdelegatesTube = new QSimpleLed[ONETRAY_TOTALTUBE]();
    for(int i = 0 ; i < ONETRAY_TOTALTUBE ; i++)
    {
        m_pdelegatesTube[i].setFixedSize(34,34);
        m_pdelegatesTube[i].setObjectName(QString::number(indexhole + i));
        m_pdelegatesTube[i].setColors(QSimpleLed::CUSTOM);
        if(notUsedTubeList.contains(indexhole + i))
        {
            m_pdelegatesTube[i].setStates(QSimpleLed::LEDSTATES::BLINK);
        }
        else
        {
            m_pdelegatesTube[i].setStates(QSimpleLed::LEDSTATES::OFF); //红色已用  绿色可以用
        }
        int index = m_pdelegatesTube[i].objectName().toInt() - indexhole;
        int rows =  index/6;
        phLayout[rows]->addWidget(&m_pdelegatesTube[i]);
    }
    QVBoxLayout *vlayout = new QVBoxLayout;
    for(int k = 0 ; k <10 ;k++)
    {
        pwidget[k]->setLayout(phLayout[k]);
        vlayout->addWidget(pwidget[k]);
    }
    vlayout->setSpacing(lastHigh);
    widgetRect->setLayout(vlayout);
    update();

    if(notUsedTubeList.size() != 0)
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
        QRect desktopRc = QApplication::desktop()->availableGeometry();
        QPoint curPoint = event->globalPos() - mouseStartPoint;
        if (event->globalY() > desktopRc.height())
        {
            curPoint.setY(desktopRc.height() - mouseStartPoint.y());
        }
        move(curPoint);
    }

    QWidget::mouseMoveEvent(event);
}
void PMessageBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}
void PMessageBox::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor colorBackGround = bgmcColor;
    p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setBrush(colorBackGround);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}



void PMessageBox::on_toolButton_OK_clicked()
{
    emit this->_makesureupdateTraytube(mIndextubeTray);
    close();  
}

void PMessageBox::on_toolButton_Cancel_clicked()
{
    emit this->_cancelupdateTraytube(mIndextubeTray);
    close();
}


//void PMessageBox::closeEvent(QCloseEvent *event)
//{
//    event->accept();
//    close();
//    killTimer(mTimeShow);
//    killTimer(mTimeShowSameone);
//    QLOG_TRACE()<<"关闭提示窗口";
//}

#include "select_tube.h"
#include "ui_select_tube.h"
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDebug>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define TUBE_LEFT_SAPCE_X  30
#define TUBE_LEFT_SAPCE_Y  30
#define TUBE_BIG           15
#define TUBE_SMALLE        8

//试管盘间得间距
#define TRAY_SAPCE    60
//每个试管得间距
#define TUBE_SPACE_X  8
#define TUBE_SPACE_Y  6


Select_Tube::Select_Tube(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Select_Tube)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("试管用量"));
    ui->widget->installEventFilter(this);
    ui->widget->setMouseTracking(true);
    m_Tube_Pos.clear();
    m_cover_tube.clear();
    m_sel_tube.clear();
    m_Seltct_item.clear();
    Init_Tube_Pos();
}

Select_Tube::~Select_Tube()
{
    delete ui;
}
void Select_Tube::Init_Tube_Pos()
{
    QPoint Tube_pos;
    QVector<QPoint> Pos_cover;
    int Tray1_end_x = 0;
    int Row = 0 ;
    int Temp_Row = 0;
    for(int n = 0 ; n < 120 ;n++)
    {
        Row = n/12;
        Temp_Row = n%12;
        Pos_cover.clear();
        Tube_pos.setX(TUBE_LEFT_SAPCE_X+((2*Temp_Row+1)*TUBE_BIG)+Temp_Row*TUBE_SPACE_X);
        Tube_pos.setY(TUBE_LEFT_SAPCE_Y+2*Row*TUBE_BIG+TUBE_SPACE_Y*Row);
        if(n == 119) Tray1_end_x = Tube_pos.x();
        m_Tube_Pos.insert(n,Tube_pos);
        QPoint top_left;
        top_left.setY(Tube_pos.y()- TUBE_BIG);
        top_left.setX(Tube_pos.x()- TUBE_BIG);
        QPoint bottom_right;
        bottom_right.setX(Tube_pos.x() + TUBE_BIG );
        bottom_right.setY(Tube_pos.y() + TUBE_BIG);
        Pos_cover<<top_left<<bottom_right;
        m_cover_tube.insert(n,Pos_cover);
    }
    for(int k = 120 ; k<240 ;k++)
    {
        Row = (k-120)/12;
        Temp_Row = (k-120)%12;
        Pos_cover.clear();
        Tube_pos.setX(Tray1_end_x + TRAY_SAPCE+((2*Temp_Row+1)*TUBE_BIG)+Temp_Row*TUBE_SPACE_X);
        Tube_pos.setY(TUBE_LEFT_SAPCE_Y+2*Row*TUBE_BIG+TUBE_SPACE_Y*Row);
        if(k == 239) Tray1_end_x = Tube_pos.x();
        m_Tube_Pos.insert(k,Tube_pos);
        QPoint top_left;
        top_left.setY(Tube_pos.y()- TUBE_BIG);
        top_left.setX(Tube_pos.x()- TUBE_BIG);
        QPoint bottom_right;
        bottom_right.setX(Tube_pos.x() + TUBE_BIG );
        bottom_right.setY(Tube_pos.y() + TUBE_BIG);
        Pos_cover<<top_left<<bottom_right;
        m_cover_tube.insert(k,Pos_cover);
    }
    for(int i = 240 ; i<360 ;i++)
    {
        Row = (i-240)/12;
        Temp_Row = (i-240)%12;
        Pos_cover.clear();
        Tube_pos.setX(Tray1_end_x + TRAY_SAPCE+((2*Temp_Row+1)*TUBE_BIG)+Temp_Row*TUBE_SPACE_X);
        Tube_pos.setY(TUBE_LEFT_SAPCE_Y+2*Row*TUBE_BIG+TUBE_SPACE_Y*Row);
        m_Tube_Pos.insert(i,Tube_pos);
        QPoint top_left;
        top_left.setY(Tube_pos.y()- TUBE_BIG);
        top_left.setX(Tube_pos.x()- TUBE_BIG);
        QPoint bottom_right;
        bottom_right.setX(Tube_pos.x() + TUBE_BIG );
        bottom_right.setY(Tube_pos.y() + TUBE_BIG);
        Pos_cover<<top_left<<bottom_right;
        m_cover_tube.insert(i,Pos_cover);
    }
}
void Select_Tube::Draw_Tube()
{
    QPainter painter(ui->widget);
    QMap<int,QPoint>::iterator  iter;
    for(iter = m_Tube_Pos.begin(); iter != m_Tube_Pos.end();iter++)
    {
        QPalette palette;
        painter.setPen(QColor(0, 160, 230));
        int tube = iter.key();
        if(m_sel_tube.contains(tube))
            palette.setColor(QPalette::Background,Qt::white);
        else
            palette.setColor(QPalette::Background,Qt::green);
        painter.setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿
        painter.setBrush(QBrush(Qt::white,Qt::SolidPattern)); //设置画刷形式
        painter.drawEllipse(iter.value(),TUBE_BIG,TUBE_BIG); //画大圆
        painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
        painter.drawEllipse(iter.value(),TUBE_SMALLE,TUBE_SMALLE); //画小圆
    }
	update();
}
bool Select_Tube::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->widget && event->type() == QEvent::Paint) {
        Draw_Tube();
    }
    return QWidget::eventFilter(watched, event);
}

void Select_Tube::paintEvent(QPaintEvent*)
{
    m_paint =  new QPainter;
    m_paint->begin(this);
  
    m_paint->end();
    delete m_paint;
}
void Select_Tube::SelTube_data(QMap<int,bool> select_item)
{

    if(select_item.size() < 0) return;
    QMap<int,bool>::iterator iter;
    iter = select_item.begin();
    while(iter != select_item.end())
    {
        int hole = iter.key();
        if(!m_sel_tube.contains(hole))
            m_sel_tube.append(hole);
        iter++;
    }
	return;
}
void Select_Tube::mousePressEvent(QMouseEvent *event)
{
    int sel_start = -1;
    QPoint Pos_ = event->pos();
    QMap<int , QVector<QPoint>>::iterator it;
    it = m_cover_tube.begin();
    while (it != m_cover_tube.end() )
    {
        QPoint temp_top_left;
        temp_top_left = it.value().at(0);
        QPoint temp_bottom_right;
        temp_bottom_right = it.value().at(1);
        if(( temp_top_left.x()<Pos_.x() && temp_top_left.y()<Pos_.y() ) &&(Pos_.x() < temp_bottom_right.x() &&  Pos_.y()<temp_bottom_right.y()))
        {
            sel_start = it.key();
            m_Seltct_item.insert(sel_start, true);
			break;  
        }
        it++;
    }
    SelTube_data(m_Seltct_item);
    return;
}

void Select_Tube::on_OK_Sure_clicked()
{
    int start_tube = 0;
    int end_tube = 0;
    int total = m_sel_tube.size();
	qSort(m_sel_tube.begin(), m_sel_tube.end());
    if(total == 2){
      start_tube =  m_sel_tube.at(0);
      end_tube = m_sel_tube.at(1);
    }
    for(int k = start_tube+1; k < end_tube ; k++)
    {
        m_sel_tube.append(k);
    }

    QList<int> Handoff_tube;
    Handoff_tube.clear();
    if(start_tube == 0 && end_tube == 119)
        Handoff_tube.append(1);
    else if(start_tube == 0 && end_tube == 239)
    {
        Handoff_tube.append(1);
        Handoff_tube.append(2);
    }
    else if(start_tube == 0 && end_tube == 359)
    {
        Handoff_tube.append(1);
        Handoff_tube.append(2);
        Handoff_tube.append(3);
    }
    else if(start_tube == 120 && end_tube == 239)
    {
        Handoff_tube.append(2);
    }
    else if(start_tube == 120 && end_tube == 359)
    {
        Handoff_tube.append(2);
        Handoff_tube.append(3);
    }
    else if(start_tube == 240 && end_tube == 359)
    {
        Handoff_tube.append(3);
    }
    emit Synchronous_Tbue(Handoff_tube);
    emit Synchronous_save_Tbue(Handoff_tube);
    m_sel_tube.clear();
    m_Seltct_item.clear();
    close();
    return;
}

void Select_Tube::on_Cancel_clicked()
{
    m_sel_tube.clear();
    m_Seltct_item.clear();
}
void Select_Tube::closeEvent(QCloseEvent *event)
{
    m_sel_tube.clear();
    m_Seltct_item.clear();
    event->accept();
}

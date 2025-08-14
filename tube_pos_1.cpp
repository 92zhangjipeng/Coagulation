#include "tube_pos_1.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif


#define TRAY_HOLE_BEGIN 120
#define TRAY_HOLE_END   240

Tube_Pos_1::Tube_Pos_1(QWidget *parent) : QWidget(parent)
{
	m_CupPos.clear();

    m_Tube_Selectused_black.clear();

    m_font.setFamily("新宋体");

    m_font.setPixelSize(TEXT_SIZE);

    m_font.setLetterSpacing(QFont::AbsoluteSpacing, 1);// 设置字符间距

    InitPos(true);

    this->setMinimumSize(1100,1100);
}
Tube_Pos_1::~Tube_Pos_1()
{
    delete m_paint;

    m_paint = nullptr;
}
void Tube_Pos_1::paintEvent(QPaintEvent*)
{
    if(m_paint == nullptr)
        m_paint =  new QPainter;

    m_paint->begin(this);

    m_paint->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);

    Init_All_tube_hole(m_paint);

    Selected_use_Tube(m_paint);

    BloodMove2TubeColor(m_paint);//待测试管

    m_paint->end();
}
void Tube_Pos_1::InitPos(bool openfirst)
{
    QPointF Temp;
    for(int n = TRAY_HOLE_BEGIN ; n < TRAY_HOLE_END ; n++)
    {
        int pos = n - 120;

        int Row = pos/12;

        int Temp_Row = pos%12;

        Temp.setX(CUP_SPACE_X_0_1+((2*Temp_Row+1)*CUP_RADIUS_BIG)+Temp_Row*CUP_SPACE_CUP_X);

        Temp.setY(CUP_SPACE_Y+2*Row*CUP_RADIUS_BIG+CUP_SPACE_CUP_Y*Row);

        m_CupPos.insert(n,Temp);
    }
    //读取保存的孔号
    if(openfirst)
    {
        int lastHole = INI_File().GetTrayTubeB().toInt();

        QMap<int ,QPointF>::iterator it;

        for(int i = TRAY_HOLE_BEGIN ; i<lastHole; i++)
        {
            it = m_CupPos.find(i);

            m_Tube_Selectused.insert(it.key(),it.value());

            m_CupPos.remove(it.key());
        }
    }
    return;
}
void Tube_Pos_1::Initialize(bool openfirst)
{
    m_CupPos.clear();

    InitPos(openfirst);

    m_Tube_Selectused.clear();

    m_Tube_Selectused_black.clear();

    return;
}

void Tube_Pos_1::SaveHoleIniFile()
{
    INI_File().SetTrayTubeB(QString("%1").arg(TRAY_HOLE_BEGIN + m_Tube_Selectused_black.size()));
}

void Tube_Pos_1::InitSet_TubeUsedNum(quint32 &use_hole)
{
    use_hole = m_Tube_Selectused.size();
}

void Tube_Pos_1::GetUsedHole_test_tray(QMap<int, QPointF> &tray_hole)
{
    QMap<int, QPointF>::iterator it = m_CupPos.begin();

    while(it != m_CupPos.end())
    {
        tray_hole.insert(it.key(),it.value());

        it++;
    }

    return;
}

void Tube_Pos_1::Selected_used_Tube(QList<quint32> Hole)
{
    QMap<int, QPointF>::iterator iter;

    for(int i = 0 ; i < Hole.count(); i++)
    {
        if(m_CupPos.contains(Hole.at(i)))
        {
            iter = m_CupPos.find(Hole.at(i));

            m_Tube_Selectused.insert(iter.key(),iter.value());

            m_CupPos.remove(iter.key());
        }
    }
    return;
}

void Tube_Pos_1::Selected_used_Tube(quint32 Hole)
{
    QMap<int ,QPointF>::iterator iter;

    if(m_Tube_Selectused.contains(Hole))
    {
        iter = m_Tube_Selectused.find(Hole);

        m_Tube_Selectused_black.insert(iter.key(),iter.value());

        m_Tube_Selectused.remove(iter.key());
    }
    return;
}


/***************   初始颜色 ---      *********************/
void Tube_Pos_1::Init_All_tube_hole(QPainter *painter)
{
    QMap<int,QPointF>::iterator  iter;

    for(iter = m_CupPos.begin(); iter != m_CupPos.end();iter++)
    {
        QPalette palette;

        painter->setPen(cglobal::g_LineColor);

        palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor);

        //painter->setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿

        painter->setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式

        painter->drawEllipse(iter.value(),CUP_RADIUS_BIG,CUP_RADIUS_BIG); //画大圆

        painter->setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色

        painter->drawEllipse(iter.value(),CUP_RADIUS_SMALL,CUP_RADIUS_SMALL); //画小圆
    }
    update();
}

/***************   选中颜色 ---     *********************/
void Tube_Pos_1::Selected_use_Tube(QPainter *painter)
{
    QMap<int,QPointF>::iterator  iter;

    for(iter = m_Tube_Selectused.begin(); iter != m_Tube_Selectused.end();iter++)
    {
        QPalette palette;

        painter->setPen(cglobal::g_LineColor);

        palette.setColor(QPalette::Background,cglobal::g_SelectColor);

        //painter->setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿

        painter->setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式

        painter->drawEllipse(iter.value(),CUP_RADIUS_BIG,CUP_RADIUS_BIG); //画大圆

        painter->setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色

        painter->drawEllipse(iter.value(),CUP_RADIUS_SMALL,CUP_RADIUS_SMALL); //画小圆
    }
   update();
}

/***************   血样移动到试管颜色    *********************/
void Tube_Pos_1::BloodMove2TubeColor(QPainter *painter)
{
    QMap<int,QPointF>::iterator  iter;
    for(iter = m_Tube_Selectused_black.begin(); iter != m_Tube_Selectused_black.end();iter++)
    {
        QPalette palette;

        painter->setPen(cglobal::g_LineColor);

        palette.setColor(QPalette::Background,cglobal::g_MoveedColor);

        //painter->setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿

        painter->setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式

        painter->drawEllipse(iter.value(),CUP_RADIUS_BIG,CUP_RADIUS_BIG); //画大圆

        painter->setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色

        painter->drawEllipse(iter.value(),CUP_RADIUS_SMALL,CUP_RADIUS_SMALL); //画小圆
    }
    update();
}


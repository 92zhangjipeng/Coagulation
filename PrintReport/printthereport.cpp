#pragma execution_character_set("utf-8")
#include "printthereport.h"
#include <QFile>
#include <QPainter>
#include <QPdfWriter>
#include <QWidget>
#include <qt_windows.h>
#include "QsLog/include/QsLog.h"
#include <QDesktopServices>
#include <QDir>
#include <customcreatsql.h>
#include <quiutils.h>
#include <operclass/fullyautomatedplatelets.h>



Printthereport::Printthereport(QObject *parent) : QObject(parent)
{
    moveToThread(&m_thread);

    initpara();

    textEdit = new QTextEdit();

    QObject::connect(&m_thread,&QThread::started,this,&Printthereport::_threadruningstart);


}

Printthereport::~Printthereport()
{
    disconnect();// 断开所有信号槽连接

    if(m_thread.isRunning())
    {
        m_thread.quit();
        m_thread.wait();
    }
    if(textEdit){
        delete textEdit;
        textEdit = nullptr;
    }
}

void Printthereport::_Start()
{
    if(!m_thread.isRunning())
    {
        m_thread.start();
    }
}


void Printthereport::_threadruningstart()
{
     QLOG_DEBUG()<<"Printthereport"<<QThread::currentThreadId();
}


void Printthereport::CreateFolder(QString folderPath)
{
    // 判断文件夹是否存在，不存在则创建
    QDir dir(folderPath);
    if (!dir.exists())
    {
        bool ismkdir = QDir().mkdir(folderPath);
        if(!ismkdir)
        {
            QLOG_WARN()<<"创建文件夹失败";
        }
        else
        {
            QLOG_DEBUG()<<"创建文件夹成功";
        }
    }
    else
    {
        QLOG_TRACE()<<"PDF文件夹已存在";
    }
}


void Printthereport::InsertReagentTestData(quint8 IndexReagent,QStringList ResultData,QStringList &liststr,const bool Sexman,QString &unitStr)
{
    QVariant HeighValue,LowValue;

    QList<double> testValuelist;
    testValuelist.clear();
    int index =  0 ;
    QString  offset;
    //去除%
    for(QString datatest : ResultData)
    {
        QString tmp = datatest;
        QUIUtils::parseDataratio(tmp);
        testValuelist.push_back(tmp.toDouble());
    }
    index = testValuelist.size();
    for(double valuetest : testValuelist)
    {
        quint8 paraType = (IndexReagent*4) - index;
        FullyAutomatedPlatelets::pinstancesqlData()->synchronizeStandardValues(paraType,Sexman,HeighValue,LowValue,unitStr);
        if(valuetest >= HeighValue.toDouble())
        {
            offset = "↑" ;
        }
        else if(valuetest <= LowValue.toDouble())
        {
             offset = "↓" ;
        }
        else if(valuetest > LowValue.toDouble() && valuetest < HeighValue.toDouble())
        {
           offset = "";
        }
        QString outPutResult = QString("%1%%2").arg(valuetest).arg(offset);
        liststr.push_back(outPutResult);
        index--;
    }

}

QPoint Printthereport::switchReagentName(QPainter *pPainter, quint8 indexReag,quint8 n,
                                        int textbottomy, int width, int height,int spacesize)
{
    QString reagname = "";
    switch(indexReag)
    {
        case AA_REAGENT:  reagname =  tr("AA");   break;
        case ADP_REAGENT: reagname = tr("ADP"); break;
        case EPI_REAGENT: reagname=  tr("EPI"); break;
        case COL_REAGENT: reagname = tr("COL"); break;
        case RIS_REAGENT: reagname = tr("RIS"); break;
    default:
        reagname = ("null");
        break;
    }
    QPoint topleftReagname(spacesize,textbottomy + n*height);
    QPoint bottomRightReagNmae(spacesize + width,textbottomy +(n+1)*height);
    pPainter->drawText(QRect(topleftReagname,bottomRightReagNmae),Qt::AlignVCenter|Qt::AlignLeft,reagname);
    return bottomRightReagNmae;
}

void Printthereport::fullintopdfreagresult(QPainter *pPainter, QString resultreagdata, int k, QPoint bottompos, int itemWidth,
                                               int itemHeight)
{
    QPoint topleftResultsec;
    QPoint bottomRightresultsec;
    topleftResultsec.setX(bottompos.x() + k*itemWidth);
    topleftResultsec.setY(bottompos.y() - itemHeight);
    bottomRightresultsec.setX(bottompos.x() + (k+1)*itemWidth);
    bottomRightresultsec.setY(bottompos.y());
    pPainter->drawText(QRect(topleftResultsec,bottomRightresultsec),Qt::AlignVCenter|Qt::AlignLeft,resultreagdata);
    return;
}

void Printthereport::intsertwriteunit(QPainter *pPainter,QPoint bottompos,int itemWidth,int itemHeight)//单位
{
    QPoint topleftunit(bottompos.x() + 5*itemWidth,bottompos.y() - itemHeight);
    QPoint bottomRightunit(bottompos.x() + 6*itemWidth,bottompos.y());
    pPainter->drawText(QRect(topleftunit,bottomRightunit),Qt::AlignVCenter|Qt::AlignLeft,"S/%");
    return;
}

void Printthereport::intsertReferencevalues(QPainter *pPainter,QPoint bottompos,int itemWidth, int itemHeight,bool mansex)//参考值
{
    QString outReferencevalues;
    QPoint topleftReferencevalues(bottompos.x() + 6*itemWidth,bottompos.y() - itemHeight);
    QPoint bottomRightReferencevalues(bottompos.x() + 7*itemWidth,bottompos.y());
    if(mansex)
        outReferencevalues = "80-81%";
    else
        outReferencevalues = "60-78%";
    pPainter->drawText(QRect(topleftReferencevalues,bottomRightReferencevalues),Qt::AlignVCenter|Qt::AlignLeft,outReferencevalues);
    return;
}

void Printthereport::insertwriteoffset(QPainter *pPainter,QPoint bottompos,int itemWidth, int itemHeight,QString uporduwn)
{
    QPoint topleftoffset(bottompos.x() + 4*itemWidth,bottompos.y() - itemHeight);
    QPoint bottomRightoffset(bottompos.x() + 5*itemWidth,bottompos.y());
    pPainter->drawText(QRect(topleftoffset,bottomRightoffset),Qt::AlignVCenter|Qt::AlignLeft,uporduwn);
    return;
}


void Printthereport::slotprintoutresult()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPagedPaintDevice::A4);
    printer.setResolution(300);
    printer.setPageMargins(QMarginsF(30,30,30,30));
    QPrintDialog dialog(&printer, nullptr);
    if (dialog.exec() != QDialog::Accepted)
        return;
    printDocument(&printer);
    return;
}


void Printthereport::slotwritePdf(QString pathload)
{
    CreatPdfFileLayout(pathload);
    return;
}


void Printthereport::printDocument(QPrinter *printer)
{
    QString _hospitalName = FullyAutomatedPlatelets::pinstancesqlData()->FindPassword("hospital_name");

    int spacing_ = 30;

    QPainter *pPainter = new QPainter(printer);
    //pPainter->begin(printer);
    pPainter->setRenderHint(QPainter::Antialiasing);//抗锯齿
    int nPDFWidth =  pPainter->viewport().width()  - spacing_*2;
    int nPDFHeight = pPainter->viewport().height() - spacing_*2;
    QFont font_title;
    pPainter->setPen(Qt::black);
    pPainter->setFont(font_title);
    font_title.setPointSize(16);
    //标题
    int title_heigh = 100;
    pPainter->drawText(QRect(spacing_,spacing_,nPDFWidth,title_heigh),Qt::AlignCenter,QString("%1血小板聚集测试报告").arg(_hospitalName));

    //打印时间
    font_title.setPointSize(6);
    pPainter->setFont(font_title);
    int printTimeHeigh = 40;
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    pPainter->drawText(QRect(spacing_,title_heigh+spacing_,nPDFWidth,printTimeHeigh),Qt::AlignRight, QString("打印时间:%1").arg(dateTime));

    //画横线
    int linewidth = 2;
    pPainter->setPen(QPen(QBrush(QColor(0,0,0)),linewidth));
    pPainter->drawLine(spacing_,title_heigh + printTimeHeigh+ spacing_+linewidth,nPDFWidth,title_heigh + spacing_+linewidth + printTimeHeigh);
    pPainter->drawLine(spacing_,title_heigh + printTimeHeigh + spacing_+ linewidth*3,nPDFWidth,title_heigh + spacing_+linewidth*3 + printTimeHeigh);

    //信息内容
    int topinfoy = title_heigh + printTimeHeigh + spacing_+ linewidth*3 + 5;
    int infoWidth = nPDFWidth/5;
    int infoHeight = 80; //信息高度
    font_title.setPointSize(8);
    pPainter->setFont(font_title);

    QPoint topLeft(spacing_ + infoWidth*0,topinfoy);
    QPoint bottomRight(spacing_ + infoWidth*0 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeft,bottomRight),Qt::AlignVCenter|Qt::AlignLeft,"样本号: " + m_sampleid);

    QPoint topLeftName(bottomRight.x(),topinfoy);
    QPoint botttomRightName(spacing_ + infoWidth*1 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftName,botttomRightName),Qt::AlignVCenter|Qt::AlignLeft,"姓名: " + m_samplename);

    QPoint topLeftSex(botttomRightName.x(),topinfoy);
    QPoint bottomRightSex(spacing_ + infoWidth*2 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftSex,bottomRightSex),Qt::AlignVCenter|Qt::AlignLeft,"性别: "+ m_sampleSex);


    QPoint topLeftBednum(bottomRightSex.x(),topinfoy);
    QPoint bottomRightbednum(spacing_ + infoWidth*3 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftBednum,bottomRightbednum),Qt::AlignVCenter|Qt::AlignLeft,"床号: " + m_sampleBednum);


    QPoint topLeftAges(bottomRightbednum.x(),topinfoy);
    QPoint bottomRightAges(spacing_ + infoWidth*4 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftAges,bottomRightAges),Qt::AlignVCenter|Qt::AlignLeft,"年龄: "+ QString::number(m_sampleAges));


    QPoint topLeftdevp(spacing_ + infoWidth*0,topinfoy + infoHeight);
    QPoint bottomRightdevp(spacing_ + infoWidth*1,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftdevp,bottomRightdevp),Qt::AlignVCenter|Qt::AlignLeft,"科室: " + m_sampledepartment);

    QPoint topLeftSendDor(bottomRightdevp.x(),topinfoy + infoHeight);
    QPoint bottomRightSendDor(spacing_ + infoWidth*2,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftSendDor,bottomRightSendDor),Qt::AlignVCenter|Qt::AlignLeft, "送检医生: "+ m_refertithedoctor);

    QPoint topLeftViewDor(bottomRightSendDor.x(),topinfoy + infoHeight);
    QPoint bottomRightViewDor(spacing_ + infoWidth*3,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftViewDor,bottomRightViewDor),Qt::AlignVCenter|Qt::AlignLeft,"审核医生: "+ m_reviewdoctors);

    QPoint topLeftcodebar(bottomRightViewDor.x(),topinfoy + infoHeight);
    QPoint bottomRightcodebar(spacing_ + infoWidth*5,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftcodebar,bottomRightcodebar),Qt::AlignVCenter|Qt::AlignLeft,"条形码: "+ m_samplebarcode);

    //在画一条横线
    QPoint setrtLine_(spacing_,topinfoy + infoHeight*2 + 10);
    QPoint endLine_(nPDFWidth,topinfoy + infoHeight*2 +  10);
    pPainter->drawLine(setrtLine_,endLine_);

    font_title.setPointSize(12);
    font_title.setBold(true);
    pPainter->setFont(font_title);
    int text_height = 100;
    QPoint text_topleft(spacing_,endLine_.y());
    QPoint text_bottomRigh(nPDFWidth, endLine_.y() + text_height);
    pPainter->drawText(QRect(text_topleft,text_bottomRigh),Qt::AlignVCenter|Qt::AlignLeft,"测试结果");

    //测试结果
    int resultBottom = 0;
    font_title.setPointSize(8);
    pPainter->setFont(font_title);
    QStringList ItemResultView;
    ItemResultView<<"测试项目"<<"60S"<<"180S"<<"300S"<<"Max"<<"偏移"<<"单位"<<"参考值";
    int itemWidth = nPDFWidth/ItemResultView.size();
    int itemHeight = 100;
    for(int n = 0; n < ItemResultView.size();n++ )
    {
        QString resultitem = ItemResultView.at(n);
        QPoint itemtopleft(spacing_ + itemWidth*n,text_bottomRigh.y());
        QPoint itembottomRight(spacing_ + itemWidth*(n+1) ,text_bottomRigh.y() + itemHeight);
        if(resultBottom == 0)
            resultBottom = text_bottomRigh.y() + itemHeight;
        pPainter->drawText(QRect(itemtopleft,itembottomRight),Qt::AlignVCenter|Qt::AlignLeft,resultitem);
    }

    bool mansex = false;
    (m_sampleSex == "男")? mansex = true : mansex = false;

    QMap<quint8,QStringList> resultdata;
    resultdata.clear();

    QMap<quint8,QString> reagResultCurvedata = m_testReagResult.value<QMap<quint8,QString>>();
    switchreagtestdata(reagResultCurvedata,resultdata);

    int nrows = 0;
    QPoint bottomInfopos;
    auto it = resultdata.begin();
    while(it != resultdata.end())
    {
        QPoint bottomy = switchReagentName(pPainter,it.key(),nrows,resultBottom,itemWidth,itemHeight,spacing_); //试剂名称
        QStringList reagresult_ = it.value();
        for(int k = 0; k < reagresult_.size(); k++)
        {
            QString resultreagdata = reagresult_.at(k);
            fullintopdfreagresult(pPainter,resultreagdata,k,bottomy,itemWidth,itemHeight);  //写入测试结果
        }

        insertwriteoffset(pPainter,bottomy,itemWidth,itemHeight,"↑"); //偏移

        intsertwriteunit(pPainter,bottomy,itemWidth,itemHeight); //单位

        intsertReferencevalues(pPainter,bottomy,itemWidth,itemHeight,mansex);//参考值

        bottomInfopos = bottomy;

        it++;
        nrows++;
    }
    //测试曲线
    int toptocurve = bottomInfopos.y();
    font_title.setPointSize(12);
    font_title.setBold(true);
    pPainter->setFont(font_title);
    QPoint topleftcurveText(spacing_,toptocurve);
    QPoint bottomrightcurveText(nPDFWidth,toptocurve + itemHeight);
    pPainter->drawText(QRect(topleftcurveText,bottomrightcurveText),Qt::AlignVCenter|Qt::AlignLeft,"测试曲线");

    int curveAdnremarkheight = nPDFHeight - text_bottomRigh.y() - itemHeight; //曲线图和备注的高度
    int curvegheight = curveAdnremarkheight/3;

    m_pixmap = m_pixmap.scaled(nPDFWidth ,curvegheight,Qt::IgnoreAspectRatio);
    QPoint topleftcurve(spacing_,bottomrightcurveText.y());
    QPoint bottomrightcurve(nPDFWidth,bottomrightcurveText.y() + curvegheight);
    pPainter->drawPixmap(QRect(topleftcurve,bottomrightcurve),m_pixmap);

    //备注
    setrtLine_.setX(spacing_);
    setrtLine_.setY(bottomrightcurve.y());
    endLine_.setX(nPDFWidth);
    endLine_.setY(bottomrightcurve.y());
    pPainter->drawLine(setrtLine_,endLine_);

    QPoint topleftremarkText(spacing_,endLine_.y());
    QPoint bottomrightremarkheightText(nPDFWidth,endLine_.y()+itemHeight);
    pPainter->drawText(QRect(topleftremarkText,bottomrightremarkheightText),Qt::AlignVCenter|Qt::AlignLeft,"备注:");

    //int remark_height = nPDFHeight - bottomrightremarkheightText.y();
    QPoint topleftremark(spacing_,bottomrightremarkheightText.y());
    QPoint bottomrightremarkheight(nPDFWidth,nPDFHeight);

    pPainter->drawText(QRect(topleftremark,bottomrightremarkheight),Qt::AlignVCenter|Qt::AlignLeft|Qt::AlignTop,"医生备注标签提示信息或者其它");


    pPainter->restore();
    pPainter->end();
    delete pPainter;


    return;
}



//构造PDF模板布局 输出PDF
void Printthereport::CreatPdfFileLayout(const QString& file_path)
{
    QFile pdfFile(file_path);
    if(!pdfFile.open(QIODevice::WriteOnly)){
        QLOG_WARN() << "无法打开文件:" << file_path;
        return;
    }

    QString _hospitalName = FullyAutomatedPlatelets::pinstancesqlData()->FindPassword("hospital_name");

    QPdfWriter *pWriter = new QPdfWriter(&pdfFile);
    pWriter->setPageSize(QPagedPaintDevice::A4);

    const int kPageMargin  = 20; // 统一边距
    const int kTitleFontSize = 16;    // 标题字体
    const int kSmallFontSize = 6;     // 小号字体
    const int kBodyFontSize = 8;      // 正文字体

    pWriter->setResolution(300);//设置分辨率 屏幕分辨率 打印机分辨率 高分辨率
    pWriter->setPageMargins(QMarginsF(kPageMargin,kPageMargin,
                                      kPageMargin,kPageMargin));


    QPainter *pPainter = new QPainter(pWriter);
    pPainter->setRenderHint(QPainter::Antialiasing);//抗锯齿
    int nPDFWidth =  pPainter->viewport().width()  - kPageMargin*2;
    int nPDFHeight = pPainter->viewport().height() - kPageMargin*2;
    QFont font_title;
    pPainter->setPen(Qt::black);
    pPainter->setFont(font_title);
    font_title.setPointSize(kTitleFontSize);
    //标题
    const int kTitleHeight  = 100;
    pPainter->drawText(QRect(kPageMargin,kPageMargin,nPDFWidth,kTitleHeight ),Qt::AlignCenter,
                       QString("%1血小板聚集测试报告").arg(_hospitalName));

    //打印时间
    font_title.setPointSize(kSmallFontSize);
    pPainter->setFont(font_title);
    int printTimeHeigh = 25;
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    pPainter->drawText(QRect(kPageMargin,kTitleHeight +kPageMargin,nPDFWidth,printTimeHeigh),
                       Qt::AlignRight, QString("打印时间:%1").arg(dateTime));

    //画横线
    const int kLineWidth  = 2;
    pPainter->setPen(QPen(QBrush(QColor(0,0,0)),kLineWidth ));
    pPainter->drawLine(kPageMargin,kTitleHeight  + printTimeHeigh+ kPageMargin+kLineWidth ,
                       nPDFWidth,kTitleHeight  + kPageMargin+kLineWidth  + printTimeHeigh);
    pPainter->drawLine(kPageMargin,kTitleHeight  + printTimeHeigh + kPageMargin+ kLineWidth *3,
                       nPDFWidth,kTitleHeight  + kPageMargin+kLineWidth *3 + printTimeHeigh);

    //信息内容
    int topinfoy = kTitleHeight  + printTimeHeigh + kPageMargin+ kLineWidth *3 + 5;
    int infoWidth = nPDFWidth/5;
    const int infoHeight = 80; //信息高度
    font_title.setPointSize(kBodyFontSize);
    pPainter->setFont(font_title);

    QPoint topLeft(kPageMargin + infoWidth*0,topinfoy);
    QPoint bottomRight(kPageMargin + infoWidth*0 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeft,bottomRight),Qt::AlignVCenter|Qt::AlignLeft,"样本号: " + m_sampleid);

    QPoint topLeftName(bottomRight.x(),topinfoy);
    QPoint botttomRightName(kPageMargin + infoWidth*1 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftName,botttomRightName),Qt::AlignVCenter|Qt::AlignLeft,"姓名: " + m_samplename);

    QPoint topLeftSex(botttomRightName.x(),topinfoy);
    QPoint bottomRightSex(kPageMargin + infoWidth*2 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftSex,bottomRightSex),Qt::AlignVCenter|Qt::AlignLeft,"性别: "+ m_sampleSex);


    QPoint topLeftBednum(bottomRightSex.x(),topinfoy);
    QPoint bottomRightbednum(kPageMargin + infoWidth*3 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftBednum,bottomRightbednum),Qt::AlignVCenter|Qt::AlignLeft,"床号: " + m_sampleBednum);


    QPoint topLeftAges(bottomRightbednum.x(),topinfoy);
    QPoint bottomRightAges(kPageMargin + infoWidth*4 + infoWidth,topinfoy + infoHeight);
    pPainter->drawText(QRect(topLeftAges,bottomRightAges),Qt::AlignVCenter|Qt::AlignLeft,"年龄: "+ QString::number(m_sampleAges));


    QPoint topLeftdevp(kPageMargin + infoWidth*0,topinfoy + infoHeight);
    QPoint bottomRightdevp(kPageMargin + infoWidth*1,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftdevp,bottomRightdevp),Qt::AlignVCenter|Qt::AlignLeft,"科室: " + m_sampledepartment);

    QPoint topLeftSendDor(bottomRightdevp.x(),topinfoy + infoHeight);
    QPoint bottomRightSendDor(kPageMargin + infoWidth*2,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftSendDor,bottomRightSendDor),Qt::AlignVCenter|Qt::AlignLeft, "送检医生: "+ m_refertithedoctor);

    QPoint topLeftViewDor(bottomRightSendDor.x(),topinfoy + infoHeight);
    QPoint bottomRightViewDor(kPageMargin + infoWidth*3,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftViewDor,bottomRightViewDor),Qt::AlignVCenter|Qt::AlignLeft,"审核医生: "+ m_reviewdoctors);

    QPoint topLeftcodebar(bottomRightViewDor.x(),topinfoy + infoHeight);
    QPoint bottomRightcodebar(kPageMargin + infoWidth*5,topinfoy + infoHeight*2);
    pPainter->drawText(QRect(topLeftcodebar,bottomRightcodebar),Qt::AlignVCenter|Qt::AlignLeft,"条形码: "+ m_samplebarcode);

    //在画一条横线
    QPoint setrtLine_(kPageMargin,topinfoy + infoHeight*2 + 10);
    QPoint endLine_(nPDFWidth,topinfoy + infoHeight*2 +  10);
    pPainter->drawLine(setrtLine_,endLine_);

    font_title.setPointSize(12);
    font_title.setBold(true);
    pPainter->setFont(font_title);
    int text_height = 100;
    QPoint text_topleft(kPageMargin,endLine_.y());
    QPoint text_bottomRigh(nPDFWidth, endLine_.y() + text_height);
    pPainter->drawText(QRect(text_topleft,text_bottomRigh),Qt::AlignVCenter|Qt::AlignLeft,"测试结果");

    //测试结果
    int resultBottom = 0;
    font_title.setPointSize(8);
    pPainter->setFont(font_title);
    QStringList ItemResultView;
    ItemResultView<<"测试项目"<<"60S"<<"180S"<<"300S"<<"Max"<<"偏移"<<"单位"<<"参考值";
    int itemWidth = nPDFWidth/ItemResultView.size();
    int itemHeight = 100;
    for(int n = 0; n < ItemResultView.size();n++ )
    {
        QString resultitem = ItemResultView.at(n);
        QPoint itemtopleft(kPageMargin + itemWidth*n,text_bottomRigh.y());
        QPoint itembottomRight(kPageMargin + itemWidth*(n+1) ,text_bottomRigh.y() + itemHeight);
        if(resultBottom == 0)
            resultBottom = text_bottomRigh.y() + itemHeight;
        pPainter->drawText(QRect(itemtopleft,itembottomRight),Qt::AlignVCenter|Qt::AlignLeft,resultitem);
    }

    bool mansex = false;
    (m_sampleSex == "男")? mansex = true : mansex = false;

    QMap<quint8,QStringList> resultdata;
    resultdata.clear();

    QMap<quint8,QString> reagResultCurvedata = m_testReagResult.value<QMap<quint8,QString>>();
    switchreagtestdata(reagResultCurvedata,resultdata);

    int nrows = 0;
    QPoint bottomInfopos;
    auto it = resultdata.begin();
    while(it != resultdata.end())
    {
        QPoint bottomy = switchReagentName(pPainter,it.key(),nrows,resultBottom,itemWidth,itemHeight,kPageMargin); //试剂名称
        QStringList reagresult_ = it.value();
        for(int k = 0; k < reagresult_.size(); k++)
        {
            QString resultreagdata = reagresult_.at(k);
            fullintopdfreagresult(pPainter,resultreagdata,k,bottomy,itemWidth,itemHeight);  //写入测试结果
        }

        insertwriteoffset(pPainter,bottomy,itemWidth,itemHeight,"↑"); //偏移

        intsertwriteunit(pPainter,bottomy,itemWidth,itemHeight); //单位

        intsertReferencevalues(pPainter,bottomy,itemWidth,itemHeight,mansex);//参考值

        bottomInfopos = bottomy;

        it++;
        nrows++;
    }
    //测试曲线
    int toptocurve = bottomInfopos.y();
    font_title.setPointSize(12);
    font_title.setBold(true);
    pPainter->setFont(font_title);
    QPoint topleftcurveText(kPageMargin,toptocurve);
    QPoint bottomrightcurveText(nPDFWidth,toptocurve + itemHeight);
    pPainter->drawText(QRect(topleftcurveText,bottomrightcurveText),Qt::AlignVCenter|Qt::AlignLeft,"测试曲线");

    int curveAdnremarkheight = nPDFHeight - text_bottomRigh.y() - itemHeight; //曲线图和备注的高度
    int curvegheight = curveAdnremarkheight/3;

    m_pixmap = m_pixmap.scaled(nPDFWidth ,curvegheight,Qt::IgnoreAspectRatio);
    QPoint topleftcurve(kPageMargin,bottomrightcurveText.y());
    QPoint bottomrightcurve(nPDFWidth,bottomrightcurveText.y() + curvegheight);
    pPainter->drawPixmap(QRect(topleftcurve,bottomrightcurve),m_pixmap);

    //备注
    setrtLine_.setX(kPageMargin);
    setrtLine_.setY(bottomrightcurve.y());
    endLine_.setX(nPDFWidth);
    endLine_.setY(bottomrightcurve.y());
    pPainter->drawLine(setrtLine_,endLine_);

    QPoint topleftremarkText(kPageMargin,endLine_.y());
    QPoint bottomrightremarkheightText(nPDFWidth,endLine_.y()+itemHeight);
    pPainter->drawText(QRect(topleftremarkText,bottomrightremarkheightText),Qt::AlignVCenter|Qt::AlignLeft,"备注:");

    //int remark_height = nPDFHeight - bottomrightremarkheightText.y();
    QPoint topleftremark(kPageMargin,bottomrightremarkheightText.y());
    QPoint bottomrightremarkheight(nPDFWidth,nPDFHeight);

    pPainter->drawText(QRect(topleftremark,bottomrightremarkheight),Qt::AlignVCenter|Qt::AlignLeft|Qt::AlignTop,"医生备注标签提示信息或者其它");

    delete pPainter;
    delete pWriter;
    pdfFile.close();
    return;
}

void Printthereport::_NotifyPageSizeKind(Page_Size page_type,QPdfWriter *pWriter)
{
    switch (page_type)
    {
        case Page_Size::A3:
            pWriter->setPageSize(QPagedPaintDevice::A3);
        break;
        case Page_Size::A4:
            pWriter->setPageSize(QPagedPaintDevice::A4);
        break;
        case Page_Size::A5:
            pWriter->setPageSize(QPagedPaintDevice::A5);
        break;
        case Page_Size::CUSTOM_PAGE:

        break;
    }
}

void Printthereport::switchreagtestdata(QMap<quint8,QString> _data, QMap<quint8,QStringList>& _outdata_)
{
    QStringList curvedataview;
    auto iter = _data.begin();
    while(iter != _data.end())
    {
        curvedataview.clear();
        double numadd = 0;
        QStringList curveList = iter.value().split(",");
        for(int k = 0; k <curveList.size(); k++)
        {
            QString valstr = curveList[k];
            QUIUtils::parseDataratio(valstr);
            double val = qAbs(valstr.toDouble());
            numadd += val;
        }
        if(!qFuzzyIsNull(numadd))
        {
            curvedataview = curveList;
            _outdata_.insert(iter.key(),curvedataview);
        }
        iter++;
    }
}


void Printthereport::initpara()
{
    m_sampleid = "";
    m_samplename="";
    m_sampleSex ="";
    m_sampleBednum ="";
    m_sampleAges = 0;
    m_sampledepartment =""; //科室
    m_samplebarcode ="";
    m_refertithedoctor =""; //送检医生
    m_reviewdoctors =""; //审核医生
    m_testReagResult.clear();
}

void Printthereport::_settingSampleId(const QString idunm)
{
    m_sampleid = idunm;
    return;
}

void Printthereport::_setpixmapcurve(QPixmap pixmapcurve)
{
    m_pixmap = pixmapcurve;
    return;
}

void Printthereport::_setsamplename(const QString namesam)
{
    m_samplename = namesam;
}

void Printthereport::_setsampleSex(const QString sexindex)
{
    m_sampleSex = sexindex;
}

void Printthereport::_setsampleBednum(const QString bednum)
{
    m_sampleBednum = bednum;
    return;
}

void Printthereport::_setsampleAges(const quint8 ages)
{
    m_sampleAges = ages;
    return;
}

void Printthereport::_setsampledepartment(const QString sampledepartment)
{
    m_sampledepartment = sampledepartment;
    return;
}

void Printthereport::_setsampleBarcode(const QString barcodenum)
{
    m_samplebarcode = barcodenum;
    return;
}

void Printthereport::_setrefertithedoctor(const QString refertithedoctor)
{
    m_refertithedoctor = refertithedoctor;
    return;
}

void Printthereport::_setreviewdoctors(const QString viewdoctor)
{
    m_reviewdoctors = viewdoctor;
    return;
}

void Printthereport::_setTestCurvedata(QVariant data)
{
    m_testReagResult = data;
    return;
}

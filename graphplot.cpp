#include "graphplot.h"
#include "ui_graphplot.h"
#include "cglobal.h"
#include "sampledataprocess.h"
#include "loginui.h"
#include <operclass/fullyautomatedplatelets.h>
#include <QSharedPointer>
#include "globaldata.h"
#include <QtConcurrentRun>


#pragma execution_character_set("utf-8")


GraphPlot::GraphPlot(QWidget *parent) :QWidget(parent),
    ui(new Ui::GraphPlot),
    mcuteNumData(1),
    mbaverage(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    QPixmap pixmapmouse(":/Picture/test_hover.png");
    QCursor cursor(pixmapmouse);
    this->setCursor(cursor);

    bool ok_ = GlobalData::CreadFolder("suowei_testblood");
    QLOG_DEBUG()<<"创建索唯测试数据文件夹状态"<<ok_<<endl;

    InitChart();
}

GraphPlot::~GraphPlot()
{
    delete ui;


}


void GraphPlot::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);

	// Get equipment kind
	quint8 equipmentKind = 0;
	SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);

	// Calculate available space
	const int xSpace = 5;
	const int ySpace = 5;
	const int availableWidth = ui->widget_CurveInner->width() - xSpace * 5;
	const int availableHeight = ui->widget_CurveInner->height() - ySpace * 4;

	// Determine grid layout based on equipment kind
	int columns = 4;
	int rows = 1;
	int curveWidth = availableWidth / columns;
	int curveHeight = 0;

	switch (equipmentKind) {
	case KS600:
		rows = 1;
		curveHeight = availableHeight / 3 + availableHeight / 6; // same as /3 + /3/2
		break;
	case KS800:
		rows = 2;
		curveHeight = availableHeight / rows;
		break;
	case KS1200:
		rows = 3;
		curveHeight = availableHeight / rows;
		break;
	default:
		rows = 1;
		curveHeight = availableHeight / 3 + availableHeight / 6;
		break;
	}

	// Position all widgets in the grid
	for (int i = 0; i < m_pchnWidgetList.size(); i++) {
		int col = i % columns;
		int row = i / columns;

		// Skip if we exceed the expected widget count for this equipment
		if ((equipmentKind == KS600 && i >= 4) ||
			(equipmentKind == KS800 && i >= 8) ||
			(equipmentKind == KS1200 && i >= 12)) {
			break;
		}

		int x = xSpace * (col + 1) + col * curveWidth;
		int y = ySpace * (row + 1) + row * curveHeight;

		if (equipmentKind == KS600) {
			m_pchnWidgetList[i]->setFixedSize(curveWidth, curveHeight);
			y = ySpace + curveHeight / 2; // Special vertical position for KS600
		}
		else {
			m_pchnWidgetList[i]->setMaximumSize(curveWidth, curveHeight);
		}

		m_pchnWidgetList[i]->setGeometry(x, y, curveWidth, curveHeight);
	}
}

void GraphPlot::innitKindequipment()
{
    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmentKind);

    //预定义所有通道部件数组
    QWidget* allChannels[12] = {
        ui->aisle_widget_1, ui->aisle_widget_2, ui->aisle_widget_3, ui->aisle_widget_4,
        ui->aisle_widget_5, ui->aisle_widget_6, ui->aisle_widget_7, ui->aisle_widget_8,
        ui->aisle_widget_9, ui->aisle_widget_10, ui->aisle_widget_11, ui->aisle_widget_12
    };

    //根据设备类型确定通道数量
    int activeChannels = 0;
    switch (equipmentKind) {
        case KS600: activeChannels = 4; break;
        case KS800: activeChannels = 8; break;
        case KS1200: activeChannels = 12; break;
        default: activeChannels = 12; // 默认值
    }

    //统一处理通道显示/隐藏
    for (int i = 0; i < 12; ++i) {
        if (i < activeChannels) {
            m_pchnWidgetList.push_back(allChannels[i]);
            allChannels[i]->show(); // 确保显示
        } else {
            allChannels[i]->hide();
        }
    }

    for(auto pwidget : m_pchnWidgetList)
    {
        int indexchn = QUIUtils::StringFindintnum(pwidget->objectName()); //从1开始
        if(QCustomPlot* pCurveWidget = pwidget->findChild<QCustomPlot *>()){
            _initCreatCurveWidget(pCurveWidget,indexchn);
        }
    }
    return;
}


void GraphPlot::backallCurveClear()
{
    auto it =  mcurveWidgetList.begin();
    while(it != mcurveWidgetList.end())
    {
        it.value()->data().data()->clear();
        int index = it.key();
        QCustomPlot* pPlot =  GetCurvepWidget(index);
        pPlot->setBackground(QBrush(QColor("#FFFFFF")));
        it++;
    }
}

void GraphPlot::CleanCruve(quint8 indexChannel)
{
    bool bhad = mcurveWidgetList.contains(indexChannel);
    if(bhad)
    {
        auto cPGraph  = mcurveWidgetList.find(indexChannel);
        cPGraph.value()->data().data()->clear();
        cPGraph.value()->data().data()->squeeze();

        QCustomPlot* pPlot =  GetCurvepWidget(indexChannel);
        pPlot->setBackground(QBrush(QColor(188,187,183)));
        pPlot->replot(QCustomPlot::rpQueuedReplot);
    }
    return;
}

QCustomPlot* GraphPlot::GetCurvepWidget(int indexChn)
{
    QCustomPlot* pbackout = nullptr;
    for(auto pwidget : m_pchnWidgetList)
    {
        int ChnNum = QUIUtils::StringFindintnum(pwidget->objectName()) - 1; //通道从0 开始
        if(ChnNum == indexChn)
        {
            pbackout = pwidget->findChild<QCustomPlot *>();
            break;
        }
    }
    return pbackout;
}


void GraphPlot::notifyplotname(quint8 indexchn, QCustomPlot* pPlotWidget,QString dada)
{
    if(m_titemap.contains(indexchn))
    {
        auto iter = m_titemap.find(indexchn);
        QString outprint = QString("通道%1%2").arg(indexchn + 1).arg(dada);
        iter.value()->setText(outprint);
        pPlotWidget->replot(/*QCustomPlot::rpQueuedReplot*/);
    }
}

void  GraphPlot::_initCreatCurveWidget(QCustomPlot* pPlotWidget,int indexchn)
{
    int indexChannel = indexchn - 1;
    QString ShowText = QString("通道%1").arg(indexchn);

    QCPTextElement *plotTitle = new QCPTextElement(pPlotWidget);
    plotTitle->setText(ShowText);
    plotTitle->setTextColor(QColor(0,0,0));

    plotTitle->setFont(QFont("宋体", 12, QFont::Normal));
    m_titemap.insert(indexChannel,plotTitle);

    pPlotWidget->plotLayout()->insertRow(0);
    pPlotWidget->plotLayout()->addElement(0, 0, plotTitle);


    //设置曲线可拖拽 滚轮放大缩小 图像可选择
    //禁用选择矩形
    pPlotWidget->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    //使能拖动
    pPlotWidget->setInteraction(QCP::iRangeDrag, true);
    //pPlotWidget->setInteractions(QCP::iRangeDrag | QCP::iSelectPlottables);


    pPlotWidget->setFont(QFont(font().family(), 10));//设置文本的字体

    connect(pPlotWidget, &QCustomPlot::mouseMove, this, [=](QMouseEvent *event) {
        double x = event->pos().x();
        double y = event->pos().y();

        double x_ = pPlotWidget->xAxis->pixelToCoord(x);
        double y_ = pPlotWidget->yAxis->pixelToCoord(y);

        QString str = QString("x:%1;\ny:%2").arg(QString::number(x_, 10, 3))
        .arg(QString::number(y_, 10, 3));
        QToolTip::showText(cursor().pos(), str, pPlotWidget);
    });


    // 设置背景色
    pPlotWidget->setBackground(QBrush(QColor("#f5f7f9")));
    pPlotWidget->axisRect()->setupFullAxesBox();//四边安装轴并显示

    // 设置X/Y轴的标签
    pPlotWidget->xAxis->setLabel(tr("时间S"));
    pPlotWidget->yAxis->setLabel(tr("百分比%"));

    // 设置X/Y轴标签颜色
    pPlotWidget->xAxis->setLabelColor(QColor(Qt::red));
    pPlotWidget->yAxis->setLabelColor(QColor(Qt::red));

    // 设置x=0或y=0所在直线的画笔
    pPlotWidget->xAxis->grid()->setZeroLinePen(QPen(QColor(Qt::darkGray)));
    pPlotWidget->yAxis->grid()->setZeroLinePen(QPen(QColor(Qt::darkGray)));

    // 设置X/Y轴刻度范围
    pPlotWidget->xAxis->setRange(0, 300);
    pPlotWidget->yAxis->setRange(-20, 100);

    //设置X/Y轴刻度数，也就是分为几段
    QSharedPointer<QCPAxisTickerFixed> MyTicker (new QCPAxisTickerFixed);
    MyTicker.data()->setTickStep(30);
    MyTicker.data()->setTickCount(10);
    pPlotWidget->xAxis->setTicker(MyTicker);

    //pPlotWidget->xAxis->ticker()->setTickCount(10);
    pPlotWidget->yAxis->ticker()->setTickCount(10);
    pPlotWidget->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);//可读性优于设置
    pPlotWidget->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

    // 设置X/Y轴刻度值文本的颜色
    pPlotWidget->xAxis->setTickLabelColor(QColor("#000000"));
    pPlotWidget->yAxis->setTickLabelColor(QColor("#000000"));

    // 设置X/Y轴轴线的画笔
    pPlotWidget->xAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
    pPlotWidget->yAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));

    // 设置X/Y轴大刻度的画笔，被分段的位置
    pPlotWidget->xAxis->setTickPen(QPen(QColor("#FF0000")));
    pPlotWidget->yAxis->setTickPen(QPen(QColor("#FF0000")));

    // 设置X/Y轴小刻度的画笔
    pPlotWidget->xAxis->setSubTickPen(QPen(QColor("#000000")));
    pPlotWidget->yAxis->setSubTickPen(QPen(QColor("#000000")));

    // 设置内部网格线的画笔
    pPlotWidget->xAxis->grid()->setPen(QPen(QColor(Qt::darkRed), 1, Qt::DotLine));
    pPlotWidget->yAxis->grid()->setPen(QPen(QColor(Qt::darkRed), 1, Qt::DotLine));

    QCPGraph* m_showCpgraphChannel = nullptr;
    m_showCpgraphChannel = pPlotWidget->addGraph();
    m_showCpgraphChannel->setPen(QColor(0,0,0,255));
    m_showCpgraphChannel->setSmooth(false);//平滑开启曲线
    mcurveWidgetList.insert(indexChannel,m_showCpgraphChannel);

    pPlotWidget->replot(/*QCustomPlot::rpQueuedReplot*/);
    return;
}


void GraphPlot::DrawCurvePoint(quint8 indexChn, QVector<double> Xdata, QVector<double> Ydata)
{
    bool bhad = mcurveWidgetList.contains(indexChn);
    if(bhad)
    {
        auto cPGraph  = mcurveWidgetList.find(indexChn);
        cPGraph.value()->setData(Xdata, Ydata);
        QCustomPlot* pPlot = GetCurvepWidget(indexChn);
        pPlot->setBackground(QColor("#E8E8E8"));
        pPlot->replot(/*QCustomPlot::rpQueuedReplot*/);
    }
    return;
}

void GraphPlot::InitChart()
{
    for (int f = 0; f < MACHINE_SETTING_CHANNEL; f++){
        for (int k = 0; k < REAGENT_TOTAL; k++){
            mTestDataX[f][k].clear();    // 直接清空
            mTestDataY[f][k].clear();
            mTestDataX[f][k].reserve(TOTALDATANUM);
            mTestDataY[f][k].reserve(TOTALDATANUM);
        }
    }
    return;
}

/*单个项目测试完成 保存曲线图 进度条设置为0*/
void  GraphPlot::CompleteOneSample(const quint8 indexChannel, const quint8 reagents)
{
    //形参 通道数从0开始
    QString CompleteSample = "";
    int sampleid = 0;
    const quint8 testIndexReagent = reagents - 1;
    StructInstance::getInstance()->rootTestChnGetSampleInfo(indexChannel,CompleteSample,sampleid);

    resetvect(indexChannel,testIndexReagent);


	QCustomPlot* pPlot = GetCurvepWidget(indexChannel);
	QString outdata = "测试完成";
	notifyplotname(indexChannel, pPlot, outdata);

    //恢复异常通道显示状态
    FullyAutomatedPlatelets::pinstanceTesting()->giveupSampleChannelFlash(true,indexChannel);

    // 恢复通道状态（主线程执行）
//    QMetaObject::invokeMethod(this, [indexChannel] {
//        if (auto platelets = FullyAutomatedPlatelets::pinstanceTesting()) {
//            platelets->giveupSampleChannelFlash(true, indexChannel);
//        }
//    }, Qt::QueuedConnection);

    QLOG_DEBUG()<<"试剂"<<GlobalData::mapIndexReagentnames(reagents)<<"测试结束通道"<<
                  indexChannel + 1<<"清空进度状态"<<endl;

//    QtConcurrent::run([this, indexChannel, reagents, sampleid] {
//       save_test_data_to_sqllite(indexChannel, reagents, sampleid);
//    });

    save_test_data_to_sqllite(indexChannel,reagents,sampleid); //保存测试的数据到数据库
    return;
}

void GraphPlot::save_test_data_to_sqllite( const quint8 indexChannel, const quint8 reagents, int finishsampleid)
{
    quint16 AnaemiaInitValue = 0;   //贫血初值
    quint8 AnaemiaHole = 0, RichBloodHole = 0;
    QVector<double> TestedDatamap;  //数据
    TestedDatamap.clear();

    QString sampleID = GlobalData::groupDateAndID(GlobalData::ObatinCreatSampleTime(),finishsampleid);

    //获取贫血值和测试数据 && 清空测试完成试剂数据
    StructInstance::getInstance()->root_getinitvaluedata(indexChannel,reagents,AnaemiaInitValue,
                                                         TestedDatamap,AnaemiaHole,RichBloodHole);

    //测试数据保存和配置通道
    QString curvepoint ="";
    GlobalData::vectorconversionString(TestedDatamap,curvepoint);
    QString _outResultdata =  QUIUtils::OutPrintTestedResult(TestedDatamap); //测试结果
    FullyAutomatedPlatelets::pinstancesqlData()->updateTestCurveDataTale(sampleID,reagents,curvepoint);
    FullyAutomatedPlatelets::pinstancesqlData()->updateTestCurveDataTale(sampleID,ANEMIA,QString::number(indexChannel));

    QString keyval;
    switch(reagents)
    {
        case AA_REAGENT:  keyval ="AA"; break;
        case ADP_REAGENT:  keyval ="ADP"; break;
        case EPI_REAGENT:  keyval ="EPI"; break;
        case COL_REAGENT:  keyval ="COL"; break;
        case RIS_REAGENT:  keyval ="RIS"; break;
    default: break;
    }
    FullyAutomatedPlatelets::pinstancesqlData()->updateTestResultTable(sampleID,keyval ,_outResultdata);
    return;
}

double GraphPlot::get_mean(QVector<double> in)
{
    double sum = std::accumulate(in.begin(), in.end(), 0.0);
    double mean =  sum / in.size(); //均值
    return mean;
}

void  GraphPlot::recvsycnViewCurvePara(int cutnum , bool bavge)
{
    mcuteNumData = cutnum;
    mbaverage = bavge;
    QLOG_DEBUG()<<"分段数"<<mcuteNumData<<"平均"<<mbaverage;
}

void GraphPlot::resetvect(const int& Chn,const int& reagentNum){
    mTestDataX[Chn][reagentNum].clear();
    mTestDataY[Chn][reagentNum].clear();
    mTestDataX[Chn][reagentNum].reserve(TOTALDATANUM);
    mTestDataY[Chn][reagentNum].reserve(TOTALDATANUM);

    mCurvePlote_x[Chn][reagentNum].clear();
    mCurvePlote_y[Chn][reagentNum].clear();
    mCurvePlote_x[Chn][reagentNum].reserve(TOTALDATANUM);
    mCurvePlote_y[Chn][reagentNum].reserve(TOTALDATANUM);
}

/*接收测试数据实时绘制曲线*/
void GraphPlot::GetTestingValue(const QString &Sample, const quint8 &project, const quint8 &test_channel,
                                const float &testingdata,
                                const int &nPRP,
                                const int &initPRP,
                                const int &initPPP){

    const quint8 testingChannel =  test_channel - 1;
    const quint8 testingReagents = project - 1;

    QString datestr;
    int idnum;
    QString ReagentName =  GlobalData::mapIndexReagentnames(project);
    GlobalData::apartSampleId(Sample, datestr, idnum);

    QString data_ = QString::number(testingdata*100,'f',2)+"%";
    QCustomPlot* pPlot = GetCurvepWidget(testingChannel);
    QString outdata = tr("%1:PPP=%2 PRP=%3 %5(%4)").arg(ReagentName).arg(initPPP).arg(initPRP).arg(nPRP).arg(data_);
    notifyplotname(testingChannel, pPlot,outdata);


    //绘制曲线
    mTestDataX[testingChannel][testingReagents].push_back(mTestDataY[testingChannel][testingReagents].size());
    mTestDataY[testingChannel][testingReagents].push_back(testingdata*100.00);

    //绘制测试界面模组的进度条
    emit DrawProgressbar(testingChannel, calculateProgress(testingChannel, testingReagents));


    const QString filename = QString("Id%1通道%2%3").arg(idnum).arg(test_channel).arg(ReagentName);
    const bool isopenexperimentalMode = INI_File().getexperimentalMode();
    if(mTestDataY[testingChannel][testingReagents].size() == 1){
         CleanCruve(testingChannel);
         if(isopenexperimentalMode){
             QString datastr = QString("initPPP:%1 initPRP:%2 PRP(1):%3").arg(initPPP).arg(initPRP).arg(nPRP);
             GlobalData::CreatFloadfileAppend("ExperimentalMode",filename,datastr);
         }
    }
    else if(isopenexperimentalMode){
         GlobalData::CreatFloadfileAppend("ExperimentalMode",filename, QString::number(nPRP)+":"+ data_);
    }

    if(mcuteNumData == 1 && mbaverage == false )
    {
        DrawCurvePoint(testingChannel,
                       mTestDataX[testingChannel][testingReagents],
                       mTestDataY[testingChannel][testingReagents]);
    }
    else if(mcuteNumData != 1)
    {
        int numTotal = mTestDataX[testingChannel][testingReagents].size();
        if(numTotal == 1)
        {
            mCurvePlote_x[testingChannel][testingReagents].push_back(0);
            mCurvePlote_y[testingChannel][testingReagents].push_back(testingdata*100.00);
            DrawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagents],
                           mCurvePlote_y[testingChannel][testingReagents]);
        }
        else if(numTotal % mcuteNumData == 0 && numTotal/mcuteNumData != 0 && mbaverage == false)
        {
           mCurvePlote_x[testingChannel][testingReagents].push_back(mTestDataY[testingChannel][testingReagents].size());
           mCurvePlote_y[testingChannel][testingReagents].push_back(*(mTestDataY[testingChannel][testingReagents].end() - 1)*100.00);
           DrawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagents], mCurvePlote_y[testingChannel][testingReagents]);
        }
        else if(numTotal % mcuteNumData == 0 && numTotal/mcuteNumData != 0 && mbaverage == true)
        {
            QVector<double> prpvalue;
            prpvalue.clear();
            for(int i = 1 ; i <= mcuteNumData ; i++)
            {
               prpvalue.push_back(*(mTestDataY[testingChannel][testingReagents].end()- i));
            }
            double  outprp = get_mean(prpvalue);
            mCurvePlote_x[testingChannel][testingReagents].push_back(mTestDataY[testingChannel][testingReagents].size());
            mCurvePlote_y[testingChannel][testingReagents].push_back(outprp);
            DrawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagents],
                           mCurvePlote_y[testingChannel][testingReagents]);
        }
    }


    return;
}

double GraphPlot::calculateProgress(int channel, int reagent) const {
    return qBound(0.0,
           static_cast<double>(mTestDataX[channel][reagent].size())*100/TOTALDATANUM,
           100.0);
}






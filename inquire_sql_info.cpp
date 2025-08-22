#pragma execution_character_set("utf-8")

#include "inquire_sql_info.h"
#include "ui_inquire_sql_info.h"
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include "testing.h"
#include "quiutils.h"
#include "loginui.h"
#include "globaldata.h"
#include <operclass/fullyautomatedplatelets.h>

Inquire_Sql_Info::Inquire_Sql_Info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Inquire_Sql_Info)
{
    ui->setupUi(this);
	initTextTip();
    ui->progressBarLoad->setFixedHeight(25);
    ui->progressBarLoad->hide();
    ui->progressBarLoad->setStyleSheet("QProgressBar{text-align:center;background-color:#DDDDDD;border: 0px solid #DDDDDD;border-radius:5px;}"
                                       "QProgressBar::chunk{background-color:#05B8CC;border-radius:5px; width:8px;margin:0.5px;}");

    GlobalData::QCommboxSheet(ui->comboBox_age);
    GlobalData::QCommboxSheet(ui->comboBox_sendingdoctor);
    GlobalData::QCommboxSheet(ui->comboBox_Section);
    GlobalData::QLineEditSheet(ui->lineEdit_Name);
    GlobalData::QLineEditSheet(ui->lineEdit_bednum);


    QFile stylespinBox(":/Picture/SetPng/wholeSpinBox.qss");
    if(stylespinBox.open(QIODevice::ReadOnly)) {
          QString setspinBoxQss = QLatin1String(stylespinBox.readAll());
          ui->spinBox_label_Sample->setStyleSheet(setspinBoxQss);
          stylespinBox.close();
    } else {
        QLOG_WARN() << "QSS load error:" << stylespinBox.errorString();
    }

    m_group = new QButtonGroup(this);
    m_group->addButton(ui->checkBox_all,INQUIRE_ALL_SQL);
    m_group->addButton(ui->checkBox_today,INQUIRE_TODAY_SQL);
    m_group->addButton(ui->checkBox_this_month,INQUIRE_MONTH_SQL);
    m_group->addButton(ui->checkBox_select_time,INQUIRE_SPECIFIC_SQL);
    m_group->setExclusive(true);

    ui->Inquire_curve_1->setMaximumWidth(650);

    /*下拉选时间控件*/
    initDateEditUI(ui->dateEdit_begin,ui->widget_date_);
    initDateEditUI(ui->dateEdit_end,ui->widget_date_);
    ui->checkBox_today->setChecked(true);

    //初始化曲线
    setupRealtimeDataDemo(ui->Inquire_curve_1);

    ui->Inquire_curve_1->replot();
    
    /*列表样式得初始化*/
    Init_tablewidget_style();

    //初始化精确查找控件
    QStringList Agenum;
    for(int i = 0; i< 120;i++ )
        Agenum.push_back(QString("%1").arg(i+1));
    ui->comboBox_age->addItems(Agenum);
    ui->comboBox_age->setCurrentIndex(-1);

    LoginEngineerMode(false);
}

void Inquire_Sql_Info::LoginEngineerMode(const bool Enginnermode)
{
    if(!Enginnermode){
        ui->toolButton_stats->hide();
        ui->widget_sats_curve->hide();
    }
    else{
        ui->toolButton_stats->show();
        ui->widget_sats_curve->show();
    }
    update();
}


void Inquire_Sql_Info::init_sats_curve(QVector<double> useed_reag)
{
    ui->widget_sats_curve->clearPlottables();
    ui->widget_sats_curve->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    QCPAxis *keyAxis = ui->widget_sats_curve->xAxis;
    QCPAxis *valueAxis = ui->widget_sats_curve->yAxis;
	CustomBars  *fossil = new CustomBars(keyAxis,valueAxis);

    fossil->setAntialiased(false);
    fossil->setName("统计");
    fossil->setPen(QPen(QColor(255, 99, 71).lighter(130)));
    fossil->setBrush(QColor(255, 99, 71));
	fossil->setBaseValue(0);

    // 为柱状图设置一个文字类型的key轴，ticks决定了轴的范围，而labels决定了轴的刻度文字的显示
    QVector<double> ticks{1,2,3,4,5};
    QVector<QString> labels{"AA" , "ADP" , "EPI" , "COL" , "RIS"};
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    keyAxis->setTicker(textTicker);        // 设置为文字轴
    fossil->setWidth(fossil->width()/5); // 设置柱状图的宽度大小
    keyAxis->setTickLabelRotation(0);     // 轴刻度文字旋转60度
    keyAxis->setSubTicks(false);           // 不显示子刻度
    keyAxis->setTickLength(0, 4);          // 轴内外刻度的长度分别是0,4,也就是轴内的刻度线不显示
    keyAxis->setRange(0, 8);               // 设置范围
    keyAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    valueAxis->setRange(0, 1000);
    valueAxis->setPadding(25); //轴的内边距
    valueAxis->setLabel("仪器使用试剂统计");
    valueAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    fossil->setData(ticks, useed_reag);
    ui->widget_sats_curve->repaint();
    ui->widget_sats_curve->replot(QCustomPlot::rpQueuedReplot);
}


void Clear_table(QTableWidget* table)
{
    int total = table->rowCount();
    for(int i = 0;i <total ;i++)
		table->removeRow(0);
    return;
}

void Inquire_Sql_Info::resizeEvent(QResizeEvent *event)
{
	ui->tableWidget_SQL_Inquire->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget_SQL_Inquire->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);  
}

void Inquire_Sql_Info::Init_tablewidget_style()
{
    QStringList header{tr("样本号"),tr("添加时间"), tr("条形码") , tr("姓名"), tr("性别 ") , tr("年龄")
                       ,tr("科别") ,tr("床号"),tr("送检医生"),tr("AA聚集率"),tr("ADP聚集率")
                       ,tr("EPI聚集率"),tr("COL聚集率"), tr("RIS聚集率")};
    int allcol = header.size();
    QTableWidget *pinquireTable = ui->tableWidget_SQL_Inquire;

    pinquireTable->setColumnCount(allcol); //初始化列
    pinquireTable->setHorizontalHeaderLabels(header);

	QFont font;
    font.setFamily("楷体");
    font.setBold(true);
    font.setPixelSize(14);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 1);//设置字符间距

    pinquireTable->horizontalHeader()->setFont(font);
    pinquireTable->horizontalHeader()->setHighlightSections(false);
    pinquireTable->setFocusPolicy(Qt::NoFocus);
    pinquireTable->setFrameShape(QFrame::Box); //设置边框
    pinquireTable->setShowGrid(true); //设置不显示格子线
    pinquireTable->verticalHeader()->setVisible(false); //设置垂直头不可见
    pinquireTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pinquireTable->setSelectionMode(QAbstractItemView::SingleSelection);
    pinquireTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    pinquireTable->horizontalHeader()->setStretchLastSection(true);
   
    pinquireTable->horizontalHeader()->setStretchLastSection(true);


    pinquireTable->setColumnWidth(0, 100);
    pinquireTable->setColumnWidth(2, 140);

    QHeaderView *header1 = pinquireTable->verticalHeader();
    header1->setHidden(true);

    pinquireTable->verticalHeader()->setDefaultSectionSize(30); //设置行高
    pinquireTable->horizontalHeader()->setDefaultSectionSize(30);

    pinquireTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    pinquireTable->setColumnWidth(0, 130);

    pinquireTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    pinquireTable->setColumnWidth(1, 110);

    pinquireTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    pinquireTable->setColumnWidth(2, 130);

    for(int i = 3; i < 9; i++){
        pinquireTable->horizontalHeader()->setSectionResizeMode(i,QHeaderView::Fixed);
        pinquireTable->setColumnWidth(i, 90);
    }
    for(int i = 9; i < header.size() - 1; i++){
        pinquireTable->horizontalHeader()->setSectionResizeMode(i,QHeaderView::Fixed);
        pinquireTable->setColumnWidth(i, 130);
    }
    pinquireTable->verticalHeader()->setResizeContentsPrecision(QHeaderView::Stretch);
    pinquireTable->horizontalHeader()->setFixedHeight(40); //设置表头的高度
    pinquireTable->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    pinquireTable->setAlternatingRowColors(true); //隔行换色
   //pinquireTable->verticalHeader()->show();// 显示行号

   pinquireTable->horizontalHeader()->setStyleSheet("QHeaderView::section{border:1px solid #696969;"
                                                                   "background-color:rgb(188, 187, 186); font:14pt '楷体'; "
                                                                   "color: black;};");
    pinquireTable->setStyleSheet(TableWidgetCss);
    pinquireTable->verticalScrollBar()->setStyleSheet(VScroBarCss); //垂直
    pinquireTable->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:8px;}"
         "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:2px;}"
         "QScrollBar::handle:hover{background:gray;}"
         "QScrollBar::sub-line{background:transparent;}"
         "QScrollBar::add-line{background:transparent;}");

    connect(pinquireTable, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(SelectItem(QTableWidgetItem *)));
	return;
}

Inquire_Sql_Info::~Inquire_Sql_Info()
{
    m_threadInqure.quit();
    m_threadInqure.wait();

    if(mquiredataclass)
        delete mquiredataclass;
    mquiredataclass = nullptr;

    delete ui;
    QLOG_DEBUG()<<"析构查询实例"<<__FILE__<<__LINE__<<endl;
}

void Inquire_Sql_Info::closeEvent(QCloseEvent *event)
{
    event->accept();
}


void Inquire_Sql_Info::myMoveEvent(QMouseEvent *event)
{

    // 使用局部引用避免重复访问
    QCustomPlot* plot = ui->Inquire_curve_1;
    const QPoint pos = event->pos();

    // 检查是否在绘图区域内
    if (!plot->rect().contains(pos)) {
        QToolTip::hideText();
        return;
    }

    // 坐标转换
    const double xCoord = plot->xAxis->pixelToCoord(pos.x());
    const double yCoord = plot->yAxis->pixelToCoord(pos.y());

    // 使用静态字符串避免重复分配（如果格式固定）
    static const QString format = "X: %1\nY: %2";
    QString str = format.arg(xCoord, 0, 'f', 3).arg(yCoord, 0, 'f', 3);

    // 显示tooltip
    QToolTip::showText(event->globalPos(), str, plot);
}

void Inquire_Sql_Info::_creatCPGraph(QCustomPlot* pshowcurvedata)
{
    QList<QCPGraph*> ReagCurveList;
    m_showAACpgraph = pshowcurvedata->addGraph();
    ReagCurveList.push_back(m_showAACpgraph);
    m_showADPCpgraph = pshowcurvedata->addGraph();
    ReagCurveList.push_back(m_showADPCpgraph);
    m_showEPICpgraph = pshowcurvedata->addGraph();
    ReagCurveList.push_back(m_showEPICpgraph);
    m_showCOLCpgraph = pshowcurvedata->addGraph();
    ReagCurveList.push_back(m_showCOLCpgraph);
    m_showRISCpgraph = pshowcurvedata->addGraph();
    ReagCurveList.push_back(m_showRISCpgraph);

    pshowcurvedata->legend->setVisible(true);

    int k = 0;
    QList<QPen>  curveColorList;
    QList<QString> curveNameList;

    curveColorList.append(QPen(GlobalData::customCurveColor(AA_REAGENT),2,Qt::SolidLine));
    curveNameList.append("AA");
    curveColorList.append(QPen(GlobalData::customCurveColor(ADP_REAGENT),2,Qt::SolidLine));
    curveNameList.append("ADP");
    curveColorList.append(QPen(GlobalData::customCurveColor(EPI_REAGENT),2,Qt::SolidLine));
    curveNameList.append("EPI");
    curveColorList.append(QPen(GlobalData::customCurveColor(COL_REAGENT),2,Qt::SolidLine));
    curveNameList.append("COL");
    curveColorList.append(QPen(GlobalData::customCurveColor(RIS_REAGENT),2,Qt::SolidLine));
    curveNameList.append("RIS");


    for(QCPGraph* pdata : ReagCurveList)
    {
        pdata->setPen(curveColorList.at(k));
        pdata->setName(curveNameList.at(k));
        pdata->setLineStyle(QCPGraph::LineStyle::lsLine);
        pdata->setAntialiasedFill(true);//设置抗锯齿
        k++;
    }
    pshowcurvedata->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignRight);
    pshowcurvedata->legend->setBrush(QColor(255,255,255,0));//设置图例背景
    pshowcurvedata->replot();
    return;
}

void Inquire_Sql_Info::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    if (!customPlot) {
        QLOG_WARN() << "Custom plot is null!";
        return;
    }

    //性能优化：禁用抗锯齿
    customPlot->setNotAntialiasedElements(QCP::aeAll);

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    //字体设置优化
    QFont plotFont = font();
    plotFont.setFamily(font().family());
    plotFont.setPointSize(10);
    customPlot->setFont(plotFont);


    connect(customPlot,&QCustomPlot::mouseMove,
            this,&Inquire_Sql_Info::myMoveEvent);

    // 设置背景色
    customPlot->setBackground(QBrush(QColor("#FFFAFA")));

    // 设置X/Y轴的标签
    customPlot->xAxis->setLabel(tr("时间S"));
    customPlot->yAxis->setLabel(tr("百分比%"));
    // 设置X/Y轴标签颜色
    customPlot->xAxis->setLabelColor(QColor(Qt::red));
    customPlot->yAxis->setLabelColor(QColor(Qt::red));




    // 设置X/Y轴刻度范围
    customPlot->xAxis->setRange(0, 300);
    customPlot->yAxis->setRange(-20, 100);

    //刻度设置优化
    QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
    xTicker->setTickStep(30);
    xTicker->setTickCount(10);
    customPlot->xAxis->setTicker(xTicker);

    QSharedPointer<QCPAxisTickerFixed> yTicker(new QCPAxisTickerFixed);
    yTicker->setTickStep(12); // (-20到100共120单位，分10段)
    yTicker->setTickCount(10);
    customPlot->yAxis->setTicker(yTicker);

    //网格和零线设置
    QPen zeroLinePen;
    zeroLinePen.setColor(QColor(Qt::darkGreen));  // 修正：使用Qt::darkGreen
    zeroLinePen.setWidth(2);
	customPlot->xAxis->grid()->setZeroLinePen(zeroLinePen);
	customPlot->yAxis->grid()->setZeroLinePen(zeroLinePen);

    customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|
                                QCP::iSelectAxes |QCP::iSelectLegend |
                                QCP::iSelectPlottables);

    //关联选点信号
    connect(customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)),
            this, SLOT(OnPlotClick(QCPAbstractPlottable*, int, QMouseEvent*)));

    //性能优化：延迟重绘
    QTimer::singleShot(0, customPlot, [customPlot]() {
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });

    _creatCPGraph(customPlot);// 创建图例层

    return;
}
void Inquire_Sql_Info::initTextTip()
{
	if (!m_TextTip) {
		m_TextTip = new QCPItemText(ui->Inquire_curve_1);

		m_TextTip->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
		m_TextTip->position->setType(QCPItemPosition::ptAbsolute);
		m_TextTip->setColor(Qt::black);
        m_TextTip->setFont(QFont(font().family(), 10));
		m_TextTip->setPen(QPen(Qt::darkGray));
		m_TextTip->setBrush(QBrush(QColor(255, 255, 225, 230))); // 浅黄色背景
		m_TextTip->setPadding(QMargins(8, 5, 8, 5));
		m_TextTip->setVisible(false);
	}
}
void Inquire_Sql_Info::OnPlotClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
	if (!m_TextTip) {
		initTextTip();
	}

   QCPGraph *clickedGraph = qobject_cast<QCPGraph*>(plottable);
   if (!clickedGraph) return;

   QString cannel_num = clickedGraph->name();

   // 直接使用点击的图形对象
   const QCPGraphData *ghd = clickedGraph->data()->at(dataIndex);
   if (!ghd) return;

   QString text = cannel_num + ":" + "(" + "数据量:" + QString::number(ghd->key + 1, 10, 0) + ","
				  + "结果值:" + QString::number(ghd->value, 10, 2) + ")";
   m_TextTip->setText(text);
   m_TextTip->position->setCoords(event->pos().x() + 30, event->pos().y() - 15);
   m_TextTip->setVisible(true);
   ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
}


void Inquire_Sql_Info::RecvCurveData(const QVector<QString>& data)
{
    for(int i = 0; i < data.size(); ++i){
        QString curvepos = data[i].simplified();
        if (curvepos.isEmpty()) {
            continue;
        }
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QStringList valdataList = curvepos.split(',', Qt::SkipEmptyParts);
#else
        QStringList valdataList = curvepos.split(',', QString::SkipEmptyParts);
#endif
        addInquireCurvedata(valdataList, i + 1);
    }
    ui->Inquire_curve_1->legend->setVisible(true);
    ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
    update();
    emit clickOutPdfFile(m_clickId);
    return;
}


void Inquire_Sql_Info::SelectItem(QTableWidgetItem *item)
{
    cleanPlogtandUpdate();

    QTableWidget * ptablewidget = ui->tableWidget_SQL_Inquire;
    int selrows = item->row(); //样本号的行
	if (selrows % 4 != 0)  
		return;
    if(ptablewidget->item(selrows,0) == 0)
    {
        QMessageBox::information(nullptr,tr("查询曲线失败"),tr("未查询到样本号"));
        QLOG_DEBUG()<<"选中查看曲线失败,未查询到样本号"<< __FILEW__ << __LINE__<<endl;
        return;
    }
    m_clickId = ptablewidget->item(selrows, 0)->text();
    emit this->InquierCurveView(m_clickId); //查寻样本曲线点数据
    return;
}


void Inquire_Sql_Info::addInquireCurvedata(QStringList dataList,quint8 indexReag)
{
    if (dataList.isEmpty()) {
        qWarning() << "Empty data list provided for reagent index:" << indexReag;
        return;
    }

    QVector<double> rawData;
    rawData.reserve(dataList.size());

    int conversionErrors = 0;
    std::transform(dataList.cbegin(), dataList.cend(), std::back_inserter(rawData),
                [&conversionErrors](const QString& str) {
                      bool ok;
                      double val = str.toDouble(&ok) * 100.0;
                      if (!ok) {
                          qWarning() << "Failed to convert string to double:" << str;
                          conversionErrors++;
                          return 0.0;
                    }
                    return val;
                 });

    if (conversionErrors > 0) {
        qWarning() << "Found" << conversionErrors << "conversion errors in data for reagent:" << indexReag;
    }

    //生成坐标
    QVector<double> posx(rawData.size());
    std::iota(posx.begin(), posx.end(), 0.0);

    //设置图表数据
    static const QHash<quint8, QCPGraph*> reagent_graphs {
        {AA_REAGENT,  m_showAACpgraph},
        {ADP_REAGENT, m_showADPCpgraph},
        {EPI_REAGENT, m_showEPICpgraph},
        {COL_REAGENT, m_showCOLCpgraph},
        {RIS_REAGENT, m_showRISCpgraph}
    };

    auto graph = reagent_graphs.value(indexReag, nullptr);
    if (!graph) {
        qWarning() << "Invalid reagent index or null graph pointer:" << indexReag;
        return;
    }

    //线程安全地更新图表数据
    QMetaObject::invokeMethod(this, [this, graph, posx, rawData]() {
        if (graph && !posx.isEmpty() && !rawData.isEmpty()) {
            graph->setData(posx, rawData);
            ui->Inquire_curve_1->replot();
        }
    }, Qt::QueuedConnection);
}

void Inquire_Sql_Info::cleanPlogtandUpdate()
{
    m_showAACpgraph->data().data()->clear();
    m_showADPCpgraph->data().data()->clear();
    m_showEPICpgraph->data().data()->clear();
    m_showCOLCpgraph->data().data()->clear();
    m_showRISCpgraph->data().data()->clear();
    ui->Inquire_curve_1->repaint();
    ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
    update();
    return;
}

void Inquire_Sql_Info::firstrunthread()
{
    if(!mquiredataclass)
    {
       mquiredataclass = new QueryDataThread();
       mquiredataclass->moveToThread(&m_threadInqure);
       qRegisterMetaType<InqueryDatastu_t>("InqueryDatastu_t");
       connect(mquiredataclass,&QueryDataThread::LoadInquierdata,this,&Inquire_Sql_Info::ViewLoadInquierdata);

       connect(mquiredataclass,&QueryDataThread::clearTableWidget,this,[=](){
           cleanPlogtandUpdate();
           DeleteStuData();
           update();
       });

       connect(mquiredataclass,&QueryDataThread::InquireEmpty,this,[=](){
            QMessageBox::warning(nullptr,tr("查讯完成"),tr("查询结果为空!"));
            return;
       });

       connect(&m_threadInqure,&QThread::started,
               mquiredataclass,&QueryDataThread::_startSycnData);

       connect(this,&Inquire_Sql_Info::FindModuleStyle,
               mquiredataclass,&QueryDataThread::slotFindModuleStyle);

       connect(this,&Inquire_Sql_Info::FindspecifiedData,
               mquiredataclass,&QueryDataThread::slotFindspecifiedData);

       connect(this,&Inquire_Sql_Info::Locatethelookup,
               mquiredataclass,&QueryDataThread::slotLocatethelookup); //精确查找

       connect(this,&Inquire_Sql_Info::InquierCurveView,
               mquiredataclass,
               &QueryDataThread::InquierCurveViewEnd);

       connect(mquiredataclass,&QueryDataThread::sendCurveData,
                this,&Inquire_Sql_Info::RecvCurveData,
                Qt::QueuedConnection);

       connect(this,&Inquire_Sql_Info::clickOutPdfFile,
               mquiredataclass,
               &QueryDataThread::ObatinCreatPdfPara);

       connect(mquiredataclass,&QueryDataThread::outPDFPara,this,[=](InqueryDatastu_t *pdata){
           if(pdata)
                insertparaTopdffile(pdata);
       });

       m_threadInqure.start();
    }
}



void Inquire_Sql_Info::on_toolButton_OK_clicked()
{
    firstrunthread(); //确保线程启动
    quint8 index_ = m_group->checkedId();
    if(index_ == INQUIRE_SPECIFIC_SQL)
    {
        QDateTime time1 = ui->dateEdit_begin->dateTime();
        QDateTime time2 = ui->dateEdit_end->dateTime();
        int days = time1.daysTo(time2);
        if (days >= 0)
		{
            emit FindspecifiedData(ui->dateEdit_begin,days);
		}
		else
		{
			QMessageBox::warning(nullptr, tr("提示"), tr("选择开始时间应小于(或等于)结束时间!"));
			return;
		}
    }
    else
    {
        emit FindModuleStyle(index_);
    }
    return;
}


/************** 精确查找 ***********/
void Inquire_Sql_Info::on_toolButtonFindexatc_clicked()
{
    firstrunthread();
    QString  inquierId = ui->spinBox_label_Sample->text(); //样本号
    QString  senddoctor = ui->comboBox_sendingdoctor->currentText(); //送检医生
    QString  PatientsName = ui->lineEdit_Name->text(); //姓名
    QString  PatientsAge = ui->comboBox_age->currentText();
    QString  bbednum = ui->lineEdit_bednum->text();
    QString  sectionkind =   ui->comboBox_Section->currentText(); //科别
    emit this->Locatethelookup(inquierId,senddoctor,PatientsName,PatientsAge,sectionkind,bbednum);
    return;
}

//清空查找到的数据
void Inquire_Sql_Info::DeleteStuData()
{
    Clear_table(ui->tableWidget_SQL_Inquire);
}


void Inquire_Sql_Info::SetColumnTextGroup(QTableWidget * tablewiget, int row, int col, QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tablewiget->setSpan(row*4, col, 4, 1);
    QFont font("楷体",12);
    item->setFont(font);
    tablewiget->setItem(row*4, col, item);
    return;
}

void Inquire_Sql_Info::InsertInqireResult(int Rows,quint8 Cols,QString Datastr)
{
    QFont font("楷体",12);
    QTableWidgetItem *item = new QTableWidgetItem(Datastr);
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    item->setFont(font);
    QColor bgmcolor;
    switch(Cols)
    {
        case Inquire_AA:  bgmcolor = GlobalData::customCurveColor(AA_REAGENT); break;
        case Inquire_ADP: bgmcolor = GlobalData::customCurveColor(ADP_REAGENT); break;
        case Inquire_EPI: bgmcolor = GlobalData::customCurveColor(EPI_REAGENT); break;
        case Inquire_COL: bgmcolor = GlobalData::customCurveColor(COL_REAGENT);break;
        case Inquire_RIS: bgmcolor = GlobalData::customCurveColor(RIS_REAGENT);break;
    default:  bgmcolor.setRgb(120,120,120); break;
    }
    QBrush nullColor(bgmcolor);
    item->setBackground(nullColor);
    ui->tableWidget_SQL_Inquire->setItem(Rows,Cols,item);
    return;
}


void Inquire_Sql_Info::CheckWhetherTestOrNot(QString &ResultCheck)
{
	if (ResultCheck == "" || ResultCheck == "null")
	{
		ResultCheck = "/,/,/,/";
		return;
	}
    QStringList resultdata = ResultCheck.split(",");
    if(resultdata.size() == 4)
    {
        double addnum = 0.00;
        for(int i = 0; i < resultdata.size(); i++)
        {
            QString data_ = QString(resultdata.at(i)).remove("%");
            addnum += data_.toDouble();
        }
        bool qeuipzero =  qFuzzyIsNull(addnum);
        if(qeuipzero)
        {
            ResultCheck = "/,/,/,/";
        }
    }
    return;
}

//统计试剂用量
void Inquire_Sql_Info::on_toolButton_stats_clicked()
{
    quint8 index_ = m_group->checkedId();
    switch(index_)
    {
        case INQUIRE_ALL_SQL:		stats_all_reagent();            break;
        case INQUIRE_TODAY_SQL:		stats_today_reagent();          break;
        case INQUIRE_MONTH_SQL:     stats_thismonth_reagent();      break;
        case INQUIRE_SPECIFIC_SQL:  stats_designate_reagent();      break;
        default: break;
    }
    return;
}

//统计试剂 全部查询
void Inquire_Sql_Info::stats_all_reagent()
{
    QMap<quint8,int> all_reagent;
    QVector<double> useed_reag;
    FullyAutomatedPlatelets::pinstancesqlData()->inquire_all_stats_reagent(all_reagent);
    auto it = all_reagent.begin();
    while(it != all_reagent.end())
    {
       useed_reag.push_back(it.value());
       it++;
    }
    init_sats_curve(useed_reag);
    return;
}

//统计试剂 天查询
void Inquire_Sql_Info::stats_today_reagent()
{
    QVector<double> useed_reag;
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString today_ = QString("%1%2%3").arg(current_date_time.toString("yyyy")).arg(current_date_time.toString("MM")).
        arg(current_date_time.toString("dd"));
    QMap<quint8,int> use_reag;
    FullyAutomatedPlatelets::pinstancesqlData()->inquire_single_stas_total(today_,use_reag);
    auto it = use_reag.begin();
    while(it != use_reag.end())
    {
       useed_reag.push_back(it.value());
       it++;
    }
    init_sats_curve(useed_reag);
    return;
}

//统计试剂 月查询
void Inquire_Sql_Info::stats_thismonth_reagent()
{
    QVector<double> useed_reag_vec;
    useed_reag_vec.clear();
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString systemtime_month = current_date_time.toString("yyyyMM");
    FullyAutomatedPlatelets::pinstancesqlData()->inquire_thismonth_stats_reagent(systemtime_month,useed_reag_vec);
    init_sats_curve(useed_reag_vec);
}

//统计试剂 指定时间查询
void Inquire_Sql_Info::stats_designate_reagent()
{
    QVector<double> useed_reag_vec;
    useed_reag_vec.clear();
    int days = ui->dateEdit_begin->dateTime().daysTo(ui->dateEdit_end->dateTime());
    if(days < 0)
    {
        QMessageBox::warning(nullptr,tr("提示"),tr("选择开始时间应小于(或等于)结束时间!"));
        return;
    }

    for (int i = 0; i < days; i++)
    {
        QMap<quint8,int> used_reagent;
        used_reagent.clear();
        QString need_days = ui->dateEdit_begin->dateTime().addDays(i).toString("yyyyMMdd");
        FullyAutomatedPlatelets::pinstancesqlData()->inquire_single_stas_total(need_days,used_reagent);
        auto iter = used_reagent.begin();
        while(iter != used_reagent.end())
        {
            useed_reag_vec.push_back(iter.value());
            iter++;
        }
    }
    if(useed_reag_vec.size()%5 != 0)
    {
        QMessageBox::information(nullptr,"指定统计查询失败","查询数据异常");
        return;
    }
    QVector<double> show_stats_vec;
    show_stats_vec.clear();
    int stats_reag[5] = { 0 };
    for(int k = 0 ; k < useed_reag_vec.size(); k++ )
    {
       int row_ = k/5;
       stats_reag[k%5] = useed_reag_vec.at(k%5+(row_*5)) + stats_reag[k%5];
    }
    for(int i = 0 ;i < 5 ; i++)
        show_stats_vec.push_back(stats_reag[i]);
    init_sats_curve(show_stats_vec);
    return;
}

void Inquire_Sql_Info::ViewLoadInquierdata(int numtotal, int n_ing,InqueryDatastu_t *pdata)
{
    InsertOneRowsData(pdata);
    m_del.push_back(pdata);
    if(numtotal == 0) return;
    ui->progressBarLoad->show();
    double ratio_ = n_ing*100/numtotal;
    ui->progressBarLoad->setValue(ratio_);
    ui->progressBarLoad->setFormat(QString("当前查讯进度为:%1%").arg(QString::number(ratio_, 'f', 2)));
    ui->progressBarLoad->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if(numtotal == n_ing)
    {
        qDeleteAll(m_del.begin(),m_del.end());
        m_del.clear();

        QTimer::singleShot(3000, this, [=](){
            ui->progressBarLoad->hide();
            ui->progressBarLoad->setValue(0);
        });
    }
	update();
    return;
}


//查找到数据插入到表格
void Inquire_Sql_Info::InsertOneRowsData(InqueryDatastu_t *pdata)
{
    if(pdata == nullptr)
        return;
    int row = ui->tableWidget_SQL_Inquire->rowCount();
	
    QString aaData = QString::fromLocal8Bit(pdata->ReagAAresult);
    CheckWhetherTestOrNot(aaData);

    QString adpData = QString::fromLocal8Bit(pdata->ReagADPresult);
    CheckWhetherTestOrNot(adpData);

    QString epiData = QString::fromLocal8Bit(pdata->ReagEPIresult);
    CheckWhetherTestOrNot(epiData);

    QString colData = QString::fromLocal8Bit(pdata->ReagCOLresult);
    CheckWhetherTestOrNot(colData);

    QString risData = QString::fromLocal8Bit(pdata->ReagRISresult);
    CheckWhetherTestOrNot(risData);


    QStringList AA_Result = aaData.split(",");
    QStringList ADP_Result = adpData.split(",");
    QStringList EPI_Result = epiData.split(",");
    QStringList COL_Result = colData.split(",");
    QStringList RIS_Result = risData.split(",");

    if(AA_Result.size() == 4 && ADP_Result.size() == 4 && EPI_Result.size() == 4 && COL_Result.size()== 4 && RIS_Result.size() == 4)
    {
        int rows = row/4;
        int n = 0;
        ui->tableWidget_SQL_Inquire->insertRow(row);

        QMap<quint8,QString> Sixtysecondsofdata;
        Sixtysecondsofdata.clear();
        Sixtysecondsofdata.insert(Inquire_AA, AA_Result.at(n));
        Sixtysecondsofdata.insert(Inquire_ADP,ADP_Result.at(n));
        Sixtysecondsofdata.insert(Inquire_EPI,EPI_Result.at(n));
        Sixtysecondsofdata.insert(Inquire_COL,COL_Result.at(n));
        Sixtysecondsofdata.insert(Inquire_RIS,RIS_Result.at(n));
        auto iter = Sixtysecondsofdata.constBegin();
        while(iter != Sixtysecondsofdata.constEnd())
        {
            InsertInqireResult(row,iter.key(), QString("%1 [60S]").arg(iter.value()));
            iter++;
        }

        n++;
        row++;
        ui->tableWidget_SQL_Inquire->insertRow(row);
        QMap<quint8,QString> OneHundredEightysecondsofdata;
        OneHundredEightysecondsofdata.clear();
        OneHundredEightysecondsofdata.insert(Inquire_AA,AA_Result.at(n));
        OneHundredEightysecondsofdata.insert(Inquire_ADP,ADP_Result.at(n));
        OneHundredEightysecondsofdata.insert(Inquire_EPI,EPI_Result.at(n));
        OneHundredEightysecondsofdata.insert(Inquire_COL,COL_Result.at(n));
        OneHundredEightysecondsofdata.insert(Inquire_RIS,RIS_Result.at(n));
        iter = OneHundredEightysecondsofdata.constBegin();
        while(iter != OneHundredEightysecondsofdata.constEnd())
        {
            InsertInqireResult(row,iter.key(), QString("%1 [180S]").arg(iter.value()));
            iter++;
        }

        n++;
        row++;
        ui->tableWidget_SQL_Inquire->insertRow(row);
        QMap<quint8,QString> ThreeHundredsecondsofdata;
        ThreeHundredsecondsofdata.clear();
        ThreeHundredsecondsofdata.insert(Inquire_AA,AA_Result.at(n));
        ThreeHundredsecondsofdata.insert(Inquire_ADP,ADP_Result.at(n));
        ThreeHundredsecondsofdata.insert(Inquire_EPI,EPI_Result.at(n));
        ThreeHundredsecondsofdata.insert(Inquire_COL,COL_Result.at(n));
        ThreeHundredsecondsofdata.insert(Inquire_RIS,RIS_Result.at(n));
        iter = ThreeHundredsecondsofdata.constBegin();
        while(iter != ThreeHundredsecondsofdata.constEnd())
        {
            InsertInqireResult(row,iter.key(), QString("%1 [300S]").arg(iter.value()));
            iter++;
        }

        n++;
        row++;
        ui->tableWidget_SQL_Inquire->insertRow(row);
        QMap<quint8,QString> Maxsecondsofdata;
        Maxsecondsofdata.clear();
        Maxsecondsofdata.insert(Inquire_AA,AA_Result.at(n));
        Maxsecondsofdata.insert(Inquire_ADP,ADP_Result.at(n));
        Maxsecondsofdata.insert(Inquire_EPI,EPI_Result.at(n));
        Maxsecondsofdata.insert(Inquire_COL,COL_Result.at(n));
        Maxsecondsofdata.insert(Inquire_RIS,RIS_Result.at(n));
        iter = Maxsecondsofdata.constBegin();
        while(iter != Maxsecondsofdata.constEnd())
        {
            InsertInqireResult(row,iter.key(), QString("%1 [MAX]").arg(iter.value()));
            iter++;
        }


		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_Sample, QString::fromLocal8Bit(pdata->Sample_Num));
        QString addtime_ = QString::fromLocal8Bit(pdata->AddTime);
        QStringList adtimeList = addtime_.split(" ");
        if(adtimeList.size() == 2)
            SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, AddTask_time, adtimeList.at(1));
        else
            SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, AddTask_time, QString::fromLocal8Bit(pdata->AddTime));
		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_Bar_code, QString::fromLocal8Bit(pdata->BarCode));
		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_Name, QString::fromLocal8Bit(pdata->Sample_Name));
		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_Sex, QString::fromLocal8Bit(pdata->Sample_Sex));
		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_Age, QString::fromLocal8Bit(pdata->Sample_Age));
		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_Section, QString::fromLocal8Bit(pdata->Sample_kebie));
		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_Bednun, QString::fromLocal8Bit(pdata->Sample_BedNum));
		SetColumnTextGroup(ui->tableWidget_SQL_Inquire, rows, Inquire_sendingdoctor, QString::fromLocal8Bit(pdata->Sample_doctor));

        update();
    }
}


//输出PDF
void Inquire_Sql_Info::insertparaTopdffile(InqueryDatastu_t *pdata)
{
    Printthereport *pwritepdf = FullyAutomatedPlatelets::pinstancePrintPdf();

    pwritepdf->initpara();//清楚记录先

    QString outprintIddate;
    int  outprintId;
    GlobalData::apartSampleId(QString::fromLocal8Bit(pdata->Sample_Num),outprintIddate,outprintId);
    pwritepdf->_settingSampleId(QString::number(outprintId));

    QPixmap curvePixmap = QPixmap::grabWidget(ui->Inquire_curve_1, ui->Inquire_curve_1->rect());
    pwritepdf->_setpixmapcurve(curvePixmap);

    pwritepdf->_setsamplename(QString::fromLocal8Bit(pdata->Sample_Name));

    pwritepdf->_setsampleSex(QString::fromLocal8Bit(pdata->Sample_Sex));

    pwritepdf->_setsampleBednum(QString::fromLocal8Bit(pdata->Sample_BedNum));

    pwritepdf->_setsampleAges(QString::fromLocal8Bit(pdata->Sample_Age).toUInt());

    pwritepdf->_setsampledepartment(QString::fromLocal8Bit(pdata->Sample_kebie));

    pwritepdf->_setsampleBarcode(QString::fromLocal8Bit(pdata->BarCode));

    pwritepdf->_setrefertithedoctor(QString::fromLocal8Bit(pdata->Sample_doctor));

    pwritepdf->_setreviewdoctors(QString::fromLocal8Bit(pdata->Sample_doctor));//审核医生

    QMap<quint8,QString> reagcurvedata;
    reagcurvedata.clear();
    if(QString::fromLocal8Bit(pdata->ReagAAresult).length() == 0 || QString::fromLocal8Bit(pdata->ReagAAresult) == "")
    {
        reagcurvedata.insert(AA_REAGENT,"0%,0%,0%,0%");
    }
    else
        reagcurvedata.insert(AA_REAGENT,  QString::fromLocal8Bit(pdata->ReagAAresult));
    if(QString::fromLocal8Bit(pdata->ReagADPresult).length() == 0 || QString::fromLocal8Bit(pdata->ReagADPresult) == "")
    {
        reagcurvedata.insert(ADP_REAGENT,"0%,0%,0%,0%");
    }
    else
        reagcurvedata.insert(ADP_REAGENT,QString::fromLocal8Bit(pdata->ReagADPresult));
    if(QString::fromLocal8Bit(pdata->ReagEPIresult).length() == 0 || QString::fromLocal8Bit(pdata->ReagEPIresult) == "")
    {
        reagcurvedata.insert(EPI_REAGENT,"0%,0%,0%,0%");
    }
    else
        reagcurvedata.insert(EPI_REAGENT,QString::fromLocal8Bit(pdata->ReagEPIresult));
    if(QString::fromLocal8Bit(pdata->ReagCOLresult).length() == 0 || QString::fromLocal8Bit(pdata->ReagCOLresult) == "")
    {
        reagcurvedata.insert(COL_REAGENT,"0%,0%,0%,0%");
    }
    else
        reagcurvedata.insert(COL_REAGENT,QString::fromLocal8Bit(pdata->ReagCOLresult));

    if(QString::fromLocal8Bit(pdata->ReagRISresult).length() == 0 || QString::fromLocal8Bit(pdata->ReagRISresult) == "")
    {
        reagcurvedata.insert(RIS_REAGENT,"0%,0%,0%,0%");
    }
    else
        reagcurvedata.insert(RIS_REAGENT,QString::fromLocal8Bit(pdata->ReagRISresult));
    QVariant curvedata; //申明通用数据对象
    curvedata.setValue(reagcurvedata); //数据封装下
    pwritepdf->_setTestCurvedata(curvedata);
	if (pdata)
		delete pdata;
	pdata = nullptr;
    return;
}

void Inquire_Sql_Info::on_toolButton_outPrint_clicked()
{
    emit _printoutresult();
}


void Inquire_Sql_Info::on_toolButton_creatPdf_clicked()
{
    QString sPath = QFileDialog::getSaveFileName(this, tr("另存为"), "/", tr("Text Files (*.pdf)"));
    if(sPath.isEmpty()){
       return;
    }
    emit this->writepdfprint(sPath);
    return;
}


//初始化日期控件
void Inquire_Sql_Info::initDateEditUI(QDateEdit*dateEdit,QWidget*parent)
{
       dateEdit->setAlignment(Qt::AlignCenter);
       dateEdit->setCalendarPopup(true);
       dateEdit->setDisplayFormat("yyyy-MM-dd");
       dateEdit->setDate(QDate::currentDate());
       QFont font("黑体", 14);
       dateEdit->setFont(font);
       dateEdit->setStyleSheet(QString("QDateEdit{background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, "
                                       "stop:0 rgba(250, 250, 250, 255), "
                                       "stop:0.5 rgba(240, 240, 240, 255), "
                                       "stop:1 rgba(230, 230, 230, 255));"
                                       "border: 1px solid rgb(200, 200, 200);"
                                       "border-radius: 4px;"
                                       "padding-left:%2px;}"
                                       "QDateEdit::drop-down{width:%1px;border-image: url(:/Picture/minus.png);}")
                                   .arg(35).arg(10));
       dateEdit->installEventFilter(parent);
       for(auto lineEdit:dateEdit->findChildren<QLineEdit*>())
       {
           if(lineEdit)
           {
               if(lineEdit->objectName() == "qt_spinbox_lineedit")
               {
                   lineEdit->setReadOnly(true);
                   lineEdit->setFont(font);
                   lineEdit->setAlignment(Qt::AlignCenter);
                   lineEdit->installEventFilter(parent);
                   QLineEdit::connect(lineEdit,&QLineEdit::selectionChanged,[=]
                                      {
                                          lineEdit->deselect();
                                      });
               }
               break;
           }
       }
       QCalendarWidget * cale = dateEdit->calendarWidget();
       cale->setMinimumSize(500,290);
       cale->setFont(font);
       cale->setStyleSheet(QString("QCalendarWidget QWidget#qt_calendar_navigationbar {%1 ;}"
                                   "QCalendarWidget QToolButton {%1;  %9; "
                                   "  height: %3px;  "
                                   "}"
                                   "QCalendarWidget QToolButton QMenu{height:400px;width:200px;background-color:#FFFFFF;border:1px solid #%1;}"
                                   "QCalendarWidget QToolButton#qt_calendar_monthbutton {"
                                   "  width: %10px;"
                                   "}"
                                   "QCalendarWidget QToolButton#qt_calendar_yearbutton {"
                                   "  width: %6px;"
                                   "}"
                                   "QCalendarWidget QToolButton#qt_calendar_prevmonth { margin-right:%7px;"
                                   "  width: %6px;"
                                   " icon-size: %4px, %4px;}"

                                   "QCalendarWidget QToolButton#qt_calendar_nextmonth { margin-left:%7px;"
                                   "  width: %6px;"
                                   " icon-size: %4px, %4px;}"

                                   "QCalendarWidget QMenu {%1;%9; "
                                   "    width: %5px;"
                                   "    left: %7px;"
                                   "}"
                                   "QCalendarWidget QMenu::item {%2;"
                                   "   background-color: transparent;"
                                   "   padding-top:%11px;"
                                   "   padding-bottom:%11px;"
                                   "   padding-left:%3px;"
                                   "   padding-right:%8px;"
                                   "}"
                                   "QCalendarWidget QMenu::item:selected {%2;"
                                   "   background-color: rgb(100, 180, 240);"
                                   "}"
                                   "QCalendarWidget QSpinBox { %1;%9; "
                                   "  width: %6px;"
                                   "    selection-background-color: rgb(16, 130, 220); "
                                   "    selection-color: rgb(255, 255, 255);}"
                                   "QCalendarWidget QSpinBox::up-button { subcontrol-origin: border;  subcontrol-position: top right;  width:%4px; }"
                                   "QCalendarWidget QSpinBox::down-button {subcontrol-origin: border; subcontrol-position: bottom right;  width:%4px;}"
                                   "QCalendarWidget QSpinBox::up-arrow { width:%4px;  height:%4px; }"
                                   "QCalendarWidget QSpinBox::down-arrow { width:%4px;  height:%4px; }"
                                   "QCalendarWidget QWidget {alternate-background-color: rgb(247, 247, 247); }"
                                   "QCalendarWidget QAbstractItemView:enabled { %2; "
                                   "    background-color:  rgb(255, 255, 255);  "
                                   "    selection-background-color: rgb(100, 180, 240); "
                                   "    selection-color: rgb(255, 255, 255); }"
                                   "QCalendarWidget QAbstractItemView:disabled { color: rgb(200, 200, 200); }")
                               .arg("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(250, 250, 250, 255),  stop:0.5 rgba(240, 240, 240, 255), stop:1 rgba(230, 230, 230, 255))")
                               .arg(QString("font: %1pt \"黑体\"; color: rgb(0, 0, 0)").arg(14))
                               .arg(int(50))
                               .arg(int(30))
                               .arg(int(140))
                               .arg(int(200))
                               .arg(int(20))
                               .arg(int(40))
                               .arg(QString("font: %1pt \"黑体\"; color: rgb(0, 0, 0)").arg(18))
                               .arg(int(80))
                               .arg(int(5))
                           ) ;
       QSpinBox*yearEdit = 0;
       QToolButton*yearButton = 0;
       for(auto spinBox:cale->findChildren<QSpinBox*>())
       {
           if(spinBox)
           {
               if(spinBox->objectName() == "qt_calendar_yearedit")
               {
                   yearEdit = spinBox;
                   yearEdit->setAlignment(Qt::AlignCenter);
                   break;
               }
           }
       }
       for(auto btn:cale->findChildren<QToolButton*>())
       {
           if(btn)
           {
               if(btn->objectName() == "qt_calendar_yearbutton")
               {
                   yearButton = btn;
                   break;
               }
           }
       }
       if(yearEdit != 0 && yearButton != 0)
       {
           QSpinBox::connect(yearEdit, &QSpinBox::editingFinished,[=]
               {
                   QTimer::singleShot(10,[=]{
                       yearButton->setText(yearEdit->text());
                   });
               }
           );
       }
   }

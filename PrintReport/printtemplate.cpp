#include "PrintTemplate.h"
#include <QDateTime>
#include <QMessageBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QPageSetupDialog>
#include <QFileDialog>
#include <custom_style/custommessagebox.h>


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// CurveWidget 实现
CurveWidget::CurveWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(400);
    setStyleSheet(QString::fromUtf8("background-color: white;"));
}

void CurveWidget::setCurveData(const QMap<quint8, QVector<double>> &data)
{
    m_curveData = data;
    update();
}

void CurveWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int width = this->width();
    int height = this->height();
    int padding = 60;

    // 绘制背景
    painter.fillRect(rect(), Qt::white);

    if (m_curveData.isEmpty()) {
        painter.drawText(rect(), Qt::AlignCenter, QString::fromUtf8("暂无曲线数据"));
        return;
    }

    // 颜色和试剂名称映射
    QMap<quint8, QColor> reagentColors;
    QMap<quint8, QString> reagentNames;

    // 标准试剂映射
    QVector<QPair<QString, quint8>> standardReagents = {
        {QString::fromUtf8("AA"),  AA_REAGENT},
        {QString::fromUtf8("ADP"), ADP_REAGENT},
        {QString::fromUtf8("EPI"), EPI_REAGENT},
        {QString::fromUtf8("COL"), COL_REAGENT},
        {QString::fromUtf8("RIS"), RIS_REAGENT}
    };
    QVector<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::darkYellow, Qt::magenta};

    // 初始化映射
    for (int i = 0; i < standardReagents.size(); ++i) {
        quint8 reagentId = standardReagents[i].second;
        if (m_curveData.contains(reagentId)) {
            reagentColors[reagentId] = colors[i % colors.size()];
            reagentNames[reagentId] = standardReagents[i].first;
        }
    }

    // 每秒一个点，共300秒300个点
    const int maxDataPoints = 300;
    const double maxTime = 300.0;  // 300秒
    const double minY = -20.0;     // Y轴最小值
    const double maxY = 100.0;     // Y轴最大值

    // 计算绘图区域
    int plotWidth = width - 2 * padding;
    int plotHeight = height - 2 * padding;
    int plotLeft = padding;
    int plotTop = padding;

    // 绘制坐标轴
    QPen axisPen(Qt::black, 2);
    painter.setPen(axisPen);
    painter.drawLine(plotLeft, plotTop + plotHeight, plotLeft + plotWidth, plotTop + plotHeight); // X轴
    painter.drawLine(plotLeft, plotTop, plotLeft, plotTop + plotHeight); // Y轴

    // 绘制网格线
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
    int yGridSteps = 6;  // -20, 0, 20, 40, 60, 80, 100
    int xGridSteps = 6;  // 0, 50, 100, 150, 200, 250, 300

    for (int i = 1; i <= yGridSteps; ++i) {
        double yValue = minY + (maxY - minY) * i / yGridSteps;
        double normalizedY = (yValue - minY) / (maxY - minY);
        int y = plotTop + plotHeight - static_cast<int>(normalizedY * plotHeight);
        painter.drawLine(plotLeft, y, plotLeft + plotWidth, y);
    }

    for (int i = 1; i < xGridSteps; ++i) {
        double xValue = maxTime * i / xGridSteps;
        int x = plotLeft + static_cast<int>(xValue / maxTime * plotWidth);
        painter.drawLine(x, plotTop, x, plotTop + plotHeight);
    }

    // 绘制坐标轴标签
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    // Y轴标签
    for (int i = 0; i <= yGridSteps; ++i) {
        double yValue = minY + (maxY - minY) * i / yGridSteps;
        double normalizedY = (yValue - minY) / (maxY - minY);
        int y = plotTop + plotHeight - static_cast<int>(normalizedY * plotHeight);
        QString label = QString::number(yValue);
        QRect textRect(5, y - 10, 50, 20);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }
    painter.drawText(5, plotTop - 20, 70, 20, Qt::AlignCenter, QString::fromUtf8("聚集率(%)"));

    // X轴标签
    for (int i = 0; i <= xGridSteps; ++i) {
        double xValue = maxTime * i / xGridSteps;
        int x = plotLeft + static_cast<int>(xValue / maxTime * plotWidth);
        QString label = QString::number(xValue);
        QRect textRect(x - 20, plotTop + plotHeight + 5, 40, 20);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }
    painter.drawText(plotLeft + plotWidth / 2 - 30, plotTop + plotHeight + 30, 60, 20, Qt::AlignCenter, QString::fromUtf8("时间(秒)"));

    // 绘制曲线
    for (const auto &reagentId : m_curveData.keys()) {
        if (!reagentColors.contains(reagentId)) {
            continue;
        }

        const QVector<double> &data = m_curveData[reagentId];
        if (data.size() < 2) {
            continue;
        }

        QPen curvePen(reagentColors[reagentId], 2);
        painter.setPen(curvePen);

        QVector<QPointF> points;
        int pointsToDraw = qMin(data.size(), maxDataPoints);
        for (int i = 0; i < pointsToDraw; ++i) {
            double xValue = i;  // 每秒一个点
            double yValue = qBound(minY, data[i], maxY);

            double normalizedX = xValue / maxTime;
            double normalizedY = (yValue - minY) / (maxY - minY);

            double x = plotLeft + normalizedX * plotWidth;
            double y = plotTop + plotHeight - normalizedY * plotHeight;

            points.append(QPointF(x, y));
        }

        for (int i = 1; i < points.size(); ++i) {
            painter.drawLine(points[i - 1], points[i]);
        }

        // 标记关键时间点：60s, 180s, 300s, Max值
        QVector<int> keyTimes = {60, 180, 300};
        for (int time : keyTimes) {
            if (time < pointsToDraw) {
                double xValue = time;
                double yValue = qBound(minY, data[time], maxY);

                double normalizedX = xValue / maxTime;
                double normalizedY = (yValue - minY) / (maxY - minY);

                double x = plotLeft + normalizedX * plotWidth;
                double y = plotTop + plotHeight - normalizedY * plotHeight;

                // 绘制标记点
                painter.setBrush(reagentColors[reagentId]);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(QPointF(x, y), 5, 5);

                // 绘制时间标签
                painter.setPen(Qt::black);
                //painter.drawText(x + 8, y - 8, QString::number(time) + "s");
                painter.setPen(curvePen);
            }
        }

        // 标记最大值点
        if (!data.isEmpty()) {
            double maxValue = *std::max_element(data.begin(), data.end());
            int maxIndex = std::max_element(data.begin(), data.end()) - data.begin();

            if (maxIndex < pointsToDraw) {
                double xValue = maxIndex;
                double yValue = qBound(minY, maxValue, maxY);

                double normalizedX = xValue / maxTime;
                double normalizedY = (yValue - minY) / (maxY - minY);

                double x = plotLeft + normalizedX * plotWidth;
                double y = plotTop + plotHeight - normalizedY * plotHeight;

                // 绘制最大值标记点
                painter.setBrush(Qt::red);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(QPointF(x, y), 6, 6);

                // 绘制最大值标签（显示秒数）
                painter.setPen(Qt::red);
                //painter.drawText(x + 8, y - 8, "Max@" + QString::number(maxIndex) + "s");
                painter.setPen(curvePen);
            }
        }

        // 在曲线末端标记试剂名称（只显示试剂名称本身）
        if (!points.isEmpty() && reagentNames.contains(reagentId)) {
            QPointF lastPoint = points.last();
            QString reagentName = reagentNames[reagentId];

            // 绘制试剂名称标签
            painter.setPen(Qt::black);
            QFont nameFont = painter.font();
            nameFont.setBold(true);
            nameFont.setPointSize(10);
            painter.setFont(nameFont);

            // 在曲线末端右侧标记试剂名称
            painter.drawText(lastPoint.x() + 10, lastPoint.y() + 5, reagentName);

            // 恢复原来的画笔和字体
            painter.setPen(curvePen);
            painter.setFont(font);
        }


    }

}

// PrintTemplate 实现
PrintTemplate::PrintTemplate(QWidget *parent, CustomCreatSql *sql)
    : QWidget(parent)
    , m_sqlDatabase(sql)
{
    setWindowTitle(QString::fromUtf8("检验报告单"));
    setWindowFlags(Qt::Window);
    //setStyleSheet("QWidget { background-color: white; }");
    // 默认显示粘附率
    m_showAdhesionRate = true;

    initUI();
}

PrintTemplate::~PrintTemplate()
{
    QLOG_DEBUG() << "退出打印报告";
}

void PrintTemplate::initUI()
{

    setMinimumSize(900, 1050);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);

    // 1. 医院标题
    QString hospitalName = getHospitalNameFromDB();
    m_hospitalLabel = new QLabel(hospitalName);
    m_hospitalLabel->setAlignment(Qt::AlignCenter);
    QFont hospitalFont = m_hospitalLabel->font();
    hospitalFont.setBold(true);
    hospitalFont.setPointSize(20);
    m_hospitalLabel->setFont(hospitalFont);
    m_mainLayout->addWidget(m_hospitalLabel);

    // 2. 报告标题
    m_reportTitleLabel = new QLabel(QString::fromUtf8("血小板聚集功能检测报告"));
    m_reportTitleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = m_reportTitleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);
    m_reportTitleLabel->setFont(titleFont);
    m_mainLayout->addWidget(m_reportTitleLabel);

    // 3. 报告日期
    m_reportDateLabel = new QLabel(QString::fromUtf8("报告日期: ") + QDateTime::currentDateTime().toString(QString::fromUtf8("yyyy-MM-dd HH:mm:ss")));
    m_reportDateLabel->setAlignment(Qt::AlignRight);
    m_reportDateLabel->setStyleSheet(QString::fromUtf8("font-size: 12px; color: #666;"));
    m_mainLayout->addWidget(m_reportDateLabel);

    m_mainLayout->addSpacing(25);

    // 4. 患者信息区域 - 使用医院报告单格式
    QFrame *patientFrame = new QFrame();
    patientFrame->setFrameStyle(QFrame::NoFrame);
    patientFrame->setStyleSheet(QString::fromUtf8("QFrame { background-color: #fff; }"));

    QGridLayout *patientGrid = new QGridLayout(patientFrame);
    patientGrid->setSpacing(10);
    patientGrid->setContentsMargins(15, 15, 15, 15);

    // 创建标签值对的辅助函数
    auto createLabelPair = [&](const QString &labelText, QLabel *&valueLabel) {
        QLabel *label = new QLabel(labelText);
        label->setStyleSheet(QString::fromUtf8("font-weight: bold; color: #333; font-size: 13px;"));

        valueLabel = new QLabel(QString::fromUtf8("-"));
        valueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 左对齐
        valueLabel->setStyleSheet(QString::fromUtf8("color: #000; font-size: 13px; padding: 3px 5px; background-color: #f9f9f9; border: none;"));
        valueLabel->setMinimumWidth(120);

        return qMakePair(label, valueLabel);
    };

    // 第一行
    auto namePair = createLabelPair(QString::fromUtf8("姓    名:"), m_nameLabel);
    auto sexPair = createLabelPair(QString::fromUtf8("性    别:"), m_sexLabel);
    auto agePair = createLabelPair(QString::fromUtf8("年    龄:"), m_ageLabel);
    auto deptPair = createLabelPair(QString::fromUtf8("科    室:"), m_departmentLabel);

    patientGrid->addWidget(namePair.first, 0, 0);
    patientGrid->addWidget(namePair.second, 0, 1);
    patientGrid->addWidget(sexPair.first, 0, 2);
    patientGrid->addWidget(sexPair.second, 0, 3);
    patientGrid->addWidget(agePair.first, 0, 4);
    patientGrid->addWidget(agePair.second, 0, 5);
    patientGrid->addWidget(deptPair.first, 0, 6);
    patientGrid->addWidget(deptPair.second, 0, 7);

    // 第二行
    auto sampleIdPair = createLabelPair(QString::fromUtf8("样本编号:"), m_sampleIdLabel);
    auto barcodePair = createLabelPair(QString::fromUtf8("条形码号:"), m_barcodeLabel);
    auto submitPair = createLabelPair(QString::fromUtf8("送检时间:"), m_submissionTimeLabel);
    auto inspectPair = createLabelPair(QString::fromUtf8("检验时间:"), m_inspectionTimeLabel);

    patientGrid->addWidget(sampleIdPair.first, 1, 0);
    patientGrid->addWidget(sampleIdPair.second, 1, 1);
    patientGrid->addWidget(barcodePair.first, 1, 2);
    patientGrid->addWidget(barcodePair.second, 1, 3);
    patientGrid->addWidget(submitPair.first, 1, 4);
    patientGrid->addWidget(submitPair.second, 1, 5);
    patientGrid->addWidget(inspectPair.first, 1, 6);
    patientGrid->addWidget(inspectPair.second, 1, 7);

    m_mainLayout->addWidget(patientFrame);

    m_mainLayout->addSpacing(25);

    // 5. 测试结果区域
    QFrame *resultFrame = new QFrame();
    resultFrame->setFrameStyle(QFrame::NoFrame);
    resultFrame->setStyleSheet(QString::fromUtf8("QFrame { background-color: #fff; }"));

    QGridLayout *resultGrid = new QGridLayout(resultFrame);
    resultGrid->setSpacing(10);
    resultGrid->setContentsMargins(15, 15, 15, 15);

    // 表头 - 修复居中问题
    QLabel *headerLabel = new QLabel(QString::fromUtf8("血小板聚集功能检测结果"));
    headerLabel->setAlignment(Qt::AlignCenter);
    QFont headerFont = headerLabel->font();
    headerFont.setBold(true);
    headerFont.setPointSize(14);
    headerLabel->setFont(headerFont);
    resultGrid->addWidget(headerLabel, 0, 0, 1, 11); // 改为11列

    resultGrid->addItem(new QSpacerItem(20, 15), 1, 0, 1, 11);

    // 结果表格表头 - 扩展为12列
    QLabel *testLabel = new QLabel(QString::fromUtf8("测试项目"));
    testLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    testLabel->setAlignment(Qt::AlignCenter);

    QLabel *result60SLabel = new QLabel(QString::fromUtf8("60S(%)"));
    result60SLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    result60SLabel->setAlignment(Qt::AlignCenter);
    QLabel *resul180stLabel = new QLabel(QString::fromUtf8("180(%)"));
    resul180stLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    resul180stLabel->setAlignment(Qt::AlignCenter);
    QLabel *resul300stLabel = new QLabel(QString::fromUtf8("300(%)"));
    resul300stLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    resul300stLabel->setAlignment(Qt::AlignCenter);
    QLabel *resulmaxtLabel = new QLabel(QString::fromUtf8("Max(%)"));
    resulmaxtLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    resulmaxtLabel->setAlignment(Qt::AlignCenter);

    // 新增参数表头
    QLabel *aucLabel = new QLabel(QString::fromUtf8("AUC(%·分钟)"));
    aucLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    aucLabel->setAlignment(Qt::AlignCenter);

    QLabel *maxSlopeLabel = new QLabel(QString::fromUtf8("最陡斜率(%/分钟)"));
    maxSlopeLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    maxSlopeLabel->setAlignment(Qt::AlignCenter);

    QLabel *tmaTimeLabel = new QLabel(QString::fromUtf8("TMA时间(分钟)"));
    tmaTimeLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    tmaTimeLabel->setAlignment(Qt::AlignCenter);

    QLabel *lagTimeLabel = new QLabel(QString::fromUtf8("延迟时间(分钟)"));
    lagTimeLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    lagTimeLabel->setAlignment(Qt::AlignCenter);

    QLabel *adhesionLabel = new QLabel(QString::fromUtf8("粘附率(%)"));
    adhesionLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    adhesionLabel->setAlignment(Qt::AlignCenter);

    QLabel *refLabel = new QLabel(QString::fromUtf8("参考范围"));
    refLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
    refLabel->setAlignment(Qt::AlignCenter);

    // 添加表头到表格
    resultGrid->addWidget(testLabel, 2, 0);
    resultGrid->addWidget(result60SLabel, 2, 1);
    resultGrid->addWidget(resul180stLabel, 2, 2);
    resultGrid->addWidget(resul300stLabel, 2, 3);
    resultGrid->addWidget(resulmaxtLabel, 2, 4);
    resultGrid->addWidget(aucLabel, 2, 5);
    resultGrid->addWidget(maxSlopeLabel, 2, 6);
    resultGrid->addWidget(tmaTimeLabel, 2, 7);
    resultGrid->addWidget(lagTimeLabel, 2, 8);
    resultGrid->addWidget(adhesionLabel, 2, 9);
    resultGrid->addWidget(refLabel, 2, 10);

    // 创建结果行的辅助函数 - 改为11列
    auto createResultRow = [&](int row, const QString &testName, QLabel *&result60SLabel, QLabel *&result180SLabel, QLabel *&result300SLabel, QLabel *&resultMaxLabel,
                               QLabel *&aucLabel, QLabel *&maxSlopeLabel, QLabel *&tmaTimeLabel, QLabel *&lagTimeLabel, QLabel *&adhesionLabel,
                               QLabel *&refRangeLabel) {
        QLabel *testItemLabel = new QLabel(testName);
        testItemLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff;"));
        testItemLabel->setAlignment(Qt::AlignCenter);

        result60SLabel = new QLabel(QString::fromUtf8("-"));
        result60SLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        result60SLabel->setAlignment(Qt::AlignCenter);

        result180SLabel = new QLabel(QString::fromUtf8("-"));
        result180SLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        result180SLabel->setAlignment(Qt::AlignCenter);

        result300SLabel = new QLabel(QString::fromUtf8("-"));
        result300SLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        result300SLabel->setAlignment(Qt::AlignCenter);

        resultMaxLabel = new QLabel(QString::fromUtf8("-"));
        resultMaxLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        resultMaxLabel->setAlignment(Qt::AlignCenter);

        aucLabel = new QLabel(QString::fromUtf8("-"));
        aucLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        aucLabel->setAlignment(Qt::AlignCenter);

        maxSlopeLabel = new QLabel(QString::fromUtf8("-"));
        maxSlopeLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        maxSlopeLabel->setAlignment(Qt::AlignCenter);

        tmaTimeLabel = new QLabel(QString::fromUtf8("-"));
        tmaTimeLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        tmaTimeLabel->setAlignment(Qt::AlignCenter);

        lagTimeLabel = new QLabel(QString::fromUtf8("-"));
        lagTimeLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        lagTimeLabel->setAlignment(Qt::AlignCenter);

        adhesionLabel = new QLabel(QString::fromUtf8("-"));
        adhesionLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        adhesionLabel->setAlignment(Qt::AlignCenter);

        refRangeLabel = new QLabel(QString::fromUtf8("-"));
        refRangeLabel->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;"));
        refRangeLabel->setAlignment(Qt::AlignCenter);

        resultGrid->addWidget(testItemLabel, row, 0);
        resultGrid->addWidget(result60SLabel, row, 1);
        resultGrid->addWidget(result180SLabel, row, 2);
        resultGrid->addWidget(result300SLabel, row, 3);
        resultGrid->addWidget(resultMaxLabel, row, 4);
        resultGrid->addWidget(aucLabel, row, 5);
        resultGrid->addWidget(maxSlopeLabel, row, 6);
        resultGrid->addWidget(tmaTimeLabel, row, 7);
        resultGrid->addWidget(lagTimeLabel, row, 8);
        resultGrid->addWidget(adhesionLabel, row, 9);
        resultGrid->addWidget(refRangeLabel, row, 10);
    };

    // AA
    createResultRow(3, QString::fromUtf8("AA(氨基酸)"), m_aaResult60SLabel, m_aaResult180SLabel, m_aaResult300SLabel, m_aaResultMaxLabel,
                   m_aaAUCLabel, m_aaMaxSlopeLabel, m_aaTMAtimeLabel, m_aaLagTimeLabel, m_aaAdhesionLabel, m_aaRefData);
    // ADP
    createResultRow(4, QString::fromUtf8("ADP(腺苷二磷酸)"), m_adpResult60SLabel, m_adpResult180SLabel, m_adpResult300SLabel, m_adpResultMaxLabel,
                   m_adpAUCLabel, m_adpMaxSlopeLabel, m_adpTMAtimeLabel, m_adpLagTimeLabel, m_adpAdhesionLabel, m_adpRefData);
    // EPI
    createResultRow(5, QString::fromUtf8("EPI(胶原蛋白)"), m_epiResult60SLabel, m_epiResult180SLabel, m_epiResult300SLabel, m_epiResultMaxLabel,
                   m_epiAUCLabel, m_epiMaxSlopeLabel, m_epiTMAtimeLabel, m_epiLagTimeLabel, m_epiAdhesionLabel, m_epiRefData);
    // COL
    createResultRow(6, QString::fromUtf8("COL(肾上腺素)"), m_colResult60SLabel, m_colResult180SLabel, m_colResult300SLabel, m_colResultMaxLabel,
                   m_colAUCLabel, m_colMaxSlopeLabel, m_colTMAtimeLabel, m_colLagTimeLabel, m_colAdhesionLabel, m_colRefData);
    // RIS
    createResultRow(7, QString::fromUtf8("RIS(瑞斯托霉素)"), m_risResult60SLabel, m_risResult180SLabel, m_risResult300SLabel, m_risResultMaxLabel,
                   m_risAUCLabel, m_risMaxSlopeLabel, m_risTMAtimeLabel, m_risLagTimeLabel, m_risAdhesionLabel, m_risRefData);

    m_mainLayout->addWidget(resultFrame);

    m_mainLayout->addSpacing(10);

    // 6. 曲线图区域标题
    QLabel *curveTitleLabel = new QLabel(QString::fromUtf8("测试曲线图"));
    curveTitleLabel->setAlignment(Qt::AlignCenter);
    QFont curveTitleFont = curveTitleLabel->font();
    curveTitleFont.setBold(true);
    curveTitleFont.setPointSize(14);
    curveTitleLabel->setFont(curveTitleFont);
    m_mainLayout->addWidget(curveTitleLabel);

    // 7. 曲线图
    m_curveWidget = new CurveWidget();
    m_mainLayout->addWidget(m_curveWidget);

    m_mainLayout->addStretch();

    // 8. 底部按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_printButton = new QPushButton(QString::fromUtf8("打印报告"));
    m_printButton->setMinimumHeight(40);
    m_printButton->setMinimumWidth(120);
    m_printButton->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #007bff; color: white; border: none; border-radius: 5px; font-size: 14px; font-weight: bold; } QPushButton:hover { background-color: #0056b3; }"));
    connect(m_printButton, &QPushButton::clicked, this, &PrintTemplate::printReport);

    m_exportPDFButton = new QPushButton(QString::fromUtf8("导出PDF"));
    m_exportPDFButton->setMinimumHeight(40);
    m_exportPDFButton->setMinimumWidth(120);
    m_exportPDFButton->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #28a745; color: white; border: none; border-radius: 5px; font-size: 14px; font-weight: bold; } QPushButton:hover { background-color: #218838; }"));
    connect(m_exportPDFButton, &QPushButton::clicked, this, &PrintTemplate::exportToPDF);

    m_closeButton = new QPushButton(QString::fromUtf8("关闭"));
    m_closeButton->setMinimumHeight(40);
    m_closeButton->setMinimumWidth(120);
    m_closeButton->setStyleSheet(QString::fromUtf8("QPushButton { background-color: #6c757d; color: white; border: none; border-radius: 5px; font-size: 14px; font-weight: bold; } QPushButton:hover { background-color: #545b62; }"));
    connect(m_closeButton, &QPushButton::clicked, this, &PrintTemplate::close);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_printButton);
    buttonLayout->addWidget(m_exportPDFButton);
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch();
    m_mainLayout->addLayout(buttonLayout);

    // 初始清空数据
    clearData();
}

void PrintTemplate::setSampleId(const QString &sampleId)
{
    m_sampleId = sampleId;
    if (loadDataFromDatabase()) {
        updateReportDisplay();
        m_curveWidget->setCurveData(m_curveData);
    }
}

QString PrintTemplate::getSampleId() const
{
    return m_sampleId;
}

bool PrintTemplate::loadDataFromDatabase()
{
    if (!m_sqlDatabase) {
        CustomMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("数据库对象为空，无法加载数据"));
        return false;
    }

    // 1. 加载患者信息和测试结果
    if (!m_sqlDatabase->getTestResultTabledata(m_sampleId, m_patientInfo)) {
        CustomMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("无法获取样本信息: ") + m_sampleId);
        return false;
    }

    // 2. 加载曲线数据
    m_curveData.clear();
    const auto &reagentMap = m_sqlDatabase->getReagentMap();
    for (const auto &reagent : reagentMap) {
        QVector<double> curve = m_sqlDatabase->getCurveData(m_sampleId, reagent.second);
        if (!curve.isEmpty()) {
            m_curveData[reagent.second] = curve;
        }
    }

    return true;
}

void PrintTemplate::updateReportDisplay()
{
    // 更新患者信息
    m_nameLabel->setText(m_patientInfo.SampleName);
    m_sexLabel->setText(m_patientInfo.sex_);
    m_ageLabel->setText(QString::number(m_patientInfo.agesample));
    m_departmentLabel->setText(m_patientInfo.department_);
    m_sampleIdLabel->setText(m_patientInfo.sampleId);
    m_barcodeLabel->setText(m_patientInfo.barcodesample);
    m_submissionTimeLabel->setText(m_patientInfo.submissiontime_);
    m_inspectionTimeLabel->setText(m_patientInfo.inspectiontime_);

    // 更新测试结果
    QVector<double> ResultAAData = splitPercentages(m_patientInfo.AAResult);
    QVector<double> ResultADPData = splitPercentages(m_patientInfo.ADPResult);
    QVector<double> ResultEPIData = splitPercentages(m_patientInfo.EPIResult);
    QVector<double> ResultCOLData = splitPercentages(m_patientInfo.COLResult);
    QVector<double> ResultRISData = splitPercentages(m_patientInfo.RISResult);

    // 设置AA试剂的结果
    if (ResultAAData.size() >= 1) m_aaResult60SLabel->setText(QString::number(ResultAAData[0], 'f', 2));
    if (ResultAAData.size() >= 2) m_aaResult180SLabel->setText(QString::number(ResultAAData[1], 'f', 2));
    if (ResultAAData.size() >= 3) m_aaResult300SLabel->setText(QString::number(ResultAAData[2], 'f', 2));
    if (ResultAAData.size() >= 4) m_aaResultMaxLabel->setText(QString::number(ResultAAData[3], 'f', 2));

    // 设置ADP试剂的结果
    if (ResultADPData.size() >= 1) m_adpResult60SLabel->setText(QString::number(ResultADPData[0], 'f', 2));
    if (ResultADPData.size() >= 2) m_adpResult180SLabel->setText(QString::number(ResultADPData[1], 'f', 2));
    if (ResultADPData.size() >= 3) m_adpResult300SLabel->setText(QString::number(ResultADPData[2], 'f', 2));
    if (ResultADPData.size() >= 4) m_adpResultMaxLabel->setText(QString::number(ResultADPData[3], 'f', 2));

    // 设置EPI试剂的结果
    if (ResultEPIData.size() >= 1) m_epiResult60SLabel->setText(QString::number(ResultEPIData[0], 'f', 2));
    if (ResultEPIData.size() >= 2) m_epiResult180SLabel->setText(QString::number(ResultEPIData[1], 'f', 2));
    if (ResultEPIData.size() >= 3) m_epiResult300SLabel->setText(QString::number(ResultEPIData[2], 'f', 2));
    if (ResultEPIData.size() >= 4) m_epiResultMaxLabel->setText(QString::number(ResultEPIData[3], 'f', 2));

    // 设置COL试剂的结果
    if (ResultCOLData.size() >= 1) m_colResult60SLabel->setText(QString::number(ResultCOLData[0], 'f', 2));
    if (ResultCOLData.size() >= 2) m_colResult180SLabel->setText(QString::number(ResultCOLData[1], 'f', 2));
    if (ResultCOLData.size() >= 3) m_colResult300SLabel->setText(QString::number(ResultCOLData[2], 'f', 2));
    if (ResultCOLData.size() >= 4) m_colResultMaxLabel->setText(QString::number(ResultCOLData[3], 'f', 2));

    // 设置RIS试剂的结果
    if (ResultRISData.size() >= 1) m_risResult60SLabel->setText(QString::number(ResultRISData[0], 'f', 2));
    if (ResultRISData.size() >= 2) m_risResult180SLabel->setText(QString::number(ResultRISData[1], 'f', 2));
    if (ResultRISData.size() >= 3) m_risResult300SLabel->setText(QString::number(ResultRISData[2], 'f', 2));
    if (ResultRISData.size() >= 4) m_risResultMaxLabel->setText(QString::number(ResultRISData[3], 'f', 2));

    // 使用AggregationAnalyzer计算并设置AA试剂的参数
    if (!m_curveData.isEmpty() && m_curveData.contains(AA_REAGENT)) {
        const QVector<double>& aaData = m_curveData[AA_REAGENT];
        AAResult aaResult = AggregationAnalyzer::analyzeAA(aaData);
        m_aaAUCLabel->setText(QString::number(aaResult.auc, 'f', 2));
        m_aaMaxSlopeLabel->setText(QString::number(aaResult.maxSlope, 'f', 2));
        m_aaTMAtimeLabel->setText(QString::number(aaResult.tmaTime, 'f', 2)); // 分钟显示
        m_aaLagTimeLabel->setText(QString::number(aaResult.lagTime, 'f', 2)); // 分钟显示
        m_aaAdhesionLabel->setText(QString::number(aaResult.maxValue * 0.8, 'f', 2)); // 最大值的80%
    }

    // 使用AggregationAnalyzer计算并设置ADP试剂的参数
    if (!m_curveData.isEmpty() && m_curveData.contains(ADP_REAGENT)) {
        const QVector<double>& adpData = m_curveData[ADP_REAGENT];
        ADPResult adpResult = AggregationAnalyzer::analyzeADP(adpData);
        m_adpAUCLabel->setText(QString::number(adpResult.auc, 'f', 2));
        m_adpMaxSlopeLabel->setText(QString::number(adpResult.aggregationRate, 'f', 2));
        m_adpTMAtimeLabel->setText(QString::number(adpResult.tmaTime, 'f', 2)); // 分钟显示
        m_adpLagTimeLabel->setText(QString::number(adpResult.lagTime, 'f', 2)); // 分钟显示
        m_adpAdhesionLabel->setText(QString::number(adpResult.maxAggregation * 0.8, 'f', 2)); // 最大值的80%
    }

    // 使用AggregationAnalyzer计算并设置EPI试剂的参数（无粘附率）
    if (!m_curveData.isEmpty() && m_curveData.contains(EPI_REAGENT)) {
        const QVector<double>& epiData = m_curveData[EPI_REAGENT];
        EPIResult epiResult = AggregationAnalyzer::analyzeEPI(epiData);
        m_epiAUCLabel->setText(QString::number(epiResult.auc, 'f', 2));
        m_epiMaxSlopeLabel->setText(QString::number(epiResult.secondPhaseSlope, 'f', 2));
        m_epiTMAtimeLabel->setText(QString::number(epiResult.tmaTime, 'f', 2)); // 分钟显示
        m_epiLagTimeLabel->setText(QString::number(epiResult.lagTime, 'f', 2)); // 分钟显示
        m_epiAdhesionLabel->setText("-"); // EPI无粘附率
    }

    // 使用AggregationAnalyzer计算并设置COL试剂的参数（无粘附率）
    if (!m_curveData.isEmpty() && m_curveData.contains(COL_REAGENT)) {
        const QVector<double>& colData = m_curveData[COL_REAGENT];
        COLResult colResult = AggregationAnalyzer::analyzeCOL(colData);
        m_colAUCLabel->setText(QString::number(colResult.auc, 'f', 2));
        m_colMaxSlopeLabel->setText(QString::number(colResult.maxSlope, 'f', 2));
        m_colTMAtimeLabel->setText(QString::number(colResult.tmaTime, 'f', 2)); // 分钟显示
        m_colLagTimeLabel->setText(QString::number(colResult.lagTime, 'f', 2)); // 分钟显示
        m_colAdhesionLabel->setText("-"); // COL无粘附率
    }

    // 使用AggregationAnalyzer计算并设置RIS试剂的参数（无粘附率）
    if (!m_curveData.isEmpty() && m_curveData.contains(RIS_REAGENT)) {
        const QVector<double>& risData = m_curveData[RIS_REAGENT];
        RISResult risResult = AggregationAnalyzer::analyzeRIS(risData);
        m_risAUCLabel->setText(QString::number(risResult.auc, 'f', 2));
        m_risMaxSlopeLabel->setText(QString::number(risResult.aggregationRate, 'f', 2));
        m_risTMAtimeLabel->setText(QString::number(risResult.tmaTime, 'f', 2)); // 分钟显示
        m_risLagTimeLabel->setText(QString::number(risResult.lagTime, 'f', 2)); // 分钟显示
        m_risAdhesionLabel->setText("-"); // RIS无粘附率
    }

    // 根据结果是否异常设置颜色
    auto checkResult = [this](QLabel *label,double value, const QString &reagentName,
            double defaultLow, double defaultHigh) {
        QString originalText = QString::number(value, 'f', 2);
        // 获取参考值范围
        QString refRange = getReferenceRangeFromDB(reagentName);
        QPair<double, double> range = parseReferenceRange(refRange);

        // 如果解析失败，使用默认值
        double low = range.first;
        double high = range.second;
        if (low == 0 && high == 0) {
            low = defaultLow;
            high = defaultHigh;
        }

        QString mark;
        if (value < low) {
            mark = QString::fromUtf8(" ↓");
            label->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #ffebee; font-weight: bold; color: #c62828;"));
        } else if (value > high) {
            mark = QString::fromUtf8(" ↑");
            label->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #ffebee; font-weight: bold; color: #c62828;"));
        } else {
            mark = QString::fromUtf8(" ✓");
            label->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold; color: #2e7d32;"));
        }

        // 设置带标记的文本
        label->setText(originalText + mark);
    };

    // 检查最大聚集率并添加标记的函数 + 参照值
    auto checkMaxResultWithMark = [this](QLabel *label, QLabel *Reflabel,double value,
            const QString &reagentName, double defaultLow, double defaultHigh) {
        // 先设置原始文本
        QString originalText = QString::number(value, 'f', 2);

        // 获取参考值范围
        QString refRange = getReferenceRangeFromDB(reagentName);
        QPair<double, double> range = parseReferenceRange(refRange);

        // 如果解析失败，使用默认值
        double low = range.first;
        double high = range.second;
        if (low == 0 && high == 0) {
            low = defaultLow;
            high = defaultHigh;
        }

        // 添加标记
        QString mark;
        if (value < low) {
            mark = QString::fromUtf8(" ↓");
            label->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #ffebee; font-weight: bold; color: #c62828;"));
        } else if (value > high) {
            mark = QString::fromUtf8(" ↑");
            label->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #ffebee; font-weight: bold; color: #c62828;"));
        } else {
            mark = QString::fromUtf8(" ✓");
            label->setStyleSheet(QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold; color: #2e7d32;"));
        }

        // 设置带标记的文本
        label->setText(originalText + mark);
        Reflabel->setText(QString::number(low) +"-"+ QString::number(high));
    };

    // 检查AA试剂各时间点的结果
    if (ResultAAData.size() >= 1){
        checkResult(m_aaResult60SLabel, ResultAAData[0],QString::fromUtf8("AA聚集率-1"),30, 70);
    }
    if (ResultAAData.size() >= 2) {
        checkResult(m_aaResult180SLabel, ResultAAData[1],QString::fromUtf8("AA聚集率-2"),30, 70);
    }
    if (ResultAAData.size() >= 3){
        checkResult(m_aaResult300SLabel, ResultAAData[2],QString::fromUtf8("AA聚集率-3"), 30, 70);
    }
    if (ResultAAData.size() >= 4) {
        // 对Max值使用带标记的检查
        m_aaResultMaxLabel->setText(QString::number(ResultAAData[3], 'f', 2)); // 先设置原始值
        checkMaxResultWithMark(m_aaResultMaxLabel, m_aaRefData,
                               ResultAAData[3], QString::fromUtf8("AA聚集率-Max"), 30, 70);
    }

    // 检查ADP试剂各时间点的结果
    if (ResultADPData.size() >= 1){
        checkResult(m_adpResult60SLabel, ResultADPData[0],QString::fromUtf8("ADP聚集率-1"), 40, 80);
    }
    if (ResultADPData.size() >= 2) {
        checkResult(m_adpResult180SLabel, ResultADPData[1], QString::fromUtf8("ADP聚集率-2"),40, 80);
    }
    if (ResultADPData.size() >= 3){
        checkResult(m_adpResult300SLabel, ResultADPData[2], QString::fromUtf8("ADP聚集率-3"),40, 80);
    }
    if (ResultADPData.size() >= 4) {
        m_adpResultMaxLabel->setText(QString::number(ResultADPData[3], 'f', 2));
        checkMaxResultWithMark(m_adpResultMaxLabel, m_adpRefData, ResultADPData[3],
                               QString::fromUtf8("ADP聚集率-Max"), 40, 80);
    }

    // 检查EPI试剂各时间点的结果
    if (ResultEPIData.size() >= 1) {
        checkResult(m_epiResult60SLabel, ResultEPIData[0],QString::fromUtf8("EPI聚集率-1"),35, 75);
    }
    if (ResultEPIData.size() >= 2) {
        checkResult(m_epiResult180SLabel, ResultEPIData[1],QString::fromUtf8("EPI聚集率-2"), 35, 75);
    }
    if (ResultEPIData.size() >= 3) {
        checkResult(m_epiResult300SLabel, ResultEPIData[2],QString::fromUtf8("EPI聚集率-3"),35, 75);
    }
    if (ResultEPIData.size() >= 4) {
        m_epiResultMaxLabel->setText(QString::number(ResultEPIData[3], 'f', 2));
        checkMaxResultWithMark(m_epiResultMaxLabel, m_epiRefData,
                               ResultEPIData[3], QString::fromUtf8("EPI聚集率-Max"), 35, 75);
    }

    // 检查COL试剂各时间点的结果
    if (ResultCOLData.size() >= 1) {
        checkResult(m_colResult60SLabel, ResultCOLData[0], QString::fromUtf8("COL聚集率-1"),50, 90);
    }
    if (ResultCOLData.size() >= 2) {
        checkResult(m_colResult180SLabel, ResultCOLData[1], QString::fromUtf8("COL聚集率-2"),50, 90);
    }
    if (ResultCOLData.size() >= 3) {
        checkResult(m_colResult300SLabel, ResultCOLData[2], QString::fromUtf8("COL聚集率-3"),50, 90);
    }
    if (ResultCOLData.size() >= 4) {
        m_colResultMaxLabel->setText(QString::number(ResultCOLData[3], 'f', 2));
        checkMaxResultWithMark(m_colResultMaxLabel, m_colRefData,
                               ResultCOLData[3], QString::fromUtf8("COL聚集率-Max"), 50, 90);
    }

    // 检查RIS试剂各时间点的结果
    if (ResultRISData.size() >= 1) {
        checkResult(m_risResult60SLabel, ResultRISData[0],QString::fromUtf8("RIS聚集率-1"), 45, 85);
    }
    if (ResultRISData.size() >= 2) {
        checkResult(m_risResult180SLabel, ResultRISData[1], QString::fromUtf8("RIS聚集率-2"),45, 85);
    }
    if (ResultRISData.size() >= 3) {
        checkResult(m_risResult300SLabel, ResultRISData[2], QString::fromUtf8("RIS聚集率-3"),45, 85);
    }
    if (ResultRISData.size() >= 4) {
        m_risResultMaxLabel->setText(QString::number(ResultRISData[3], 'f', 2));
        checkMaxResultWithMark(m_risResultMaxLabel,m_risRefData,
                               ResultRISData[3], QString::fromUtf8("RIS聚集率-Max"), 45, 85);
    }
}

void PrintTemplate::clearData()
{
    m_nameLabel->setText(QString::fromUtf8("-"));
    m_sexLabel->setText(QString::fromUtf8("-"));
    m_ageLabel->setText(QString::fromUtf8("-"));
    m_departmentLabel->setText(QString::fromUtf8("-"));
    m_sampleIdLabel->setText(QString::fromUtf8("-"));
    m_barcodeLabel->setText(QString::fromUtf8("-"));
    m_submissionTimeLabel->setText(QString::fromUtf8("-"));
    m_inspectionTimeLabel->setText(QString::fromUtf8("-"));

    // 清空AA试剂结果
    m_aaResult60SLabel->setText(QString::fromUtf8("-"));
    m_aaResult180SLabel->setText(QString::fromUtf8("-"));
    m_aaResult300SLabel->setText(QString::fromUtf8("-"));
    m_aaResultMaxLabel->setText(QString::fromUtf8("-"));

    // 清空ADP试剂结果
    m_adpResult60SLabel->setText(QString::fromUtf8("-"));
    m_adpResult180SLabel->setText(QString::fromUtf8("-"));
    m_adpResult300SLabel->setText(QString::fromUtf8("-"));
    m_adpResultMaxLabel->setText(QString::fromUtf8("-"));

    // 清空EPI试剂结果
    m_epiResult60SLabel->setText(QString::fromUtf8("-"));
    m_epiResult180SLabel->setText(QString::fromUtf8("-"));
    m_epiResult300SLabel->setText(QString::fromUtf8("-"));
    m_epiResultMaxLabel->setText(QString::fromUtf8("-"));

    // 清空COL试剂结果
    m_colResult60SLabel->setText(QString::fromUtf8("-"));
    m_colResult180SLabel->setText(QString::fromUtf8("-"));
    m_colResult300SLabel->setText(QString::fromUtf8("-"));
    m_colResultMaxLabel->setText(QString::fromUtf8("-"));

    // 清空RIS试剂结果
    m_risResult60SLabel->setText(QString::fromUtf8("-"));
    m_risResult180SLabel->setText(QString::fromUtf8("-"));
    m_risResult300SLabel->setText(QString::fromUtf8("-"));
    m_risResultMaxLabel->setText(QString::fromUtf8("-"));

    // 清空AA试剂参数
    m_aaAUCLabel->setText(QString::fromUtf8("-"));
    m_aaMaxSlopeLabel->setText(QString::fromUtf8("-"));
    m_aaTMAtimeLabel->setText(QString::fromUtf8("-"));
    m_aaLagTimeLabel->setText(QString::fromUtf8("-"));
    m_aaAdhesionLabel->setText(QString::fromUtf8("-"));

    // 清空ADP试剂参数
    m_adpAUCLabel->setText(QString::fromUtf8("-"));
    m_adpMaxSlopeLabel->setText(QString::fromUtf8("-"));
    m_adpTMAtimeLabel->setText(QString::fromUtf8("-"));
    m_adpLagTimeLabel->setText(QString::fromUtf8("-"));
    m_adpAdhesionLabel->setText(QString::fromUtf8("-"));

    // 清空EPI试剂参数
    m_epiAUCLabel->setText(QString::fromUtf8("-"));
    m_epiMaxSlopeLabel->setText(QString::fromUtf8("-"));
    m_epiTMAtimeLabel->setText(QString::fromUtf8("-"));
    m_epiLagTimeLabel->setText(QString::fromUtf8("-"));
    m_epiAdhesionLabel->setText(QString::fromUtf8("-"));

    // 清空COL试剂参数
    m_colAUCLabel->setText(QString::fromUtf8("-"));
    m_colMaxSlopeLabel->setText(QString::fromUtf8("-"));
    m_colTMAtimeLabel->setText(QString::fromUtf8("-"));
    m_colLagTimeLabel->setText(QString::fromUtf8("-"));
    m_colAdhesionLabel->setText(QString::fromUtf8("-"));

    // 清空RIS试剂参数
    m_risAUCLabel->setText(QString::fromUtf8("-"));
    m_risMaxSlopeLabel->setText(QString::fromUtf8("-"));
    m_risTMAtimeLabel->setText(QString::fromUtf8("-"));
    m_risLagTimeLabel->setText(QString::fromUtf8("-"));
    m_risAdhesionLabel->setText(QString::fromUtf8("-"));

    // 重置样式
    QString normalStyle = QString::fromUtf8("padding: 8px; border: none; background-color: #fff; font-weight: bold;");

    // AA试剂
    m_aaResult60SLabel->setStyleSheet(normalStyle);
    m_aaResult180SLabel->setStyleSheet(normalStyle);
    m_aaResult300SLabel->setStyleSheet(normalStyle);
    m_aaResultMaxLabel->setStyleSheet(normalStyle);

    // ADP试剂
    m_adpResult60SLabel->setStyleSheet(normalStyle);
    m_adpResult180SLabel->setStyleSheet(normalStyle);
    m_adpResult300SLabel->setStyleSheet(normalStyle);
    m_adpResultMaxLabel->setStyleSheet(normalStyle);

    // EPI试剂
    m_epiResult60SLabel->setStyleSheet(normalStyle);
    m_epiResult180SLabel->setStyleSheet(normalStyle);
    m_epiResult300SLabel->setStyleSheet(normalStyle);
    m_epiResultMaxLabel->setStyleSheet(normalStyle);

    // COL试剂
    m_colResult60SLabel->setStyleSheet(normalStyle);
    m_colResult180SLabel->setStyleSheet(normalStyle);
    m_colResult300SLabel->setStyleSheet(normalStyle);
    m_colResultMaxLabel->setStyleSheet(normalStyle);

    // RIS试剂
    m_risResult60SLabel->setStyleSheet(normalStyle);
    m_risResult180SLabel->setStyleSheet(normalStyle);
    m_risResult300SLabel->setStyleSheet(normalStyle);
    m_risResultMaxLabel->setStyleSheet(normalStyle);


    m_curveWidget->setCurveData(QMap<quint8, QVector<double>>());
}


QVector<double> PrintTemplate::splitPercentages(const QString& input)
{
    QVector<double> result;

    #ifdef QT_VERSION_CHECK
    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        // Qt 5.14+ 版本
        QStringList items = input.split(',', Qt::SkipEmptyParts);
    #else
        // Qt 5.14 以下版本
        QStringList items = input.split(',', QString::SkipEmptyParts);
    #endif
#else
    // 最兼容的方式
    QStringList items = input.split(',');
#endif

    for (const QString& item : items) {
        // 去除前后空格
        QString trimmed = item.trimmed();

        // 去掉末尾的百分号
        if (trimmed.endsWith('%')) {
            trimmed.chop(1);  // 删除最后一个字符
        }

        // 转换为double
        bool ok;
        double value = trimmed.toDouble(&ok);
        if (ok) {
            result.append(value);
        } else {
            qWarning() << "转换失败:" << trimmed;
        }
    }

    return result;
}















void PrintTemplate::printReport()
{
    QPrinter printer(QPrinter::HighResolution);

    // 设置打印机为A4纸张，纵向打印
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize(QPageSize::A5));

    // 显示页面设置对话框
    QPageSetupDialog pageDialog(&printer, this);
    if (pageDialog.exec() == QDialog::Accepted) {
        // 显示打印对话框
        QPrintDialog dialog(&printer, this);
        if (dialog.exec() == QDialog::Accepted) {
            QPainter painter(&printer);
            painter.setRenderHint(QPainter::Antialiasing);

            // 获取页面尺寸和边距信息
            QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);
            QMarginsF margins = printer.pageLayout().margins(QPageLayout::Millimeter);

            // 计算实际可打印区域（减去边距）
            QRectF printableRect(
                margins.left() * printer.resolution() / 25.4, // 毫米转像素
                margins.top() * printer.resolution() / 25.4,
                pageRect.width() - (margins.left() + margins.right()) * printer.resolution() / 25.4,
                pageRect.height() - (margins.top() + margins.bottom()) * printer.resolution() / 25.4
            );

            // 创建一个临时widget用于打印（不包含按钮）
            QWidget printWidget;
            QVBoxLayout printLayout(&printWidget);
            printLayout.setSpacing(5); // 减少间距

            // 复制主要布局内容（不包含按钮）
            for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
                QLayoutItem *item = m_mainLayout->itemAt(i);
                if (item && item->widget()) {
                    // 创建副本widget
                    QWidget *originalWidget = item->widget();
                    QWidget *copyWidget = new QWidget(&printWidget);
                    copyWidget->setFixedSize(originalWidget->size());

                    // 渲染原始widget到副本
                    QPixmap pixmap(originalWidget->size());
                    originalWidget->render(&pixmap);

                    QLabel *label = new QLabel(&printWidget);
                    label->setPixmap(pixmap);
                    label->setAlignment(Qt::AlignTop); // 顶部对齐
                    printLayout.addWidget(label);
                }
            }

            // 设置打印widget的大小，更精确地计算内容高度
            int contentHeight = 0;
            for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
                QLayoutItem *item = m_mainLayout->itemAt(i);
                if (item && item->widget()) {
                    contentHeight += item->widget()->height();
                }
            }
            contentHeight += (m_mainLayout->count() - 2) * printLayout.spacing(); // 加上间距

            printWidget.setFixedSize(width(), contentHeight);

            // 计算缩放比例，使内容更好地适应可打印区域
            double xscale = printableRect.width() / double(printWidget.width());
            double yscale = printableRect.height() / double(printWidget.height());
            double scale = qMin(xscale, yscale) * 0.95; // 稍微减少边距，充分利用空间

            // 应用更精确的变换，减少顶部留白
            painter.translate(printableRect.left() + printableRect.width() / 2,
                             printableRect.top() + printableRect.height() / 2);
            painter.scale(scale, scale);
            painter.translate(-printWidget.width() / 2, -printWidget.height() / 2);

            // 绘制内容（不包含按钮）
            printWidget.render(&painter);
            painter.end();

            CustomMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("打印完成!"));
        }
    }
}

// 从数据库获取医院名
QString PrintTemplate::getHospitalNameFromDB()
{
    const QString hospitalName = "hospital_name";
    QString titleHospital = m_sqlDatabase->FindPassword(hospitalName);
    return titleHospital + QString::fromUtf8("医院检验科报告单");
}

// 从数据库获取参考值
QString PrintTemplate::getReferenceRangeFromDB(const QString &reagentName)
{
    QString manrefVal ="",womanrefVal = "";
    m_sqlDatabase->obtainPersondata(reagentName,manrefVal,womanrefVal);
    const QString sexindex = m_sexLabel->text();
    if(sexindex == QString::fromUtf8("男"))
    {
        return manrefVal;
    }
    else if(sexindex == QString::fromUtf8("女"))
    {
        return womanrefVal;
    }
    else
        return QString::fromUtf8("未设置性别");
    return QString::fromUtf8("-");
}

// 解析参考值范围
QPair<double, double> PrintTemplate::parseReferenceRange(const QString &refRange)
{
    QPair<double, double> result(0, 0);

    // 先去掉单位部分（括号及括号内的内容）
    QString cleanedRef = refRange;
    int leftBracket = cleanedRef.indexOf("(");
    if (leftBracket != -1) {
        cleanedRef = cleanedRef.left(leftBracket);
    }

    // 然后按"-"分割
    QStringList parts = cleanedRef.split("-");
    if (parts.size() == 2) {
        bool ok1, ok2;
        double low = parts[0].toDouble(&ok1);
        double high = parts[1].toDouble(&ok2);
        if (ok1 && ok2) {
            result.first = low;
            result.second = high;
        }
    }

    return result;
}

// 设置粘附率显示状态
void PrintTemplate::setShowAdhesionRate(bool show)
{
    m_showAdhesionRate = show;

    // 更新显示状态
    if (m_showAdhesionRate) {
        // 显示粘附率列
        m_aaAdhesionLabel->show();
        m_adpAdhesionLabel->show();
        m_epiAdhesionLabel->show();
        m_colAdhesionLabel->show();
        m_risAdhesionLabel->show();
    } else {
        // 隐藏粘附率列
        m_aaAdhesionLabel->hide();
        m_adpAdhesionLabel->hide();
        m_epiAdhesionLabel->hide();
        m_colAdhesionLabel->hide();
        m_risAdhesionLabel->hide();
    }
}

void PrintTemplate::exportToPDF()
{
    // 获取默认保存路径
    QString defaultFileName = QString("血小板聚集功能检测报告_%1.pdf").arg(m_sampleId);
    QString filePath = QFileDialog::getSaveFileName(this,
                                                   QString::fromUtf8("导出PDF报告"),
                                                   defaultFileName,
                                                   QString::fromUtf8("PDF文件 (*.pdf)"));

    if (filePath.isEmpty()) {
        return; // 用户取消了操作
    }

    // 确保文件扩展名正确
    if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
        filePath += ".pdf";
    }

    // 创建PDF打印机
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);

    // 设置打印机为A4纸张，纵向打印
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize(QPageSize::A5));

    QPainter painter(&printer);
    painter.setRenderHint(QPainter::Antialiasing);

    // 获取页面尺寸和边距信息
    QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);
    QMarginsF margins = printer.pageLayout().margins(QPageLayout::Millimeter);

    // 计算实际可打印区域（减去边距）
    QRectF printableRect(
        margins.left() * printer.resolution() / 25.4, // 毫米转像素
        margins.top() * printer.resolution() / 25.4,
        pageRect.width() - (margins.left() + margins.right()) * printer.resolution() / 25.4,
        pageRect.height() - (margins.top() + margins.bottom()) * printer.resolution() / 25.4
    );

    // 创建一个临时widget用于PDF导出（不包含按钮）
    QWidget pdfWidget;
    QVBoxLayout pdfLayout(&pdfWidget);
    pdfLayout.setSpacing(5); // 减少间距

    // 复制主要布局内容（不包含按钮）
    for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
        QLayoutItem *item = m_mainLayout->itemAt(i);
        if (item && item->widget()) {
            // 创建副本widget
            QWidget *originalWidget = item->widget();

            // 渲染原始widget到pixmap
            QPixmap pixmap(originalWidget->size());
            originalWidget->render(&pixmap);

            QLabel *label = new QLabel(&pdfWidget);
            label->setPixmap(pixmap);
            label->setAlignment(Qt::AlignTop); // 顶部对齐
            pdfLayout.addWidget(label);
        }
    }

    // 设置PDF widget的大小，使用实际内容高度
    int contentHeight = 0;
    for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
        QLayoutItem *item = m_mainLayout->itemAt(i);
        if (item && item->widget()) {
            contentHeight += item->widget()->height();
        }
    }
    contentHeight += (m_mainLayout->count() - 2) * pdfLayout.spacing(); // 加上间距

    pdfWidget.setFixedSize(width(), contentHeight);

    // 计算缩放比例，使内容更好地适应可打印区域
    double xscale = printableRect.width() / double(pdfWidget.width());
    double yscale = printableRect.height() / double(pdfWidget.height());
    double scale = qMin(xscale, yscale) * 0.95; // 稍微减少边距，充分利用空间

    // 应用更精确的变换，减少顶部留白
    painter.translate(printableRect.left() + printableRect.width() / 2,
                     printableRect.top() + printableRect.height() / 2);
    painter.scale(scale, scale);
    painter.translate(-pdfWidget.width() / 2, -pdfWidget.height() / 2);

    // 绘制内容（不包含按钮）
    pdfWidget.render(&painter);
    painter.end();

    CustomMessageBox::information(this,
                           QString::fromUtf8("成功"),
                           QString::fromUtf8("PDF导出完成!\n文件保存位置: %1").arg(filePath));
}

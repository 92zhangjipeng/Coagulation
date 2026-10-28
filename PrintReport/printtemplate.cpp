#include "PrintTemplate.h"
#include "ini_file.h"
#include <QDateTime>
#include <QMessageBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QPageSetupDialog>
#include <QFileDialog>
#include <custom_style/custommessagebox.h>
#include <QScrollArea>
#include <QDesktopWidget>
#include <QApplication>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// ==================== CurveWidget 实现 ====================
CurveWidget::CurveWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(280);
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

    for (int i = 0; i < standardReagents.size(); ++i) {
        quint8 reagentId = standardReagents[i].second;
        if (m_curveData.contains(reagentId)) {
            reagentColors[reagentId] = colors[i % colors.size()];
            reagentNames[reagentId] = standardReagents[i].first;
        }
    }

    const int maxDataPoints = 300;
    const double maxTime = 300.0;
    const double minY = -20.0;
    const double maxY = 100.0;

    int plotWidth = width - 2 * padding;
    int plotHeight = height - 2 * padding;
    int plotLeft = padding;
    int plotTop = padding;

    // 坐标轴
    QPen axisPen(Qt::black, 2);
    painter.setPen(axisPen);
    painter.drawLine(plotLeft, plotTop + plotHeight, plotLeft + plotWidth, plotTop + plotHeight);
    painter.drawLine(plotLeft, plotTop, plotLeft, plotTop + plotHeight);

    // 网格线
    painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
    int yGridSteps = 6;
    int xGridSteps = 6;

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

    // 轴标签
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    for (int i = 0; i <= yGridSteps; ++i) {
        double yValue = minY + (maxY - minY) * i / yGridSteps;
        double normalizedY = (yValue - minY) / (maxY - minY);
        int y = plotTop + plotHeight - static_cast<int>(normalizedY * plotHeight);
        QString label = QString::number(yValue);
        QRect textRect(5, y - 10, 50, 20);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }
    painter.drawText(5, plotTop - 20, 70, 20, Qt::AlignCenter, QString::fromUtf8("聚集率(%"));

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
        if (!reagentColors.contains(reagentId)) continue;
        const QVector<double> &data = m_curveData[reagentId];
        if (data.size() < 2) continue;

        QPen curvePen(reagentColors[reagentId], 2);
        painter.setPen(curvePen);

        QVector<QPointF> points;
        int pointsToDraw = qMin(data.size(), maxDataPoints);
        for (int i = 0; i < pointsToDraw; ++i) {
            double xValue = i;
            double yValue = qBound(minY, data[i], maxY);
            double normalizedX = xValue / maxTime;
            double normalizedY = (yValue - minY) / (maxY - minY);
            double x = plotLeft + normalizedX * plotWidth;
            double y = plotTop + plotHeight - normalizedY * plotHeight;
            points.append(QPointF(x, y));
        }

        for (int i = 1; i < points.size(); ++i)
            painter.drawLine(points[i - 1], points[i]);

        // 标记关键时间点和最大值
        QVector<int> keyTimes = {60, 180, 300};
        for (int time : keyTimes) {
            if (time < pointsToDraw) {
                double xValue = time;
                double yValue = qBound(minY, data[time], maxY);
                double normalizedX = xValue / maxTime;
                double normalizedY = (yValue - minY) / (maxY - minY);
                double x = plotLeft + normalizedX * plotWidth;
                double y = plotTop + plotHeight - normalizedY * plotHeight;
                painter.setBrush(reagentColors[reagentId]);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(QPointF(x, y), 5, 5);
                painter.setPen(curvePen);
            }
        }

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
                painter.setBrush(Qt::red);
                painter.drawEllipse(QPointF(x, y), 6, 6);
                painter.setPen(curvePen);
            }
        }

        // 曲线末端标记试剂名称
        if (!points.isEmpty() && reagentNames.contains(reagentId)) {
            QPointF lastPoint = points.last();
            painter.setPen(Qt::black);
            QFont nameFont = painter.font();
            nameFont.setBold(true);
            nameFont.setPointSize(10);
            painter.setFont(nameFont);
            painter.drawText(lastPoint.x() + 10, lastPoint.y() + 5, reagentNames[reagentId]);
            painter.setPen(curvePen);
            painter.setFont(font);
        }
    }
}

// ==================== PrintTemplate 实现 ====================
PrintTemplate::PrintTemplate(QWidget *parent, CustomCreatSql *sql)
    : QWidget(parent)
    , m_sqlDatabase(sql)
    , m_patientFrame(nullptr)
    , m_showAdhesionRate(true)
{
    setWindowTitle(QString::fromUtf8("检验报告单"));
    setWindowFlags(Qt::Window);

    // 初始化支持的试剂列表（顺序即为表格显示顺序）
    m_reagents = {
        { AA_REAGENT,  QString::fromUtf8("AA(花生四烯酸)"),   QString::fromUtf8("AA聚集率-Max"), 30, 70 },
        { ADP_REAGENT, QString::fromUtf8("ADP(二磷酸腺苷)"), QString::fromUtf8("ADP聚集率-Max"), 40, 80 },
        { EPI_REAGENT, QString::fromUtf8("EPI(肾上腺素)"), QString::fromUtf8("EPI聚集率-Max"), 35, 75 },
        { COL_REAGENT, QString::fromUtf8("COL(胶原蛋白)"), QString::fromUtf8("COL聚集率-Max"), 50, 90 },
        { RIS_REAGENT, QString::fromUtf8("RIS(瑞斯托霉素)"), QString::fromUtf8("RIS聚集率-Max"), 45, 85 }
    };

    initUI();
}

PrintTemplate::~PrintTemplate()
{
    QLOG_DEBUG() << "退出打印报告";
}

void PrintTemplate::initUI()
{
    QWidget *page = new QWidget(this);
    m_mainLayout = new QVBoxLayout(page);
    m_mainLayout->setSpacing(8);
    m_mainLayout->setContentsMargins(16, 8, 16, 8);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(page);

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(8, 8, 8, 8);
    outer->setSpacing(6);
    outer->addWidget(scroll, 1);

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
    m_mainLayout->addSpacing(8);

    // 4. 患者信息区域
    m_patientFrame = new QFrame();
    m_patientFrame->setFrameStyle(QFrame::NoFrame);
    m_patientFrame->setStyleSheet(QString::fromUtf8("QFrame { background-color: #fff; }"));
    QGridLayout *patientGrid = new QGridLayout(m_patientFrame);
    patientGrid->setSpacing(10);
    patientGrid->setContentsMargins(15, 15, 15, 15);

    auto createLabelPair = [&](const QString &labelText, QLabel *&valueLabel) {
        QLabel *label = new QLabel(labelText);
        label->setStyleSheet(QString::fromUtf8("font-weight: bold; color: #333; font-size: 13px;"));
        valueLabel = new QLabel(QString::fromUtf8("-"));
        valueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
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

    auto referPair = createLabelPair(QString::fromUtf8("送检医生:"), m_referDoctorLabel);
    auto reviewPair = createLabelPair(QString::fromUtf8("审核医生:"), m_reviewDoctorLabel);
    patientGrid->addWidget(referPair.first, 2, 0);
    patientGrid->addWidget(referPair.second, 2, 1);
    patientGrid->addWidget(reviewPair.first, 2, 2);
    patientGrid->addWidget(reviewPair.second, 2, 3);

    m_mainLayout->addWidget(m_patientFrame);
    m_mainLayout->addSpacing(8);

    // 5. 测试结果区域（动态表格）
    m_resultFrame = new QFrame();
    m_resultFrame->setFrameStyle(QFrame::NoFrame);
    m_resultFrame->setStyleSheet(QString::fromUtf8("QFrame { background-color: #fff; }"));
    m_resultGrid = new QGridLayout(m_resultFrame);
    m_resultGrid->setSpacing(10);
    m_resultGrid->setContentsMargins(15, 15, 15, 15);

    // 大标题
    QLabel *headerLabel = new QLabel(QString::fromUtf8("血小板聚集功能检测结果"));
    headerLabel->setAlignment(Qt::AlignCenter);
    QFont headerFont = headerLabel->font();
    headerFont.setBold(true);
    headerFont.setPointSize(14);
    headerLabel->setFont(headerFont);
    m_resultGrid->addWidget(headerLabel, 0, 0, 1, 11);
    m_resultGrid->addItem(new QSpacerItem(20, 15), 1, 0, 1, 11);

    // 表头行（第2行）
    QStringList headers = {
        QString::fromUtf8("测试项目"), QString::fromUtf8("60S(%)"), QString::fromUtf8("180(%)"),
        QString::fromUtf8("300(%)"), QString::fromUtf8("Max(%)"), QString::fromUtf8("AUC(%·分钟)"),
        QString::fromUtf8("最陡斜率(%/分钟)"), QString::fromUtf8("TMA时间(分钟)"),
        QString::fromUtf8("延迟时间(分钟)"), QString::fromUtf8("粘附率(%)"), QString::fromUtf8("参考范围")
    };
    m_columnHeaders.clear();
    for (int col = 0; col < headers.size(); ++col) {
        QLabel *label = new QLabel(headers[col]);
        label->setStyleSheet(QString::fromUtf8("font-weight: bold; background-color: #e0e0e0; padding: 8px; border: none;"));
        label->setAlignment(Qt::AlignCenter);
        m_resultGrid->addWidget(label, 2, col);
        m_columnHeaders.append(label);
        if (col == 9) m_adhesionHeaderLabel = label;
    }

    m_mainLayout->addWidget(m_resultFrame);
    m_mainLayout->addSpacing(10);

    // 6. 曲线图区域（可由勾选控制显示/隐藏，打印和导出同步）
    m_curveSection = new QFrame();
    m_curveSection->setFrameStyle(QFrame::NoFrame);
    QVBoxLayout *curveLayout = new QVBoxLayout(m_curveSection);
    curveLayout->setContentsMargins(0, 0, 0, 0);
    curveLayout->setSpacing(8);

    QLabel *curveTitleLabel = new QLabel(QString::fromUtf8("测试曲线图"));
    curveTitleLabel->setAlignment(Qt::AlignCenter);
    QFont curveTitleFont = curveTitleLabel->font();
    curveTitleFont.setBold(true);
    curveTitleFont.setPointSize(14);
    curveTitleLabel->setFont(curveTitleFont);
    curveLayout->addWidget(curveTitleLabel);

    m_curveWidget = new CurveWidget();
    curveLayout->addWidget(m_curveWidget);
    m_mainLayout->addWidget(m_curveSection);

    m_remarkSection = new QFrame();
    m_remarkSection->setFrameStyle(QFrame::NoFrame);
    m_remarkSection->setAttribute(Qt::WA_StyledBackground, true);
    m_remarkSection->setStyleSheet(QString::fromUtf8(
        "QFrame { background-color: transparent; border: none; }"));
    QHBoxLayout *remarkLayout = new QHBoxLayout(m_remarkSection);
    remarkLayout->setContentsMargins(15, 4, 15, 4);
    remarkLayout->setSpacing(6);
    remarkLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QLabel *remarkTitle = new QLabel(QString::fromUtf8("备注:"));
    remarkTitle->setStyleSheet(QString::fromUtf8(
        "background: transparent; border: none; padding: 0px;"
        "font-weight: bold; font-size: 15px; color: #111;"));
    remarkTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_remarkLabel = new QLabel(QString::fromUtf8("-"));
    m_remarkLabel->setWordWrap(true);
    m_remarkLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_remarkLabel->setStyleSheet(QString::fromUtf8(
        "background: transparent; border: none; padding: 0px;"
        "font-weight: bold; font-size: 15px; color: #111;"));
    remarkLayout->addWidget(remarkTitle, 0, Qt::AlignVCenter);
    remarkLayout->addWidget(m_remarkLabel, 1, Qt::AlignVCenter);
    m_mainLayout->addWidget(m_remarkSection);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_showCurveCheck = new QCheckBox(QString::fromUtf8("输出时显示测试曲线图"));
    m_showCurveCheck->setChecked(true);
    m_showCurveCheck->setStyleSheet(QString::fromUtf8("font-size: 13px; color: #333;"));
    connect(m_showCurveCheck, &QCheckBox::toggled, this, &PrintTemplate::setShowCurveChart);

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
    buttonLayout->addWidget(m_showCurveCheck);
    buttonLayout->addSpacing(16);
    buttonLayout->addWidget(m_printButton);
    buttonLayout->addWidget(m_exportPDFButton);
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch();
    outer->addLayout(buttonLayout);

    if (QApplication::desktop()) {
        const QRect desk = QApplication::desktop()->availableGeometry(this);
        const int w = qMin(1080, qMax(860, desk.width() - 48));
        const int h = qMax(480, desk.height() - 48);
        resize(w, h);
    }
    setMinimumSize(860, 480);

    clearData();
    applyFormOptions();
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

    if (!m_sqlDatabase->getTestResultTabledata(m_sampleId, m_patientInfo)) {
        CustomMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("无法获取样本信息: ") + m_sampleId);
        return false;
    }

    m_curveData.clear();
    for (const ReagentInfo &info : m_reagents) {
        QVector<double> curve = m_sqlDatabase->getCurveData(m_sampleId, info.id);
        if (!curve.isEmpty()) {
            m_curveData[info.id] = curve;
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
    auto textOrDash = [](const QString &s) {
        return s.trimmed().isEmpty() ? QString::fromUtf8("-") : s;
    };
    m_referDoctorLabel->setText(textOrDash(m_patientInfo.refertithedoctor_));
    m_reviewDoctorLabel->setText(textOrDash(m_patientInfo.reviewdoctors_));
    m_remarkLabel->setText(textOrDash(m_patientInfo.remark_));

    // 清空旧行，根据实际存在的试剂动态创建新行
    clearResultRows();
    for (const ReagentInfo &info : m_reagents) {
        if (m_curveData.contains(info.id)) {
            addResultRow(info);
        }
    }

    // 更新每一行的数据
    for (const ReagentInfo &info : m_reagents) {
        if (m_curveData.contains(info.id)) {
            updateResultRow(info.id);
        }
    }

    applyFormOptions();
}

void PrintTemplate::clearResultRows()
{
    for (const RowWidgets &row : m_resultRows) {
        delete row.testItemLabel;
        delete row.result60SLabel;
        delete row.result180SLabel;
        delete row.result300SLabel;
        delete row.resultMaxLabel;
        delete row.aucLabel;
        delete row.maxSlopeLabel;
        delete row.tmaTimeLabel;
        delete row.lagTimeLabel;
        delete row.adhesionLabel;
        delete row.refRangeLabel;
    }
    m_resultRows.clear();
    m_reagentRowMap.clear();
}

void PrintTemplate::addResultRow(const ReagentInfo &info)
{
    int row = m_resultRows.size() + 3;   // 表头占第0-2行，动态行从第3行开始
    RowWidgets widgets;

    auto createLabel = [&](const QString &text, bool bold = false) -> QLabel* {
        QLabel *label = new QLabel(text);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(QString("padding: 8px; border: none; background-color: #fff; %1")
                             .arg(bold ? "font-weight: bold;" : ""));
        return label;
    };

    widgets.testItemLabel = createLabel(info.displayName);
    widgets.result60SLabel = createLabel("-", true);
    widgets.result180SLabel = createLabel("-", true);
    widgets.result300SLabel = createLabel("-", true);
    widgets.resultMaxLabel = createLabel("-", true);
    widgets.aucLabel = createLabel("-", true);
    widgets.maxSlopeLabel = createLabel("-", true);
    widgets.tmaTimeLabel = createLabel("-", true);
    widgets.lagTimeLabel = createLabel("-", true);
    widgets.adhesionLabel = createLabel("-", true);
    widgets.refRangeLabel = createLabel("-", true);

    m_resultGrid->addWidget(widgets.testItemLabel, row, 0);
    m_resultGrid->addWidget(widgets.result60SLabel, row, 1);
    m_resultGrid->addWidget(widgets.result180SLabel, row, 2);
    m_resultGrid->addWidget(widgets.result300SLabel, row, 3);
    m_resultGrid->addWidget(widgets.resultMaxLabel, row, 4);
    m_resultGrid->addWidget(widgets.aucLabel, row, 5);
    m_resultGrid->addWidget(widgets.maxSlopeLabel, row, 6);
    m_resultGrid->addWidget(widgets.tmaTimeLabel, row, 7);
    m_resultGrid->addWidget(widgets.lagTimeLabel, row, 8);
    m_resultGrid->addWidget(widgets.adhesionLabel, row, 9);
    m_resultGrid->addWidget(widgets.refRangeLabel, row, 10);

    m_resultRows.append(widgets);
    m_reagentRowMap[info.id] = m_resultRows.size() - 1;
}

void PrintTemplate::updateResultRow(quint8 reagentId)
{
    if (!m_reagentRowMap.contains(reagentId)) return;
    int idx = m_reagentRowMap[reagentId];
    RowWidgets &row = m_resultRows[idx];

    // 获取该试剂对应的原始结果字符串
    QString resultStr;
    switch (reagentId) {
        case AA_REAGENT: resultStr = m_patientInfo.AAResult; break;
        case ADP_REAGENT: resultStr = m_patientInfo.ADPResult; break;
        case EPI_REAGENT: resultStr = m_patientInfo.EPIResult; break;
        case COL_REAGENT: resultStr = m_patientInfo.COLResult; break;
        case RIS_REAGENT: resultStr = m_patientInfo.RISResult; break;
        default: return;
    }
    QVector<double> values = splitPercentages(resultStr);
    // 设置60S,180S,300S,Max的原始数值（暂时不添加标记）
    if (values.size() >= 1) row.result60SLabel->setText(QString::number(values[0], 'f', 2));
    if (values.size() >= 2) row.result180SLabel->setText(QString::number(values[1], 'f', 2));
    if (values.size() >= 3) row.result300SLabel->setText(QString::number(values[2], 'f', 2));
    if (values.size() >= 4) row.resultMaxLabel->setText(QString::number(values[3], 'f', 2));

    // 计算分析参数
    const QVector<double>& curve = m_curveData[reagentId];
    if (!curve.isEmpty()) {
        switch (reagentId) {
            case AA_REAGENT: {
                auto res = AggregationAnalyzer::analyzeAA(curve);
                row.aucLabel->setText(QString::number(res.auc, 'f', 2));
                row.maxSlopeLabel->setText(QString::number(res.maxSlope, 'f', 2));
                row.tmaTimeLabel->setText(QString::number(res.tmaTime, 'f', 2));
                row.lagTimeLabel->setText(QString::number(res.lagTime, 'f', 2));
                row.adhesionLabel->setText(QString::number(res.maxValue * 0.8, 'f', 2));
                break;
            }
            case ADP_REAGENT: {
                auto res = AggregationAnalyzer::analyzeADP(curve);
                row.aucLabel->setText(QString::number(res.auc, 'f', 2));
                row.maxSlopeLabel->setText(QString::number(res.aggregationRate, 'f', 2));
                row.tmaTimeLabel->setText(QString::number(res.tmaTime, 'f', 2));
                row.lagTimeLabel->setText(QString::number(res.lagTime, 'f', 2));
                row.adhesionLabel->setText(QString::number(res.maxAggregation * 0.8, 'f', 2));
                break;
            }
            case EPI_REAGENT: {
                auto res = AggregationAnalyzer::analyzeEPI(curve);
                row.aucLabel->setText(QString::number(res.auc, 'f', 2));
                row.maxSlopeLabel->setText(QString::number(res.secondPhaseSlope, 'f', 2));
                row.tmaTimeLabel->setText(QString::number(res.tmaTime, 'f', 2));
                row.lagTimeLabel->setText(QString::number(res.lagTime, 'f', 2));
                row.adhesionLabel->setText("-");
                break;
            }
            case COL_REAGENT: {
                auto res = AggregationAnalyzer::analyzeCOL(curve);
                row.aucLabel->setText(QString::number(res.auc, 'f', 2));
                row.maxSlopeLabel->setText(QString::number(res.maxSlope, 'f', 2));
                row.tmaTimeLabel->setText(QString::number(res.tmaTime, 'f', 2));
                row.lagTimeLabel->setText(QString::number(res.lagTime, 'f', 2));
                row.adhesionLabel->setText("-");
                break;
            }
            case RIS_REAGENT: {
                auto res = AggregationAnalyzer::analyzeRIS(curve);
                row.aucLabel->setText(QString::number(res.auc, 'f', 2));
                row.maxSlopeLabel->setText(QString::number(res.aggregationRate, 'f', 2));
                row.tmaTimeLabel->setText(QString::number(res.tmaTime, 'f', 2));
                row.lagTimeLabel->setText(QString::number(res.lagTime, 'f', 2));
                row.adhesionLabel->setText("-");
                break;
            }
        }
    }

    // 获取参考范围并判断Max值异常
    const ReagentInfo *reagentInfo = nullptr;
    for (const auto &info : m_reagents) {
        if (info.id == reagentId) {
            reagentInfo = &info;
            break;
        }
    }
    if (reagentInfo && values.size() >= 4) {
        double maxVal = values[3];
        QString refRange = getReferenceRangeFromDB(reagentInfo->refDbKey);
        QPair<double, double> range = parseReferenceRange(refRange);
        double low = (range.first == 0 && range.second == 0) ? reagentInfo->defaultLow : range.first;
        double high = (range.first == 0 && range.second == 0) ? reagentInfo->defaultHigh : range.second;

        updateResultWithMark(row.resultMaxLabel, maxVal, low, high);
        row.refRangeLabel->setText(QString::number(low) + "-" + QString::number(high));
    }
}

void PrintTemplate::updateResultWithMark(QLabel *label, double value, double low, double high)
{
    QString originalText = QString::number(value, 'f', 2);
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
    label->setText(originalText + mark);
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
    m_referDoctorLabel->setText(QString::fromUtf8("-"));
    m_reviewDoctorLabel->setText(QString::fromUtf8("-"));
    m_remarkLabel->setText(QString::fromUtf8("-"));

    clearResultRows();
    m_curveWidget->setCurveData(QMap<quint8, QVector<double>>());
}

QVector<double> PrintTemplate::splitPercentages(const QString& input)
{
    QVector<double> result;
#ifdef QT_VERSION_CHECK
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList items = input.split(',', Qt::SkipEmptyParts);
#else
    QStringList items = input.split(',', QString::SkipEmptyParts);
#endif
#else
    QStringList items = input.split(',');
#endif

    for (const QString& item : items) {
        QString trimmed = item.trimmed();
        if (trimmed.endsWith('%')) trimmed.chop(1);
        bool ok;
        double value = trimmed.toDouble(&ok);
        if (ok) result.append(value);
        else qWarning() << "转换失败:" << trimmed;
    }
    return result;
}

void PrintTemplate::printReport()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize(QPageSize::A5));

    QPageSetupDialog pageDialog(&printer, this);
    if (pageDialog.exec() == QDialog::Accepted) {
        QPrintDialog dialog(&printer, this);
        if (dialog.exec() == QDialog::Accepted) {
            QPainter painter(&printer);
            painter.setRenderHint(QPainter::Antialiasing);

            QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);
            QMarginsF margins = printer.pageLayout().margins(QPageLayout::Millimeter);
            QRectF printableRect(
                margins.left() * printer.resolution() / 25.4,
                margins.top() * printer.resolution() / 25.4,
                pageRect.width() - (margins.left() + margins.right()) * printer.resolution() / 25.4,
                pageRect.height() - (margins.top() + margins.bottom()) * printer.resolution() / 25.4
            );

            QWidget printWidget;
            QVBoxLayout printLayout(&printWidget);
            printLayout.setSpacing(5);

            for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
                QLayoutItem *item = m_mainLayout->itemAt(i);
                if (item && item->widget() && item->widget()->isVisible()) {
                    QWidget *originalWidget = item->widget();
                    QPixmap pixmap(originalWidget->size());
                    originalWidget->render(&pixmap);
                    QLabel *label = new QLabel(&printWidget);
                    label->setPixmap(pixmap);
                    label->setAlignment(Qt::AlignTop);
                    printLayout.addWidget(label);
                }
            }

            int contentHeight = 0;
            for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
                QLayoutItem *item = m_mainLayout->itemAt(i);
                if (item && item->widget() && item->widget()->isVisible())
                    contentHeight += item->widget()->height();
            }
            contentHeight += (m_mainLayout->count() - 2) * printLayout.spacing();
            printWidget.setFixedSize(width(), contentHeight);

            double xscale = printableRect.width() / double(printWidget.width());
            double yscale = printableRect.height() / double(printWidget.height());
            double scale = qMin(xscale, yscale) * 0.95;

            painter.translate(printableRect.left() + printableRect.width() / 2,
                              printableRect.top() + printableRect.height() / 2);
            painter.scale(scale, scale);
            painter.translate(-printWidget.width() / 2, -printWidget.height() / 2);

            printWidget.render(&painter);
            painter.end();

            CustomMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("打印完成!"));
        }
    }
}

QString PrintTemplate::getHospitalNameFromDB()
{
    const QString hospitalName = "hospital_name";
    QString titleHospital = m_sqlDatabase->FindPassword(hospitalName);
    return titleHospital + QString::fromUtf8("检验科");
}

QString PrintTemplate::getReferenceRangeFromDB(const QString &reagentKey)
{
    QString manrefVal = "", womanrefVal = "";
    m_sqlDatabase->obtainPersondata(reagentKey, manrefVal, womanrefVal);
    const QString sexindex = m_sexLabel->text();
    if (sexindex == QString::fromUtf8("男"))
        return manrefVal;
    else if (sexindex == QString::fromUtf8("女"))
        return womanrefVal;
    else
        return QString::fromUtf8("未设置性别");
    return QString::fromUtf8("-");
}

QPair<double, double> PrintTemplate::parseReferenceRange(const QString &refRange)
{
    QPair<double, double> result(0, 0);
    QString cleanedRef = refRange;
    int leftBracket = cleanedRef.indexOf("(");
    if (leftBracket != -1) cleanedRef = cleanedRef.left(leftBracket);
    QStringList parts = cleanedRef.split("-");
    if (parts.size() == 2) {
        bool ok1, ok2;
        double low = parts[0].toDouble(&ok1);
        double high = parts[1].toDouble(&ok2);
        if (ok1 && ok2) result = qMakePair(low, high);
    }
    return result;
}

void PrintTemplate::applyFormOptions()
{
    INI_File ini;
    const PrintFormOptions opt = ini.loadPrintForm();
    QString title = opt.title.trimmed();
    if (title.isEmpty()) {
        title = QString::fromUtf8("\xe8\xa1\x80\xe5\xb0\x8f\xe6\x9d\xbf\xe8\x81\x9a\xe9\x9b\x86\xe5\x88\x86\xe6\x9e\x90\xe6\x8a\xa5\xe5\x91\x8a");
    }
    if (m_reportTitleLabel) {
        m_reportTitleLabel->setText(title);
    }
    if (m_patientFrame) {
        m_patientFrame->setVisible(opt.patient);
    }

    auto showCell = [](QLabel *label, bool on) {
        if (!label) {
            return;
        }
        label->setVisible(on);
        if (on) {
            label->setMinimumSize(0, 0);
            label->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        } else {
            label->setFixedSize(0, 0);
        }
    };
    const bool columnOn[] = {
        true,
        opt.sec60,
        opt.sec60,
        opt.sec60,
        opt.maxAgg,
        opt.auc,
        opt.slope,
        opt.tma,
        opt.lag,
        opt.adhesion,
        opt.reference
    };
    const int columnCount = int(sizeof(columnOn) / sizeof(columnOn[0]));
    for (int i = 0; i < m_columnHeaders.size() && i < columnCount; ++i) {
        showCell(m_columnHeaders.at(i), columnOn[i]);
    }
    for (const RowWidgets &row : m_resultRows) {
        showCell(row.testItemLabel, true);
        showCell(row.result60SLabel, opt.sec60);
        showCell(row.result180SLabel, opt.sec60);
        showCell(row.result300SLabel, opt.sec60);
        showCell(row.resultMaxLabel, opt.maxAgg);
        showCell(row.aucLabel, opt.auc);
        showCell(row.maxSlopeLabel, opt.slope);
        showCell(row.tmaTimeLabel, opt.tma);
        showCell(row.lagTimeLabel, opt.lag);
        showCell(row.adhesionLabel, opt.adhesion);
        showCell(row.refRangeLabel, opt.reference);
    }
    m_showAdhesionRate = opt.adhesion;
    if (m_showCurveCheck) {
        m_showCurveCheck->blockSignals(true);
        m_showCurveCheck->setChecked(opt.curve);
        m_showCurveCheck->blockSignals(false);
    }
    setShowCurveChart(opt.curve);
}

void PrintTemplate::setShowAdhesionRate(bool show)
{
    m_showAdhesionRate = show;
    auto showCell = [](QLabel *label, bool on) {
        if (!label) {
            return;
        }
        label->setVisible(on);
        if (on) {
            label->setMinimumSize(0, 0);
            label->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        } else {
            label->setFixedSize(0, 0);
        }
    };
    showCell(m_adhesionHeaderLabel, show);
    for (const RowWidgets &row : m_resultRows) {
        showCell(row.adhesionLabel, show);
    }
}

void PrintTemplate::setShowCurveChart(bool show)
{
    if (m_curveSection) {
        m_curveSection->setVisible(show);
    }
    if (m_mainLayout) {
        m_mainLayout->activate();
    }
    if (m_remarkSection) {
        m_remarkSection->raise();
    }
    updateGeometry();
}

void PrintTemplate::exportToPDF()
{
    QString defaultFileName = QString("血小板聚集功能检测报告_%1.pdf").arg(m_sampleId);
    QString filePath = QFileDialog::getSaveFileName(this,
                                                   QString::fromUtf8("导出PDF报告"),
                                                   defaultFileName,
                                                   QString::fromUtf8("PDF文件 (*.pdf)"));
    if (filePath.isEmpty()) return;
    if (!filePath.endsWith(".pdf", Qt::CaseInsensitive))
        filePath += ".pdf";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize(QPageSize::A5));

    QPainter painter(&printer);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);
    QMarginsF margins = printer.pageLayout().margins(QPageLayout::Millimeter);
    QRectF printableRect(
        margins.left() * printer.resolution() / 25.4,
        margins.top() * printer.resolution() / 25.4,
        pageRect.width() - (margins.left() + margins.right()) * printer.resolution() / 25.4,
        pageRect.height() - (margins.top() + margins.bottom()) * printer.resolution() / 25.4
    );

    QWidget pdfWidget;
    QVBoxLayout pdfLayout(&pdfWidget);
    pdfLayout.setSpacing(5);

    for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
        QLayoutItem *item = m_mainLayout->itemAt(i);
        if (item && item->widget() && item->widget()->isVisible()) {
            QWidget *originalWidget = item->widget();
            QPixmap pixmap(originalWidget->size());
            originalWidget->render(&pixmap);
            QLabel *label = new QLabel(&pdfWidget);
            label->setPixmap(pixmap);
            label->setAlignment(Qt::AlignTop);
            pdfLayout.addWidget(label);
        }
    }

    int contentHeight = 0;
    for (int i = 0; i < m_mainLayout->count() - 1; ++i) {
        QLayoutItem *item = m_mainLayout->itemAt(i);
        if (item && item->widget() && item->widget()->isVisible())
            contentHeight += item->widget()->height();
    }
    contentHeight += (m_mainLayout->count() - 2) * pdfLayout.spacing();
    pdfWidget.setFixedSize(width(), contentHeight);

    double xscale = printableRect.width() / double(pdfWidget.width());
    double yscale = printableRect.height() / double(pdfWidget.height());
    double scale = qMin(xscale, yscale) * 0.95;

    painter.translate(printableRect.left() + printableRect.width() / 2,
                     printableRect.top() + printableRect.height() / 2);
    painter.scale(scale, scale);
    painter.translate(-pdfWidget.width() / 2, -pdfWidget.height() / 2);

    pdfWidget.render(&painter);
    painter.end();

    CustomMessageBox::information(this,
                           QString::fromUtf8("成功"),
                           QString::fromUtf8("PDF导出完成!\n文件保存位置: %1").arg(filePath));
}

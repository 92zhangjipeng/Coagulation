#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "inquire_sql_info.h"
#include "ui_inquire_sql_info.h"
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QPushButton>
#include <QScrollBar>
#include <QMessageBox>
#include <QTimer>
#include "testing.h"
#include "quiutils.h"
#include "loginui.h"
#include "globaldata.h"
#include <operclass/fullyautomatedplatelets.h>
#include <algorithm>
#include <limits>
#include <numeric>
#include <custom_style/custommessagebox.h>
#include "PrintReport/PrintTemplate.h"

Inquire_Sql_Info::Inquire_Sql_Info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Inquire_Sql_Info)
{
    ui->setupUi(this);
    initTextTip();
    ui->progressBarLoad->hide();

    // 美化"聚集率曲线" GroupBox：浅蓝渐变背景 + 圆角边框
    ui->widget_curve->setStyleSheet(
        "QGroupBox#widget_curve { "
        "  border: 1px solid #D4EAF5; "
        "  border-radius: 8px; "
        "  margin-top: 14px; "
        "  background: transparent; "
        "} "
        "QGroupBox#widget_curve::title { "
        "  subcontrol-origin: margin; "
        "  left: 12px; "
        "  padding: 0 4px; "
        "  color: #2C5F8A; "
        "  font-weight: bold; "
        "}"
    );

    // === 用代码动态创建查询条件区所有控件 ===
    // --- 第一行控件
    checkBox_today = new QCheckBox("今天", ui->widget_date_);
    checkBox_this_month = new QCheckBox("本月", ui->widget_date_);
    checkBox_all = new QCheckBox("全部", ui->widget_date_);
    checkBox_select_time = new QCheckBox("自定义", ui->widget_date_);
    label_fromdate = new QLabel("起：", ui->widget_date_);
    dateEdit_begin = new QDateEdit(ui->widget_date_);
    dateEdit_begin->setCalendarPopup(true);
    dateEdit_begin->setDisplayFormat("yyyy-MM-dd");
    label = new QLabel("止：", ui->widget_date_);
    dateEdit_end = new QDateEdit(ui->widget_date_);
    dateEdit_end->setCalendarPopup(true);
    dateEdit_end->setDisplayFormat("yyyy-MM-dd");
    toolButton_OK = new QToolButton(ui->widget_date_);
    toolButton_OK->setText("查询");
    toolButton_stats = new QToolButton(ui->widget_date_);
    toolButton_stats->setText("试剂统计");
    toolButton_outPrint = new QToolButton(ui->widget_date_);
    toolButton_outPrint->setText("打印报告");

    // --- 第二行控件
    label_Sample = new QLabel("样本号：", ui->widget_date_);
    spinBox_label_Sample = new QSpinBox(ui->widget_date_);
    spinBox_label_Sample->setButtonSymbols(QAbstractSpinBox::NoButtons);
    spinBox_label_Sample->setRange(1, 999999);
    label_Name = new QLabel("姓名：", ui->widget_date_);
    lineEdit_Name = new QLineEdit(ui->widget_date_);
    lineEdit_Name->setPlaceholderText("请输入患者姓名");
    label_age = new QLabel("年龄：", ui->widget_date_);
    comboBox_age = new QComboBox(ui->widget_date_);
    label_Section = new QLabel("科别：", ui->widget_date_);
    comboBox_Section = new QComboBox(ui->widget_date_);
    label_sendingdoctor = new QLabel("送检医生：", ui->widget_date_);
    comboBox_sendingdoctor = new QComboBox(ui->widget_date_);
    label_bednum = new QLabel("床号：", ui->widget_date_);
    lineEdit_bednum = new QLineEdit(ui->widget_date_);
    lineEdit_bednum->setPlaceholderText("请输入床号");
    toolButtonFindexatc = new QToolButton(ui->widget_date_);
    toolButtonFindexatc->setText("精确查找");

    QFont fieldFont(QStringLiteral("Microsoft YaHei"));
    fieldFont.setPixelSize(12);
    const int fieldH = 28;
    const QFontMetrics fieldMetrics(fieldFont);
    auto fitLabel = [&](QLabel *lab) {
        lab->setFont(fieldFont);
        lab->setFixedHeight(fieldH);
        const int textW = fieldMetrics.boundingRect(lab->text()).width();
        lab->setFixedWidth(textW + 28);
        lab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        lab->setStyleSheet(QStringLiteral(
            "QLabel { font-weight: normal; font-size: 12px; background: transparent;"
            " color: #1A3344; padding: 0px 6px 0px 8px; }"));
    };
    auto fitCheck = [&](QCheckBox *box) {
        box->setFont(fieldFont);
        box->setFixedHeight(fieldH);
        box->setFixedWidth(fieldMetrics.width(box->text()) + 40);
        box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    };
    auto fitButton = [&](QToolButton *btn) {
        btn->setFont(fieldFont);
        btn->setFixedHeight(fieldH);
        btn->setMinimumWidth(fieldMetrics.width(btn->text()) + 18);
        btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    };
    auto fitField = [&](QWidget *editor, int width) {
        editor->setFont(fieldFont);
        editor->setFixedSize(width, fieldH);
        editor->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    };
    fitCheck(checkBox_today);
    fitCheck(checkBox_this_month);
    fitCheck(checkBox_all);
    fitCheck(checkBox_select_time);
    fitLabel(label_fromdate);
    fitLabel(label);
    fitLabel(label_Sample);
    fitLabel(label_Name);
    fitLabel(label_age);
    fitLabel(label_Section);
    fitLabel(label_sendingdoctor);
    fitLabel(label_bednum);
    fitField(dateEdit_begin, 118);
    fitField(dateEdit_end, 118);
    fitField(spinBox_label_Sample, 64);
    fitField(lineEdit_Name, 108);
    fitField(lineEdit_bednum, 72);
    fitField(comboBox_age, 76);
    fitField(comboBox_Section, 88);
    fitField(comboBox_sendingdoctor, 88);
    fitButton(toolButton_OK);
    fitButton(toolButton_stats);
    fitButton(toolButton_outPrint);
    fitButton(toolButtonFindexatc);

    QHBoxLayout *mainLayout = new QHBoxLayout(ui->widget_date_);
    mainLayout->setContentsMargins(10, 12, 10, 6);
    mainLayout->setSpacing(8);
    ui->widget_date_->setMinimumHeight(0);
    ui->widget_date_->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->widget_date_->setTitle(QString::fromUtf8("查询条件"));
    ui->widget_curve->setTitle(QString::fromUtf8("试剂统计"));
    ui->widget_curve->setMinimumHeight(180);
    ui->widget_curve->hide();

    mainLayout->addWidget(checkBox_today, 0, Qt::AlignVCenter);
    mainLayout->addWidget(checkBox_this_month, 0, Qt::AlignVCenter);
    mainLayout->addWidget(checkBox_all, 0, Qt::AlignVCenter);
    mainLayout->addWidget(checkBox_select_time, 0, Qt::AlignVCenter);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(label_fromdate, 0, Qt::AlignVCenter);
    mainLayout->addWidget(dateEdit_begin, 0, Qt::AlignVCenter);
    mainLayout->addWidget(label, 0, Qt::AlignVCenter);
    mainLayout->addWidget(dateEdit_end, 0, Qt::AlignVCenter);
    mainLayout->addSpacing(4);
    mainLayout->addWidget(toolButton_OK, 0, Qt::AlignVCenter);
    mainLayout->addWidget(toolButton_stats, 0, Qt::AlignVCenter);
    mainLayout->addWidget(toolButton_outPrint, 0, Qt::AlignVCenter);
    mainLayout->addSpacing(8);
    mainLayout->addWidget(toolButtonFindexatc, 0, Qt::AlignVCenter);
    mainLayout->addStretch(1);

    // === 其他初始化 ===
    m_group = std::make_unique<QButtonGroup>(this);
    m_group->addButton(checkBox_all,INQUIRE_ALL_SQL);
    m_group->addButton(checkBox_today,INQUIRE_TODAY_SQL);
    m_group->addButton(checkBox_this_month,INQUIRE_MONTH_SQL);
    m_group->addButton(checkBox_select_time,INQUIRE_SPECIFIC_SQL);
    m_group->setExclusive(true);

    initDateEditUI(dateEdit_begin, ui->widget_date_);
    initDateEditUI(dateEdit_end, ui->widget_date_);

    const QString compactDate = QStringLiteral(
        "QDateEdit { font-family: 'Microsoft YaHei'; font-size: 12px;"
        " background: #FFFFFF; border: 1px solid #C5D7E4; border-radius: 3px;"
        " padding: 0px 0px 0px 4px; }"
        "QDateEdit::drop-down { subcontrol-origin: padding; subcontrol-position: center right;"
        " width: 16px; border: none; }"
        "QDateEdit QLineEdit { font-family: 'Microsoft YaHei'; font-size: 12px;"
        " border: none; background: transparent; padding: 0px; margin: 0px; }");
    const QList<QDateEdit *> dateEditors = { dateEdit_begin, dateEdit_end };
    for (QDateEdit *dateEdit : dateEditors) {
        dateEdit->setFont(fieldFont);
        dateEdit->setFixedSize(118, fieldH);
        dateEdit->setStyleSheet(compactDate);
        const QList<QLineEdit *> inners = dateEdit->findChildren<QLineEdit *>();
        for (QLineEdit *inner : inners) {
            inner->setFont(fieldFont);
            inner->setStyleSheet(QStringLiteral(
                "QLineEdit { font-size: 12px; border: none; background: transparent;"
                " padding: 0px; margin: 0px; }"));
        }
    }

    checkBox_today->setChecked(true);
    const auto syncCustomDates = [this]() {
        const bool custom = checkBox_select_time && checkBox_select_time->isChecked();
        if (label_fromdate) label_fromdate->setVisible(custom);
        if (dateEdit_begin) dateEdit_begin->setVisible(custom);
        if (label) label->setVisible(custom);
        if (dateEdit_end) dateEdit_end->setVisible(custom);
    };
    connect(checkBox_select_time, &QCheckBox::toggled, this, [syncCustomDates](bool) {
        syncCustomDates();
    });
    syncCustomDates();

    setupRealtimeDataDemo(ui->Inquire_curve_1);
    ui->Inquire_curve_1->yAxis->setRange(-20, 100);
    ui->Inquire_curve_1->xAxis->setRange(0, 300);
    ui->Inquire_curve_1->replot();

    initTableWidgetStyle();
    ui->tableWidget_SQL_Inquire->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget_SQL_Inquire, &QTableWidget::customContextMenuRequested,
            this, &Inquire_Sql_Info::onTableContextMenu);

    QStringList Agenum;
    Agenum.reserve(120);
    for(int i = 0; i < 120; i++)
        Agenum.push_back(QString::number(i + 1));
    comboBox_age->addItems(Agenum);
    comboBox_age->setCurrentIndex(-1);
    comboBox_age->setMaxVisibleItems(12);
    const QString comboPopup = QStringLiteral(
        "QAbstractItemView { background: #FFFFFF; color: #16384A; border: none; outline: 0; }"
        "QAbstractItemView::item { min-height: 24px; padding: 2px 8px; }"
        "QAbstractItemView::item:selected { background: #D4EAF5; color: #16384A; }"
        "QScrollBar:vertical { width: 8px; margin: 0px; background: #EEF3F7; border: none; }"
        "QScrollBar::handle:vertical { background: #8AA4BE; min-height: 18px; border-radius: 3px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; width: 0px; border: none; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }");
    const QList<QComboBox *> filterCombos = {
        comboBox_age, comboBox_Section, comboBox_sendingdoctor
    };
    for (QComboBox *combo : filterCombos) {
        combo->setMaxVisibleItems(12);
        if (combo->view()) {
            combo->view()->setMinimumWidth(qMax(combo->width(), 96));
            combo->view()->setStyleSheet(comboPopup);
            if (QScrollBar *bar = combo->view()->verticalScrollBar()) {
                bar->setStyleSheet(QStringLiteral(
                    "QScrollBar:vertical { width: 8px; margin: 0px; background: #EEF3F7; border: none; }"
                    "QScrollBar::handle:vertical { background: #8AA4BE; min-height: 18px; border-radius: 3px; }"
                    "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; width: 0px; }"
                    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"));
            }
        }
    }

    m_exactDialog = new QDialog(this);
    m_exactDialog->setObjectName(QStringLiteral("exactFindDialog"));
    m_exactDialog->setWindowTitle(QString::fromUtf8("精确查找"));
    m_exactDialog->setModal(true);
    m_exactDialog->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    m_exactDialog->setStyleSheet(QStringLiteral(
        "QDialog#exactFindDialog { background: #F4FAFE; }"
        "QLabel { color: #1A3344; font-family: 'Microsoft YaHei'; font-size: 13px; background: transparent; }"
        "QLineEdit, QComboBox, QSpinBox {"
        " font-family: 'Microsoft YaHei'; font-size: 13px; color: #16384A;"
        " background: #FFFFFF; border: 1px solid #C5D7E4; border-radius: 4px; padding: 0 8px; min-height: 28px; }"
        "QComboBox::drop-down { width: 18px; border: none; }"
        "QPushButton {"
        " font-family: 'Microsoft YaHei'; font-size: 13px; color: white;"
        " background: #1A6F99; border: none; border-radius: 4px; min-width: 72px; min-height: 28px; }"
        "QPushButton:hover { background: #14597C; }"
        "QPushButton#exactClose {"
        " color: #1A6F99; background: #FFFFFF; border: 1px solid #8FBFD6; }"
        "QPushButton#exactClose:hover { background: #E7F3FA; }"));
    const QList<QWidget *> exactEditors = {
        spinBox_label_Sample, lineEdit_Name, lineEdit_bednum,
        comboBox_age, comboBox_Section, comboBox_sendingdoctor
    };
    for (QWidget *editor : exactEditors) {
        editor->setParent(m_exactDialog);
        editor->setFixedSize(168, 30);
        editor->setFont(fieldFont);
    }
    const QList<QLabel *> exactLabels = {
        label_Sample, label_Name, label_age, label_Section, label_sendingdoctor, label_bednum
    };
    for (QLabel *lab : exactLabels) {
        lab->setParent(m_exactDialog);
        lab->setFixedSize(88, 30);
        lab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lab->setFont(fieldFont);
    }
    comboBox_Section->setEditable(true);
    comboBox_sendingdoctor->setEditable(true);
    if (comboBox_Section->lineEdit())
        comboBox_Section->lineEdit()->setPlaceholderText(QString::fromUtf8("可输入科别"));
    if (comboBox_sendingdoctor->lineEdit())
        comboBox_sendingdoctor->lineEdit()->setPlaceholderText(QString::fromUtf8("可输入医生"));
    QGridLayout *exactLayout = new QGridLayout(m_exactDialog);
    exactLayout->setContentsMargins(22, 18, 22, 16);
    exactLayout->setHorizontalSpacing(8);
    exactLayout->setVerticalSpacing(12);
    exactLayout->addWidget(label_Sample, 0, 0);
    exactLayout->addWidget(spinBox_label_Sample, 0, 1);
    exactLayout->addWidget(label_Name, 0, 2);
    exactLayout->addWidget(lineEdit_Name, 0, 3);
    exactLayout->addWidget(label_age, 1, 0);
    exactLayout->addWidget(comboBox_age, 1, 1);
    exactLayout->addWidget(label_Section, 1, 2);
    exactLayout->addWidget(comboBox_Section, 1, 3);
    exactLayout->addWidget(label_sendingdoctor, 2, 0);
    exactLayout->addWidget(comboBox_sendingdoctor, 2, 1);
    exactLayout->addWidget(label_bednum, 2, 2);
    exactLayout->addWidget(lineEdit_bednum, 2, 3);
    QPushButton *exactFindBtn = new QPushButton(QString::fromUtf8("查找"), m_exactDialog);
    QPushButton *exactCloseBtn = new QPushButton(QString::fromUtf8("关闭"), m_exactDialog);
    exactCloseBtn->setObjectName(QStringLiteral("exactClose"));
    QHBoxLayout *exactButtons = new QHBoxLayout;
    exactButtons->addStretch(1);
    exactButtons->addWidget(exactFindBtn);
    exactButtons->addSpacing(8);
    exactButtons->addWidget(exactCloseBtn);
    exactLayout->addLayout(exactButtons, 3, 0, 1, 4);
    connect(exactFindBtn, &QPushButton::clicked, this, [this]() {
        on_toolButtonFindexatc_clicked();
        if (m_exactDialog)
            m_exactDialog->accept();
    });
    connect(exactCloseBtn, &QPushButton::clicked, m_exactDialog, &QDialog::reject);

    // === 手动连接按钮信号槽 ===
    connect(toolButton_OK, &QToolButton::clicked, this, &Inquire_Sql_Info::on_toolButton_OK_clicked);
    connect(toolButton_stats, &QToolButton::clicked, this, &Inquire_Sql_Info::on_toolButton_stats_clicked);
    connect(toolButton_outPrint, &QToolButton::clicked, this, &Inquire_Sql_Info::on_toolButton_outPrint_clicked);
    connect(toolButtonFindexatc, &QToolButton::clicked, this, [this]() {
        if (m_exactDialog)
            m_exactDialog->exec();
    });

    ui->Inquire_curve_1->setMinimumSize(0, 0);
    ui->Inquire_curve_1->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->widget_sats_curve->setMinimumWidth(280);
    ui->widget_sats_curve->setMaximumWidth(QWIDGETSIZE_MAX);
    if (ui->horizontalLayout_3) {
        ui->horizontalLayout_3->removeWidget(ui->Inquire_curve_1);
        const int statsIndex = ui->horizontalLayout_3->indexOf(ui->widget_sats_curve);
        if (statsIndex >= 0)
            ui->horizontalLayout_3->setStretch(statsIndex, 1);
    }
    if (ui->mainLayout) {
        ui->mainLayout->setStretch(1, 1);
        ui->mainLayout->setStretch(3, 0);
    }

    QTableWidget *inquireTable = ui->tableWidget_SQL_Inquire;
    m_sampleFrame = new QFrame(inquireTable->viewport());
    m_sampleFrame->setObjectName(QStringLiteral("sampleRowFrame"));
    m_sampleFrame->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_sampleFrame->setStyleSheet(QStringLiteral(
        "QFrame#sampleRowFrame { background: transparent;"
        " border: 2px solid #1487B8; border-radius: 4px; }"));
    m_sampleFrame->hide();

    m_curveFollow = new QFrame(this);
    m_curveFollow->setObjectName(QStringLiteral("curveFollow"));
    m_curveFollow->setAutoFillBackground(true);
    m_curveFollow->setStyleSheet(QStringLiteral(
        "QFrame#curveFollow { background: #FFFFFF; border: 1px solid #7EB6D4; border-radius: 8px; }"
        "QLabel#curveFollowTitle {"
        " background: #E7F3FA; color: #14597C;"
        " font-family: 'Microsoft YaHei'; font-size: 13px; font-weight: 700;"
        " border: none; border-bottom: 1px solid #D4EAF5;"
        " padding: 0px 12px; }"));
    QVBoxLayout *followLayout = new QVBoxLayout(m_curveFollow);
    followLayout->setContentsMargins(0, 0, 0, 8);
    followLayout->setSpacing(0);
    m_curveFollowTitle = new QLabel(m_curveFollow);
    m_curveFollowTitle->setObjectName(QStringLiteral("curveFollowTitle"));
    m_curveFollowTitle->setFixedHeight(34);
    m_curveFollowTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_curveFollowTitle->setStyleSheet(QStringLiteral(
        "QLabel { background: #E7F3FA; color: #14597C;"
        " font-family: 'Microsoft YaHei'; font-size: 13px; font-weight: 700;"
        " border: none; border-bottom: 1px solid #D4EAF5; padding: 0px 12px; }"));
    m_curveFollowTitle->setText(QString::fromUtf8("聚集率曲线"));
    ui->Inquire_curve_1->setParent(m_curveFollow);
    QHBoxLayout *plotRow = new QHBoxLayout;
    plotRow->setContentsMargins(10, 8, 10, 0);
    plotRow->addWidget(ui->Inquire_curve_1, 1);
    followLayout->addWidget(m_curveFollowTitle);
    followLayout->addLayout(plotRow, 1);
    m_curveFollow->hide();
    setStyleSheet(QString::fromUtf8(
        "Inquire_Sql_Info, QWidget#Inquire_Sql_Info { background: #E8F4FD; }"
        "QGroupBox { background: transparent; border: 1px solid #D4EAF5; border-radius: 8px;"
        " margin-top: 14px; font-weight: 700; color: #2C5F8A; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; color: #2C5F8A; }"
        "QLabel { background: transparent; color: #1A3344; font-weight: normal; font-size: 12px; }"
        "QLineEdit, QComboBox, QSpinBox {"
        " font-family: 'Microsoft YaHei'; font-size: 12px;"
        " background: #FFFFFF; border: 1px solid #C5D7E4; border-radius: 3px;"
        " padding: 0px 4px; color: #16384A; }"
        "QComboBox::drop-down { width: 18px; border: none; }"
        "QComboBox QAbstractItemView {"
        " background: #FFFFFF; color: #16384A; border: 1px solid #AACDE2;"
        " outline: 0; font-size: 12px; }"
        "QComboBox QAbstractItemView::item {"
        " min-height: 24px; padding: 2px 8px; color: #16384A; }"
        "QComboBox QAbstractItemView::item:selected {"
        " background: #D4EAF5; color: #16384A; }"
        "QComboBox QScrollBar:vertical { width: 8px; margin: 0px; background: #EEF3F7; border: none; }"
        "QComboBox QScrollBar::handle:vertical { background: #8AA4BE; min-height: 18px; border-radius: 3px; }"
        "QComboBox QScrollBar::add-line:vertical, QComboBox QScrollBar::sub-line:vertical {"
        " height: 0px; width: 0px; border: none; background: transparent; }"
        "QComboBox QScrollBar::add-page:vertical, QComboBox QScrollBar::sub-page:vertical { background: transparent; }"
        "QToolButton { font-family: 'Microsoft YaHei'; font-size: 12px;"
        " background: #1A6F99; color: white; border: none; border-radius: 3px;"
        " padding: 0px 12px; min-height: 28px; max-height: 28px; }"
        "QToolButton:hover { background: #14597C; }"
        "QCheckBox { background: transparent; color: #1A4D6B; spacing: 6px;"
        " min-height: 28px; max-height: 28px; }"
        "QHeaderView::section { background: #D4EAF5; color: #2C5F8A; border: none;"
        " border-right: 1px solid #C5D7E4; padding: 6px; font-weight: 700; }"
        "QTableWidget { background: white; alternate-background-color: #F4FAFE;"
        " gridline-color: #E3EEF5; border: 1px solid #D4EAF5; }"
        "QTableWidget::item:selected { background: #B8DFF7; color: #16384A; }"
        "QTableWidget QScrollBar:vertical { width: 8px; margin: 0px; background: #E7EEF4; border: none; }"
        "QTableWidget QScrollBar::handle:vertical { background: #7EA4BE; min-height: 28px;"
        " border-radius: 4px; margin: 2px; }"
        "QTableWidget QScrollBar::handle:vertical:hover { background: #1A6F99; }"
        "QTableWidget QScrollBar::add-line:vertical, QTableWidget QScrollBar::sub-line:vertical {"
        " height: 0px; width: 0px; border: none; background: transparent; }"
        "QTableWidget QScrollBar::add-page:vertical, QTableWidget QScrollBar::sub-page:vertical {"
        " background: transparent; }"
        "QTableWidget QScrollBar:horizontal { height: 8px; margin: 0px; background: #E7EEF4; border: none; }"
        "QTableWidget QScrollBar::handle:horizontal { background: #7EA4BE; min-width: 28px;"
        " border-radius: 4px; margin: 2px; }"
        "QTableWidget QScrollBar::handle:horizontal:hover { background: #1A6F99; }"
        "QTableWidget QScrollBar::add-line:horizontal, QTableWidget QScrollBar::sub-line:horizontal {"
        " height: 0px; width: 0px; border: none; background: transparent; }"
        "QTableWidget QScrollBar::add-page:horizontal, QTableWidget QScrollBar::sub-page:horizontal {"
        " background: transparent; }"
        "QProgressBar { border: 1px solid #C5D7E4; border-radius: 4px; background: white; text-align: center; }"
        "QProgressBar::chunk { background: #5FA8D3; }"));
    const QString compactCheck = QStringLiteral(
        "QCheckBox { background: transparent; color: #1A4D6B; spacing: 6px;"
        " min-height: 28px; max-height: 28px; }"
        "QCheckBox::indicator { width: 16px; height: 16px; border-radius: 3px; }"
        "QCheckBox::indicator:unchecked { background: #FFFFFF; border: 1px solid #AACDE2; }"
        "QCheckBox::indicator:checked { background: #5FA8D3; border: 1px solid #5FA8D3; }");
    const QList<QCheckBox *> inquireBoxes = {
        checkBox_today, checkBox_this_month, checkBox_all, checkBox_select_time
    };
    for (QCheckBox *box : inquireBoxes) {
        if (box) {
            box->setStyleSheet(compactCheck);
        }
    }
    const QString tableBar = QStringLiteral(
        "QScrollBar:vertical { width: 8px; margin: 0px; background: #E7EEF4; border: none; }"
        "QScrollBar::handle:vertical { background: #7EA4BE; min-height: 28px; border-radius: 4px; margin: 2px; }"
        "QScrollBar::handle:vertical:hover { background: #1A6F99; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; width: 0px; border: none; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"
        "QScrollBar:horizontal { height: 8px; margin: 0px; background: #E7EEF4; border: none; }"
        "QScrollBar::handle:horizontal { background: #7EA4BE; min-width: 28px; border-radius: 4px; margin: 2px; }"
        "QScrollBar::handle:horizontal:hover { background: #1A6F99; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { height: 0px; width: 0px; border: none; }"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: transparent; }");
    if (ui->tableWidget_SQL_Inquire->verticalScrollBar())
        ui->tableWidget_SQL_Inquire->verticalScrollBar()->setStyleSheet(tableBar);
    if (ui->tableWidget_SQL_Inquire->horizontalScrollBar())
        ui->tableWidget_SQL_Inquire->horizontalScrollBar()->setStyleSheet(tableBar);
    toolButtonFindexatc->setStyleSheet(QStringLiteral(
        "QToolButton { font-family: 'Microsoft YaHei'; font-size: 12px; color: #1A6F99;"
        " background: #FFFFFF; border: 1px solid #1A6F99; border-radius: 3px;"
        " padding: 0px 12px; min-height: 28px; max-height: 28px; }"
        "QToolButton:hover { background: #E7F3FA; }"));
    qApp->installEventFilter(this);

    LoginEngineerMode(false);
    initInputValidators();
}

void Inquire_Sql_Info::LoginEngineerMode(const bool Enginnermode)
{
    if (!Enginnermode) {
        toolButton_stats->hide();
        ui->widget_sats_curve->hide();
        ui->widget_curve->hide();
    } else {
        toolButton_stats->show();
        ui->widget_sats_curve->show();
        ui->widget_curve->show();
        stats_today_reagent();
    }
    update();
}


void Inquire_Sql_Info::init_sats_curve(QVector<double> useed_reag)
{
    ui->widget_sats_curve->clearPlottables();
    ui->widget_sats_curve->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // 设置背景色：浅蓝绿色渐变，让曲线区更有层次感
    QLinearGradient bgGrad(0, 0, 0, 1);
    bgGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
    bgGrad.setColorAt(0.0, QColor(248, 252, 255));
    bgGrad.setColorAt(1.0, QColor(232, 244, 248));
    ui->widget_sats_curve->setBackground(QBrush(bgGrad));

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
    QVector<double> values(5, 0.0);
    for (int i = 0; i < useed_reag.size() && i < values.size(); ++i) {
        values[i] = useed_reag.at(i);
    }
    double ymax = 10.0;
    for (double value : values) {
        ymax = qMax(ymax, value);
    }
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    keyAxis->setTicker(textTicker);
    fossil->setWidth(fossil->width()/5);
    keyAxis->setTickLabelRotation(0);
    keyAxis->setSubTicks(false);
    keyAxis->setTickLength(0, 4);
    keyAxis->setRange(0, 6);
    keyAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    valueAxis->setRange(0, ymax * 1.25);
    valueAxis->setPadding(25);
    valueAxis->setLabel(QString::fromUtf8("试剂使用次数"));
    valueAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    fossil->setData(ticks, values);
    ui->widget_sats_curve->repaint();
    ui->widget_sats_curve->replot(QCustomPlot::rpQueuedReplot);
}


void Clear_table(QTableWidget* table)
{
    if (!table) return;
    table->clearContents();   // 清空内容（不删除行结构）
    table->setRowCount(0);    // 重置行数为0
}

void Inquire_Sql_Info::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_followSampleRow >= 0)
        placeCurveFollow(m_followSampleRow);
}

void Inquire_Sql_Info::initTableWidgetStyle()
{
    QStringList header{tr("样本号"), tr("添加时间"), tr("条形码"), tr("姓名"), tr("性别"), tr("年龄"),
                       tr("科别"), tr("床号"), tr("送检医生"), tr("AA聚集率"), tr("ADP聚集率"),
                       tr("EPI聚集率"), tr("COL聚集率"), tr("RIS聚集率")};
    int allcol = header.size();
    QTableWidget *table = ui->tableWidget_SQL_Inquire;

    // ----- 1. 基础设置 -----
    table->setColumnCount(allcol);
    table->setHorizontalHeaderLabels(header);
    table->clearContents();
    table->setRowCount(0);
    table->setFocusPolicy(Qt::NoFocus);
    table->setFrameShape(QFrame::Box);
    table->setShowGrid(true);
    table->setGridStyle(Qt::SolidLine);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::ContiguousSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(32);
    table->horizontalHeader()->setMinimumHeight(40);

    // ----- 2. 设置列宽策略：Stretch模式，自动均匀填充整个表格宽度，避免水平溢出 -----
    for (int i = 0; i < allcol; ++i) {
        table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    table->horizontalHeader()->setStretchLastSection(true);

    // 表头字体
    QFont headerFont("楷体", 12, QFont::Bold);
    headerFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    table->horizontalHeader()->setFont(headerFont);
    table->horizontalHeader()->setHighlightSections(false);

    connect(table, &QTableWidget::itemClicked, this, &Inquire_Sql_Info::onSelectRowItem);
    connect(table->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int) {
        if (m_followSampleRow >= 0)
            placeCurveFollow(m_followSampleRow);
    });
    connect(table->horizontalScrollBar(), &QScrollBar::valueChanged, this, [this](int) {
        if (m_followSampleRow >= 0)
            placeCurveFollow(m_followSampleRow);
    });
}

Inquire_Sql_Info::~Inquire_Sql_Info()
{
    if (qApp)
        qApp->removeEventFilter(this);
    cancelInquireFill();
    if (m_threadInqure.isRunning()) {
        m_threadInqure.quit();
        m_threadInqure.wait(3000);
    }

    cleanupMemory();

    // 清理曲线标签对象 - 使用QCustomPlot的removeItem方法
    for (QCPItemText* label : m_curveLabels) {
        if (label && ui->Inquire_curve_1) {
            ui->Inquire_curve_1->removeItem(label);
        }
    }
    m_curveLabels.clear();

    delete ui;
}

void Inquire_Sql_Info::closeEvent(QCloseEvent *event)
{
    event->accept();
}


void Inquire_Sql_Info::myMoveEvent(QMouseEvent *event)
{
    QCustomPlot* plot = ui->Inquire_curve_1;
    if (!plot) return;

    const QPoint pos = event->pos();
    const QRect axisRect = plot->xAxis->axisRect()->rect();

    // 若鼠标不在绘图区域内，隐藏竖直线、圆点及提示
    if (!axisRect.contains(pos)) {
        if (m_vLineCursor) m_vLineCursor->setVisible(false);
        for (QCPItemEllipse* dot : m_curveDotList) {
            if (dot) dot->setVisible(false);
        }
        if (m_cursorValueTip) m_cursorValueTip->setVisible(false);
        plot->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    // 像素坐标 -> 数据坐标
    const double xCoord = plot->xAxis->pixelToCoord(pos.x());

    // 准备曲线列表和对应颜色
    QList<QPair<QCPGraph*, QColor> > graphList;
    if (m_showAACpgraph) graphList.append(qMakePair(m_showAACpgraph, GlobalData::customCurveColor(AA_REAGENT)));
    if (m_showADPCpgraph) graphList.append(qMakePair(m_showADPCpgraph, GlobalData::customCurveColor(ADP_REAGENT)));
    if (m_showEPICpgraph) graphList.append(qMakePair(m_showEPICpgraph, GlobalData::customCurveColor(EPI_REAGENT)));
    if (m_showCOLCpgraph) graphList.append(qMakePair(m_showCOLCpgraph, GlobalData::customCurveColor(COL_REAGENT)));
    if (m_showRISCpgraph) graphList.append(qMakePair(m_showRISCpgraph, GlobalData::customCurveColor(RIS_REAGENT)));

    // 确保圆点数量与曲线数量一致
    while (m_curveDotList.size() < graphList.size()) {
        QCPItemEllipse* dot = new QCPItemEllipse(plot);
        dot->setPen(Qt::NoPen);
        dot->setBrush(QBrush(Qt::white));
        dot->setAntialiased(true);
        dot->setClipToAxisRect(true);
        dot->setVisible(false);
        m_curveDotList.append(dot);
    }

    // 更新竖直线（在数据坐标系下：x 固定为鼠标位置的 xCoord，y 从轴的下边界到上边界）
    if (m_vLineCursor) {
        m_vLineCursor->start->setType(QCPItemPosition::ptPlotCoords);
        m_vLineCursor->end->setType(QCPItemPosition::ptPlotCoords);
        m_vLineCursor->start->setCoords(xCoord, plot->yAxis->range().lower);
        m_vLineCursor->end->setCoords(xCoord, plot->yAxis->range().upper);
        m_vLineCursor->setVisible(true);
    }

    // 在每条曲线上找到 xCoord 最近的点，绘制圆点
    QString tipText = QString("X: %1 S").arg(xCoord, 0, 'f', 1);
    for (int i = 0; i < graphList.size(); ++i) {
        if (i < 0 || i >= m_curveDotList.size()) {
            break;
        }
        QCPGraph* graph = graphList.at(i).first;
        const QColor& color = graphList.at(i).second;
        QCPItemEllipse* dot = m_curveDotList.at(i);

        if (!graph || !dot || !graph->data() || graph->data()->size() == 0) {
            if (dot) dot->setVisible(false);
            continue;
        }

        // 线性查找：找到 xCoord 最近的数据点（key 按时间递增，可直接遍历或估算索引）
        const int dataSize = graph->data()->size();
        double bestKey = 0.0;
        double bestValue = 0.0;
        double minDist = 1e300;
        bool found = false;

        // 用估算的索引作为起点，向前后扫描有限的邻居点
        // x 轴范围为 0~300，key 为连续递增，近似索引 = xCoord
        int estIdx = static_cast<int>(xCoord);
        if (estIdx < 0) estIdx = 0;
        if (estIdx >= dataSize) estIdx = dataSize - 1;

        const int scanRadius = qMin(8, dataSize);
        int startIdx = qMax(0, estIdx - scanRadius);
        int endIdx = qMin(dataSize, estIdx + scanRadius + 1);
        for (int idx = startIdx; idx < endIdx; ++idx) {
            const QCPGraphData* pData = graph->data()->at(idx);
            if (!pData) continue;
            double dist = qAbs(pData->key - xCoord);
            if (dist < minDist) {
                minDist = dist;
                bestKey = pData->key;
                bestValue = pData->value;
                found = true;
            }
        }

        if (found) {
            // topLeft/bottomRight 都用 ptPlotCoords + 相对位置，画出一个小圆点
            dot->topLeft->setType(QCPItemPosition::ptPlotCoords);
            dot->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            const double xRadius = 3.0;   // X 方向半径（数据单位）
            const double yRange = plot->yAxis->range().upper - plot->yAxis->range().lower;
            const double xRange = plot->xAxis->range().upper - plot->xAxis->range().lower;
            const double yRadius = (xRadius / qMax(1e-6, xRange)) * yRange; // 保持近似圆形

            dot->topLeft->setCoords(bestKey - xRadius, bestValue + yRadius);
            dot->bottomRight->setCoords(bestKey + xRadius, bestValue - yRadius);
            dot->setBrush(QBrush(color));
            dot->setPen(QPen(color.lighter(130), 1));
            dot->setAntialiased(true);
            dot->setClipToAxisRect(true);
            dot->setVisible(true);

            tipText += QString("\n%1: %2").arg(graph->name()).arg(bestValue, 0, 'f', 2);
        } else {
            dot->setVisible(false);
        }
    }

    // 更新数值提示框
    if (m_cursorValueTip) {
        m_cursorValueTip->setText(tipText);
        m_cursorValueTip->position->setType(QCPItemPosition::ptAxisRectRatio);
        // 放在右上角
        m_cursorValueTip->position->setCoords(0.98, 0.02);
        m_cursorValueTip->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);
        m_cursorValueTip->setVisible(true);
    }

    plot->replot(QCustomPlot::rpQueuedReplot);
}

void Inquire_Sql_Info::creatCPGraph(QCustomPlot* pshowcurvedata)
{
    QList<QCPGraph*> ReagCurveList;
    ReagCurveList.reserve(5);
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

    // 去掉图例显示
    pshowcurvedata->legend->setVisible(false);

    int k = 0;
    QList<QPen>  curveColorList;
    QList<QString> curveNameList;
    curveColorList.reserve(5);
    curveNameList.reserve(5);

    curveColorList.append(QPen(GlobalData::customCurveColor(AA_REAGENT),2,Qt::SolidLine));
    curveNameList.append(QStringLiteral("AA"));
    curveColorList.append(QPen(GlobalData::customCurveColor(ADP_REAGENT),2,Qt::SolidLine));
    curveNameList.append(QStringLiteral("ADP"));
    curveColorList.append(QPen(GlobalData::customCurveColor(EPI_REAGENT),2,Qt::SolidLine));
    curveNameList.append(QStringLiteral("EPI"));
    curveColorList.append(QPen(GlobalData::customCurveColor(COL_REAGENT),2,Qt::SolidLine));
    curveNameList.append(QStringLiteral("COL"));
    curveColorList.append(QPen(GlobalData::customCurveColor(RIS_REAGENT),2,Qt::SolidLine));
    curveNameList.append(QStringLiteral("RIS"));


    // 在曲线上添加项目名称标注
    k = 0;
    for(QCPGraph* pdata : ReagCurveList)
    {
        pdata->setPen(curveColorList.at(k));
        pdata->setName(curveNameList.at(k));
        pdata->setLineStyle(QCPGraph::LineStyle::lsLine);
        pdata->setAntialiasedFill(true);//设置抗锯齿

        // 为每条曲线添加文本标注，标注在曲线上
        QCPItemText *textLabel = new QCPItemText(pshowcurvedata);
        textLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(150, 0);
        textLabel->setText("");
        textLabel->setFont(QFont(QStringLiteral("Microsoft YaHei"), 8, QFont::Bold));
        textLabel->setColor(Qt::white);
        textLabel->setPen(Qt::NoPen);
        textLabel->setBrush(QBrush(curveColorList.at(k).color()));
        textLabel->setPadding(QMargins(5, 2, 5, 2));

        // 存储标签对象以便后续管理
        m_curveLabels.append(textLabel);

        k++;
    }
    // 锁定Y轴范围为0-100（百分比聚集率）
    pshowcurvedata->yAxis->setRange(-20, 100);
    pshowcurvedata->xAxis->setRange(0, 300);
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
    customPlot->setNotAntialiasedElements(QCP::aeNone);
    customPlot->setAntialiasedElements(QCP::aePlottables);

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    //字体设置优化
    QFont plotFont = font();
    plotFont.setFamily(font().family());
    plotFont.setPointSize(10);
    customPlot->setFont(plotFont);


    connect(customPlot,&QCustomPlot::mouseMove,
            this,&Inquire_Sql_Info::myMoveEvent);

    // 初始化鼠标交互：竖直线
    if (!m_vLineCursor) {
        m_vLineCursor = new QCPItemLine(customPlot);
        m_vLineCursor->setPen(QPen(QColor(80, 80, 80, 220), 1, Qt::DashLine));
        m_vLineCursor->setClipToAxisRect(false);
        m_vLineCursor->setVisible(false);
    }

    // 初始化鼠标交互：数值提示框
    if (!m_cursorValueTip) {
        m_cursorValueTip = new QCPItemText(customPlot);
        m_cursorValueTip->setColor(QColor(30, 30, 30));
        m_cursorValueTip->setFont(QFont(font().family(), 9));
        m_cursorValueTip->setPen(QPen(QColor(180, 180, 180)));
        m_cursorValueTip->setBrush(QBrush(QColor(255, 255, 230, 220)));
        m_cursorValueTip->setPadding(QMargins(6, 4, 6, 4));
        m_cursorValueTip->setVisible(false);
    }

    customPlot->setAutoFillBackground(true);
    customPlot->setBackground(QBrush(QColor(255, 255, 255)));
    customPlot->axisRect()->setBackground(QBrush(QColor(255, 255, 255)));
    customPlot->axisRect()->setMinimumMargins(QMargins(52, 16, 18, 40));

    const QColor axisColor(26, 111, 153);
    const QPen axisPen(axisColor, 1);
    const QPen gridPen(QColor(210, 224, 234), 1, Qt::SolidLine);
    customPlot->xAxis->setBasePen(axisPen);
    customPlot->yAxis->setBasePen(axisPen);
    customPlot->xAxis->setTickPen(axisPen);
    customPlot->yAxis->setTickPen(axisPen);
    customPlot->xAxis->setSubTickPen(axisPen);
    customPlot->yAxis->setSubTickPen(axisPen);
    customPlot->xAxis->setTickLabelColor(QColor(26, 51, 68));
    customPlot->yAxis->setTickLabelColor(QColor(26, 51, 68));
    customPlot->xAxis->grid()->setPen(gridPen);
    customPlot->yAxis->grid()->setPen(gridPen);

    customPlot->xAxis->setLabel(tr("时间S"));
    customPlot->yAxis->setLabel(tr("百分比%"));
    customPlot->xAxis->setLabelColor(axisColor);
    customPlot->yAxis->setLabelColor(axisColor);

    // 设置X轴固定范围0-300，Y轴固定范围0-100（百分比聚集率）
    customPlot->xAxis->setRange(0, 300);
    customPlot->yAxis->setRange(-20, 100);

    //刻度设置优化
    QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
    xTicker->setTickStep(30);
    xTicker->setTickCount(10);
    customPlot->xAxis->setTicker(xTicker);

    // Y轴使用自动刻度，根据数据范围自动调整
    QSharedPointer<QCPAxisTicker> yTicker(new QCPAxisTicker);
    customPlot->yAxis->setTicker(yTicker);

    //网格和零线设置
    QPen zeroLinePen(QColor(176, 190, 200), 1, Qt::DashLine);
    customPlot->xAxis->grid()->setZeroLinePen(zeroLinePen);
    customPlot->yAxis->grid()->setZeroLinePen(zeroLinePen);

    /*customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|
                                QCP::iSelectAxes |QCP::iSelectLegend |
                                QCP::iSelectPlottables);*/

    //关联选点信号
    connect(customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)),
            this, SLOT(OnPlotClick(QCPAbstractPlottable*, int, QMouseEvent*)));

    //性能优化：延迟重绘
    QTimer::singleShot(0, customPlot, [customPlot]() {
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });

    creatCPGraph(customPlot);// 创建图例层

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
   ui->Inquire_curve_1->yAxis->setRange(-20, 100);
   ui->Inquire_curve_1->xAxis->setRange(0, 300);
   ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
}


void Inquire_Sql_Info::RecvCurveData(const QVector<QString>& data)
{
    for(int i = 0; i < data.size(); ++i){
        QString curvepos = data[i].simplified();
        if (curvepos.isEmpty()) {
            QLOG_DEBUG() << "跳过空的曲线数据，索引:" << i;
            continue;
        }
        if (curvepos.size() > 100000) {
            QLOG_WARN() << "曲线数据过大，已跳过，索引:" << i;
            continue;
        }
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QStringList valdataList = curvepos.split(',', Qt::SkipEmptyParts);
#else
        QStringList valdataList = curvepos.split(',', QString::SkipEmptyParts);
#endif
        if (valdataList.isEmpty()) {
            QLOG_DEBUG() << "解析后曲线数据为空，索引:" << i;
            continue;
        }
        addInquireCurvedata(valdataList, i + 1);
    }
    // 接收完所有曲线数据后，强制锁定Y轴范围为0-100
    ui->Inquire_curve_1->yAxis->setRange(-20, 100);
    ui->Inquire_curve_1->xAxis->setRange(0, 300);
    ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
    update();
    emit clickOutPdfFile(m_clickId);
    return;
}


void Inquire_Sql_Info::hideCurveFollow()
{
    m_followSampleRow = -1;
    if (m_sampleFrame)
        m_sampleFrame->hide();
    if (m_curveFollow)
        m_curveFollow->hide();
}

bool Inquire_Sql_Info::eventFilter(QObject *watched, QEvent *event)
{
    if (!event || event->type() != QEvent::MouseButtonPress)
        return false;
    if (!m_curveFollow || !m_curveFollow->isVisible())
        return false;

    QWidget *widget = qobject_cast<QWidget *>(watched);
    if (!widget)
        return false;
    if (widget == m_curveFollow || m_curveFollow->isAncestorOf(widget))
        return false;

    const QMouseEvent *mouse = static_cast<const QMouseEvent *>(event);
    if (!rect().contains(mapFromGlobal(mouse->globalPos())))
        return false;

    QTableWidget *table = ui ? ui->tableWidget_SQL_Inquire : nullptr;
    if (table && (widget == table || table->isAncestorOf(widget))) {
        const QPoint viewportPos = table->viewport()->mapFromGlobal(mouse->globalPos());
        if (!table->indexAt(viewportPos).isValid())
            hideCurveFollow();
        return false;
    }

    hideCurveFollow();
    return false;
}

void Inquire_Sql_Info::placeCurveFollow(int sampleStartRow)
{
    QTableWidget *table = ui->tableWidget_SQL_Inquire;
    if (!table || !m_sampleFrame || !m_curveFollow || sampleStartRow < 0
            || sampleStartRow >= table->rowCount()) {
        hideCurveFollow();
        return;
    }

    m_followSampleRow = sampleStartRow;
    const int lastRow = qMin(sampleStartRow + 3, table->rowCount() - 1);
    const int y = table->rowViewportPosition(sampleStartRow);
    int blockH = 0;
    for (int row = sampleStartRow; row <= lastRow; ++row)
        blockH += table->rowHeight(row);

    const QRect viewRect = table->viewport()->rect();
    if (blockH <= 0 || y + blockH <= 0 || y >= viewRect.height()) {
        m_sampleFrame->hide();
        m_curveFollow->hide();
        return;
    }

    m_sampleFrame->setGeometry(QRect(1, y + 1, viewRect.width() - 3, blockH - 2));
    m_sampleFrame->show();
    m_sampleFrame->raise();

    QString sampleId;
    if (QTableWidgetItem *sampleItem = table->item(sampleStartRow, 0))
        sampleId = sampleItem->text();
    if (m_curveFollowTitle) {
        m_curveFollowTitle->setText(sampleId.isEmpty()
            ? QString::fromUtf8("聚集率曲线")
            : QString::fromUtf8("样本 %1    聚集率曲线").arg(sampleId));
    }

    const int panelW = qBound(620, table->width() - 48, 860);
    const int panelH = 340;
    const QPoint rowTop = table->viewport()->mapTo(this, QPoint(0, y));
    int x = table->mapTo(this, QPoint(0, 0)).x() + (table->width() - panelW) / 2;
    int yPanel = rowTop.y() + blockH + 8;
    int topLimit = 8;
    int bottomLimit = height() - 8;
    if (ui->widget_date_)
        topLimit = ui->widget_date_->geometry().bottom() + 6;
    if (ui->widget_curve && ui->widget_curve->isVisible())
        bottomLimit = qMin(bottomLimit, ui->widget_curve->geometry().top() - 6);
    if (yPanel + panelH > bottomLimit) {
        const int above = rowTop.y() - panelH - 8;
        yPanel = above >= topLimit ? above : qMax(topLimit, bottomLimit - panelH);
    }
    if (yPanel < topLimit)
        yPanel = topLimit;
    x = qBound(8, x, qMax(8, width() - panelW - 8));
    m_curveFollow->setGeometry(x, yPanel, panelW, panelH);
    m_curveFollow->show();
    m_curveFollow->raise();
    if (ui->Inquire_curve_1)
        ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
}

void Inquire_Sql_Info::onSelectRowItem(QTableWidgetItem *item)
{
    if (!item)
        return;

    QTableWidget *ptablewidget = ui->tableWidget_SQL_Inquire;
    const int startRow = (item->row() / 4) * 4;
    if (startRow < 0 || startRow >= ptablewidget->rowCount())
        return;
    if (ptablewidget->item(startRow, 0) == nullptr) {
        CustomMessageBox::warning(this, tr("查询曲线失败"), tr("未查询到样本号"));
        QLOG_DEBUG() << "选中查看曲线失败,未查询到样本号" << __FILEW__ << __LINE__ << endl;
        return;
    }
    if (startRow == m_followSampleRow && m_curveFollow && m_curveFollow->isVisible()) {
        hideCurveFollow();
        return;
    }

    m_clickId = ptablewidget->item(startRow, 0)->text();
    m_followSampleRow = startRow;
    ptablewidget->setCurrentCell(startRow, 0);
    const int lastRow = qMin(startRow + 3, ptablewidget->rowCount() - 1);
    const QModelIndex topLeft = ptablewidget->model()->index(startRow, 0);
    const QModelIndex bottomRight = ptablewidget->model()->index(lastRow, ptablewidget->columnCount() - 1);
    ptablewidget->selectionModel()->select(QItemSelection(topLeft, bottomRight),
                                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    cleanPlogtandUpdate();
    placeCurveFollow(startRow);
    emit InquierCurveView(m_clickId);
}


void Inquire_Sql_Info::addInquireCurvedata(QStringList dataList,quint8 indexReag)
{
    if (dataList.isEmpty()) {
        QLOG_WARN() << "Empty data list provided for reagent index:" << indexReag;
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
                        //QLOG_WARN() << "数据点转换失败:" << str;
                        conversionErrors++;
                        return 0.0;
                    }

                    //检查是否为无穷大或NaN
                    if (std::isinf(val) || std::isnan(val)) {
                        //QLOG_WARN() << "特殊值转换失败(inf/nan):" << str;
                        conversionErrors++;
                        return 0.0;
                    }

                    //夹紧到-20~100范围，允许显示负值
                    if (val < -20.0) val = -20.0;
                    if (val > 100.0) val = 100.0;
                    return val;
                 });

    if (conversionErrors == 300) {
        QLOG_DEBUG() << "Found" << conversionErrors << "conversion errors in data for reagent:" << indexReag;
        return;
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
            // 过滤掉 NaN 值
            QVector<double> filteredPosx;
            QVector<double> filteredData;

            for (int i = 0; i < rawData.size(); ++i) {
                if (!std::isnan(rawData[i]) && !std::isnan(posx[i])) {
                    filteredData.append(rawData[i]);
                    filteredPosx.append(posx[i]);
                }
            }

            if (!filteredPosx.isEmpty() && !filteredData.isEmpty()) {
                graph->setData(filteredPosx, filteredData);

                // 立即锁定Y轴范围为0-100（百分比）
                ui->Inquire_curve_1->yAxis->setRange(-20, 100);
                ui->Inquire_curve_1->xAxis->setRange(0, 300);

                // 更新曲线标注位置
                updateCurveLabelsPosition();

                // 执行replot
                ui->Inquire_curve_1->replot();

                // replot后再次强制锁定Y轴范围，防止QCustomPlot内部自动调整
                QTimer::singleShot(0, this, [this]() {
                    if (ui->Inquire_curve_1) {
                        ui->Inquire_curve_1->yAxis->setRange(-20, 100);
                        ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
                    }
                });
            }
        }
    }, Qt::QueuedConnection);
}

void Inquire_Sql_Info::cleanPlogtandUpdate()
{
    // 安全地清理曲线数据，检查指针是否为空
    if (m_showAACpgraph) m_showAACpgraph->data().data()->clear();
    if (m_showADPCpgraph) m_showADPCpgraph->data().data()->clear();
    if (m_showEPICpgraph) m_showEPICpgraph->data().data()->clear();
    if (m_showCOLCpgraph) m_showCOLCpgraph->data().data()->clear();
    if (m_showRISCpgraph) m_showRISCpgraph->data().data()->clear();

    // 注意：曲线标签不应该在这里清理，它们应该在曲线创建时添加并保持显示
    // 只在析构函数中清理曲线标签

    if (ui->Inquire_curve_1) {
        ui->Inquire_curve_1->yAxis->setRange(-20, 100);
        ui->Inquire_curve_1->xAxis->setRange(0, 300);
        ui->Inquire_curve_1->repaint();
        ui->Inquire_curve_1->replot(QCustomPlot::rpQueuedReplot);
    }
    update();
    return;
}

void Inquire_Sql_Info::firstrunthread()
{
    if(!mquiredataclass)
    {
       mquiredataclass = std::make_unique<QueryDataThread>();
       mquiredataclass->moveToThread(&m_threadInqure);
       qRegisterMetaType<InqueryDatastu_t>("InqueryDatastu_t");
       qRegisterMetaType<InquireRowList>("InquireRowList");
       connect(mquiredataclass.get(),&QueryDataThread::LoadInquierdata,this,&Inquire_Sql_Info::ViewLoadInquierdata);
       connect(mquiredataclass.get(),&QueryDataThread::LoadInquierdataList,
               this,&Inquire_Sql_Info::ViewLoadInquierdataList, Qt::QueuedConnection);

       connect(mquiredataclass.get(),&QueryDataThread::clearTableWidget,this,[=](){
           cancelInquireFill();
           cleanPlogtandUpdate();
           DeleteStuData();
       });

       connect(mquiredataclass.get(),&QueryDataThread::InquireEmpty,this,[=](){
            CustomMessageBox::warning(nullptr,tr("查讯完成"),tr("查询结果为空!"));
            return;
       });

       connect(&m_threadInqure,&QThread::started,
               mquiredataclass.get(),&QueryDataThread::_startSycnData);

       connect(this,&Inquire_Sql_Info::FindModuleStyle,
               mquiredataclass.get(),&QueryDataThread::slotFindModuleStyle);

       connect(this,&Inquire_Sql_Info::FindspecifiedData,
               mquiredataclass.get(),&QueryDataThread::slotFindspecifiedData);

       connect(this,&Inquire_Sql_Info::Locatethelookup,
               mquiredataclass.get(),&QueryDataThread::slotLocatethelookup);

       connect(this,&Inquire_Sql_Info::InquierCurveView,
               mquiredataclass.get(),
               &QueryDataThread::InquierCurveViewEnd);

       connect(mquiredataclass.get(),&QueryDataThread::sendCurveData,
                this,&Inquire_Sql_Info::RecvCurveData,
                Qt::QueuedConnection);

       connect(this,&Inquire_Sql_Info::clickOutPdfFile,
               mquiredataclass.get(),
               &QueryDataThread::ObatinCreatPdfPara);

       connect(mquiredataclass.get(),&QueryDataThread::outPDFPara,this,[=](InqueryDatastu_t *pdata){
           if(pdata){
           }
       });

       m_threadInitialized = true;
       m_threadInqure.start();
    }
}



void Inquire_Sql_Info::on_toolButton_OK_clicked()
{
    firstrunthread(); //确保线程启动
    quint8 index_ = m_group->checkedId();
    if(index_ == INQUIRE_SPECIFIC_SQL)
    {
        QDateTime time1 = dateEdit_begin->dateTime();
        QDateTime time2 = dateEdit_end->dateTime();
        int days = time1.daysTo(time2);
        if (days >= 0)
        {
            emit FindspecifiedData(dateEdit_begin,days);
        }
        else
        {
            CustomMessageBox::warning(nullptr, tr("提示"), tr("选择开始时间应小于(或等于)结束时间!"));
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
    QString  inquierId = sanitizeInput(spinBox_label_Sample->text());
    QString  senddoctor = sanitizeInput(comboBox_sendingdoctor->currentText());
    QString  PatientsName = sanitizeInput(lineEdit_Name->text());
    QString  PatientsAge = sanitizeInput(comboBox_age->currentText());
    QString  bbednum = sanitizeInput(lineEdit_bednum->text());
    QString  sectionkind = sanitizeInput(comboBox_Section->currentText());
    emit this->Locatethelookup(inquierId,senddoctor,PatientsName,PatientsAge,sectionkind,bbednum);
    return;
}

//清空查找到的数据
void Inquire_Sql_Info::DeleteStuData()
{
    hideCurveFollow();
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
            QString data_ = resultdata.at(i);
            data_.remove('%');
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
    QString today_ = current_date_time.toString("yyyyMMdd");
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
    int days = dateEdit_begin->dateTime().daysTo(dateEdit_end->dateTime());
    if(days < 0)
    {
        CustomMessageBox::warning(nullptr,tr("提示"),tr("选择开始时间应小于(或等于)结束时间!"));
        return;
    }

    const int MAX_QUERY_DAYS = 365;
    if (days > MAX_QUERY_DAYS)
    {
        CustomMessageBox::warning(nullptr, tr("提示"),
            tr("查询范围过大，最多支持 %1 天的统计查询，请缩小时间范围!").arg(MAX_QUERY_DAYS));
        return;
    }

    for (int i = 0; i < days; i++)
    {
        QMap<quint8,int> used_reagent;
        used_reagent.clear();
        QString need_days = dateEdit_begin->dateTime().addDays(i).toString("yyyyMMdd");
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
        CustomMessageBox::warning(this,"指定统计查询失败","查询数据异常");
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
    if (!pdata) return;

    QMutexLocker locker(&m_dataMutex);
    InsertOneRowsData(pdata);
    m_del.push_back(pdata);
    if(numtotal == 0) return;
    if (n_ing == 1 || n_ing == numtotal || (n_ing % 20) == 0) {
        ui->progressBarLoad->show();
        const double ratio_ = n_ing * 100.0 / numtotal;
        ui->progressBarLoad->setValue(static_cast<int>(ratio_));
        ui->progressBarLoad->setFormat(tr("当前查讯进度为:%1%").arg(ratio_, 0, 'f', 2));
        ui->progressBarLoad->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    if(numtotal == n_ing)
    {
        qDeleteAll(m_del.begin(),m_del.end());
        m_del.clear();
        QTimer::singleShot(3000, this, [=](){
            ui->progressBarLoad->hide();
            ui->progressBarLoad->setValue(0);
        });
    }
}

void Inquire_Sql_Info::setInquireBusy(bool busy)
{
    if (toolButton_OK)
        toolButton_OK->setEnabled(!busy);
    if (toolButtonFindexatc)
        toolButtonFindexatc->setEnabled(!busy);
    if (toolButton_stats)
        toolButton_stats->setEnabled(!busy);
}

void Inquire_Sql_Info::cancelInquireFill()
{
    m_inquireFilling = false;
    qDeleteAll(m_pendingInquireRows);
    m_pendingInquireRows.clear();
    m_inquireFillIndex = 0;
    if (ui && ui->tableWidget_SQL_Inquire)
        ui->tableWidget_SQL_Inquire->setUpdatesEnabled(true);
    setInquireBusy(false);
}

void Inquire_Sql_Info::ViewLoadInquierdataList(InquireRowList rows)
{
    cancelInquireFill();
    if (rows.isEmpty()) {
        return;
    }

    m_pendingInquireRows = rows;
    m_inquireFillIndex = 0;
    m_inquireFilling = true;
    setInquireBusy(true);

    QTableWidget *table = ui->tableWidget_SQL_Inquire;
    table->setUpdatesEnabled(false);

    ui->progressBarLoad->show();
    ui->progressBarLoad->setRange(0, qMax(1, rows.size()));
    ui->progressBarLoad->setValue(0);
    ui->progressBarLoad->setFormat(tr("当前查讯进度为:0%"));
    ui->progressBarLoad->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QTimer::singleShot(0, this, [this]() { fillInquireChunk(); });
}

void Inquire_Sql_Info::fillInquireChunk()
{
    if (!m_inquireFilling)
        return;

    QTableWidget *table = ui->tableWidget_SQL_Inquire;
    const int total = m_pendingInquireRows.size();
    const int chunkEnd = qMin(m_inquireFillIndex + 30, total);

    for (; m_inquireFillIndex < chunkEnd; ++m_inquireFillIndex) {
        InsertOneRowsData(m_pendingInquireRows.at(m_inquireFillIndex));
    }

    if (total > 0) {
        const double ratio_ = m_inquireFillIndex * 100.0 / total;
        ui->progressBarLoad->setValue(m_inquireFillIndex);
        ui->progressBarLoad->setFormat(tr("当前查讯进度为:%1%").arg(ratio_, 0, 'f', 2));
    }

    if (m_inquireFillIndex < total) {
        QTimer::singleShot(0, this, [this]() { fillInquireChunk(); });
        return;
    }

    table->setUpdatesEnabled(true);
    qDeleteAll(m_pendingInquireRows);
    m_pendingInquireRows.clear();
    m_inquireFilling = false;
    m_inquireFillIndex = 0;
    setInquireBusy(false);

    QTimer::singleShot(3000, this, [this]() {
        if (!m_inquireFilling) {
            ui->progressBarLoad->hide();
            ui->progressBarLoad->setValue(0);
        }
    });
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

    if(AA_Result.size() == 4 && ADP_Result.size() == 4 && EPI_Result.size() == 4
            && COL_Result.size()== 4 && RIS_Result.size() == 4)
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
            InsertInqireResult(row,iter.key(), QStringLiteral("%1 [60S]").arg(iter.value()));
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
            InsertInqireResult(row,iter.key(), QStringLiteral("%1 [180S]").arg(iter.value()));
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
            InsertInqireResult(row,iter.key(), QStringLiteral("%1 [300S]").arg(iter.value()));
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
            InsertInqireResult(row,iter.key(), QStringLiteral("%1 [MAX]").arg(iter.value()));
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
    }
}




void Inquire_Sql_Info::on_toolButton_outPrint_clicked()
{
    // 获取数据库实例
   auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
   if (!dbInstance) {
       QLOG_ERROR() << "Database instance is null";
       return;
   }

   // 创建打印窗口并设置自动删除
   PrintTemplate *printWidget = new PrintTemplate(nullptr, dbInstance);
   printWidget->setAttribute(Qt::WA_DeleteOnClose);

   // 设置样本号
   printWidget->setSampleId(m_clickId);

   // 获取并设置粘附率显示选项
   INI_File iniConfig;
   bool showAdhesion = iniConfig.getPrintAdhesion();
   printWidget->setShowAdhesionRate(showAdhesion);

   // 可选：设置窗口标题
   printWidget->setWindowTitle(QString("检验报告单 - %1").arg(m_clickId));

   // 显示窗口
   printWidget->show();
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


const QString& Inquire_Sql_Info::getScrollBarStyle()
{
    static const QString style =
        "QScrollBar:vertical {width: 18px; background: transparent; margin: 0px,0px,0px,0px; padding-top: 18px;padding-bottom: 18px;} "
        "QScrollBar::handle:vertical {width: 18px; background: rgba(0,0,0,25%);border-radius: 4px;min-height: 20;}"
        "QScrollBar::handle:vertical:hover {width: 8px;background: rgba(0,0,0,50%);border-radius: 4px; min-height: 20;}"
        "QScrollBar::add-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
        "QScrollBar::sub-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
        "QScrollBar::add-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
        "QScrollBar::sub-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
        "QScrollBar::sub-page:vertical {background: rgb(178,180,180); border-radius: 0px;}"
        "QScrollBar::add-page:vertical {background: rgb(178,180,180); border-radius: 0px;}";
    return style;
}

const QString& Inquire_Sql_Info::getTableWidgetStyle()
{
    static const QString style =
        "QTableWidget::item:hover{background-color:rgb(70 ,130 ,180)}"
        "QTableWidget{border:1px solid #696969;}"
        "QTableWidget::item:selected{background-color:rgb(139, 139, 122)}"
        "QTableView QTableCornerButton::section{color: white; background-color: rgb(188, 187, 186); "
        "border: 1px solid rgb(188, 187, 186);border-radius:0px; border-color: rgb(188, 187, 186);"
        "font: bold 1pt;padding:12px 0 0 10px}"
        "QHeaderView::section,QTableCornerButton:section{ "
        "padding:3px; margin:0px; color:rgba(188, 187, 186, 255);  border:1px solid rgba(188, 187, 186, 255); "
        "border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; "
        "background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
        "QTableWidget{background-color:white;border:1px;}"
        "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}"
        "QTableView QTableCornerButton::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}";
    return style;
}

void Inquire_Sql_Info::adjustYAxisRange()
{
    if (!ui->Inquire_curve_1) {
        return;
    }

    // 百分比聚集率，固定Y轴范围 0 - 100
    ui->Inquire_curve_1->yAxis->setRange(-20, 100);
}




void Inquire_Sql_Info::updateCurveLabelsPosition()
{
    if (!ui->Inquire_curve_1 || m_curveLabels.isEmpty()) {
        return;
    }

    // 定义曲线指针列表
    QList<QCPGraph*> graphs = {
        m_showAACpgraph, m_showADPCpgraph, m_showEPICpgraph,
        m_showCOLCpgraph, m_showRISCpgraph
    };

    // 定义固定位置映射：曲线索引 -> X坐标位置
    QMap<int, double> fixedPositions;
    fixedPositions[0] = 30;  // AA -> 30秒
    fixedPositions[1] = 60;  // ADP -> 60秒
    fixedPositions[2] = 120; // EPI -> 120秒
    fixedPositions[3] = 90;  // COL -> 90秒
    fixedPositions[4] = 150; // RIS -> 150秒

    // 更新每条曲线的标注位置
    for (int i = 0; i < graphs.size() && i < m_curveLabels.size(); ++i) {
        QCPGraph* graph = graphs.at(i);
        QCPItemText* label = m_curveLabels.at(i);

        if (!graph || !label) {
            continue;
        }

        // 根据曲线是否有数据显示或隐藏名称
        if (graph->dataCount() > 0) {
            // 有数据时显示曲线名称
            QString curveName = "";
            switch (i) {
                case 0: curveName = "AA"; break;
                case 1: curveName = "ADP"; break;
                case 2: curveName = "EPI"; break;
                case 3: curveName = "COL"; break;
                case 4: curveName = "RIS"; break;
            }
            label->setText(curveName);
            label->setVisible(true); // 显示整个标签框

            // 获取曲线的数据
            auto data = graph->data();

            // 找到固定位置对应的Y值
            double targetX = fixedPositions.value(i, 150); // 默认150秒
            double closestY = 0;
            double minDistance = std::numeric_limits<double>::max();

            for (auto it = data->constBegin(); it != data->constEnd(); ++it) {
                double x = (*it).key;
                double y = (*it).value;
                double distance = std::abs(x - targetX);

                if (distance < minDistance) {
                    minDistance = distance;
                    closestY = y;
                }
            }

            const double lift = 10.0 + (i % 3) * 8.0;
            double labelY = closestY + lift;
            Qt::Alignment align = Qt::AlignHCenter | Qt::AlignBottom;
            if (labelY > 96.0) {
                labelY = closestY - lift;
                align = Qt::AlignHCenter | Qt::AlignTop;
            }
            label->setPositionAlignment(align);
            label->position->setCoords(targetX, labelY);
        } else {
            // 没有数据时隐藏整个标签框
            label->setText("");
            label->setVisible(false); // 隐藏整个标签框
        }
    }

    // 更新标注位置后，强制锁定Y轴范围为0-100
    ui->Inquire_curve_1->yAxis->setRange(-20, 100);
    ui->Inquire_curve_1->xAxis->setRange(0, 300);
    ui->Inquire_curve_1->replot();
}



void Inquire_Sql_Info::onTableContextMenu(const QPoint &pos)
{
    QTableWidget *table = ui->tableWidget_SQL_Inquire;
    QTableWidgetItem *item = table->itemAt(pos);
    if (!item) return;   // 点击在空白区域

    int currentRow = item->row();
    // 确保选中有效行（表格可能有空行保护）
    if (currentRow < 0 || currentRow >= table->rowCount()) return;

    // 弹出菜单
    QMenu menu;
    menu.setStyleSheet(medicalMenuStyle);          // 应用医疗器械样式
    menu.setAttribute(Qt::WA_TranslucentBackground); // 可选，提升视觉效果
    QAction *deleteAction = menu.addAction(tr("删除该项"));
    QAction *selectedAction = menu.exec(QCursor::pos());
    if (selectedAction == deleteAction) {
        deleteSelectedItem();
    }
}

void Inquire_Sql_Info::deleteSelectedItem()
{
    if(cglobal::g_StartTesting){
        CustomMessageBox::warning(this, tr("删除失败"), tr("请测试完成后在删除!"));
        return;
    }

    QTableWidget *table = ui->tableWidget_SQL_Inquire;
    int currentRow = table->currentRow();
    if (currentRow < 0 || currentRow >= table->rowCount()) {
        CustomMessageBox::warning(this, tr("删除失败"), tr("请先选中要删除的行"));
        return;
    }

    // 每个样本占用 4 行，计算起始行（样本信息所在行）
    int startRow = (currentRow / 4) * 4;
    if (startRow + 3 >= table->rowCount()) {
        CustomMessageBox::warning(this, tr("删除失败"), tr("选中的数据不完整"));
        return;
    }

    // 获取样本号（合并单元格在第0列，位于 startRow 行）
    QTableWidgetItem *sampleItem = table->item(startRow, 0);
    if (!sampleItem) {
        CustomMessageBox::warning(this, tr("删除失败"), tr("无法获取样本号"));
        return;
    }
    QString sampleId = sampleItem->text();

    // 确认删除对话框
    QMessageBox::StandardButton ret = CustomMessageBox::question(this, tr("确认删除"),
                                         tr("确定要删除样本号为 %1 的所有数据吗？\n该操作不可恢复！").arg(sampleId),
                                         QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    // 从数据库中删除
    if (!deleteSampleFromDatabase(sampleId)) {
        CustomMessageBox::warning(this, tr("删除失败"), tr("数据库删除失败，请稍后重试"));
        return;
    }

    // 从表格中删除对应的 4 行（注意行号会动态变化，必须一次性删除或从后往前删）
    // 这里从 startRow 开始删除 4 行，由于每次删除都会影响后续行号，但 startRow 始终是未删除前的起始行
    for (int i = 0; i < 4; ++i) {
        table->removeRow(startRow);
    }

    // 如果删除后表格为空，可以考虑清空曲线显示
    if (table->rowCount() == 0) {
        cleanPlogtandUpdate();
    }
    hideCurveFollow();

    Q_EMIT sycnDelOthertableWidgetRow(sampleId);
    Q_EMIT sycnDelTestSample(sampleId); //取消分配了试管了样本号

    CustomMessageBox::information(this, tr("删除成功"), tr("数据已删除"));
}

bool Inquire_Sql_Info::deleteSampleFromDatabase(const QString &sampleId)
{
    if (sampleId.isEmpty() || sampleId.trimmed().isEmpty()) {
        QLOG_WARN() << "删除样本号为空，已拒绝操作";
        return false;
    }

    auto *db = FullyAutomatedPlatelets::pinstancesqlData();
    if (!db) {
        QLOG_ERROR() << "数据库实例为空，无法删除样本:" << sampleId;
        return false;
    }

    bool result1 = db->deleteTestResultTable(sampleId);
    bool result2 = db->deleteTestCurveDataTale(sampleId);

    if (!result1 || !result2) {
        QLOG_WARN() << "删除样本部分失败，结果表:" << result1 << "，曲线表:" << result2 << "，样本号:" << sampleId;
    }

    return result1 && result2;
}

QString Inquire_Sql_Info::sanitizeInput(const QString& input)
{
    if (input.isEmpty())
        return QString();

    QString result = input.trimmed();

    if (result.size() > 100)
        result = result.left(100);

    static const QString dangerousChars = "'\"\\;--/*%<>|&=";
    QString filtered;
    filtered.reserve(result.size());
    for (QChar c : result) {
        if (!dangerousChars.contains(c)) {
            filtered.append(c);
        }
    }

    if (filtered.contains("drop", Qt::CaseInsensitive) ||
        filtered.contains("delete", Qt::CaseInsensitive) ||
        filtered.contains("union", Qt::CaseInsensitive) ||
        filtered.contains("select", Qt::CaseInsensitive) ||
        filtered.contains("insert", Qt::CaseInsensitive) ||
        filtered.contains("update", Qt::CaseInsensitive)) {
        QLOG_WARN() << "检测到潜在的可疑输入:" << input;
        filtered.clear();
    }

    return filtered;
}

void Inquire_Sql_Info::initInputValidators()
{
    if (lineEdit_Name) {
        lineEdit_Name->setMaxLength(20);
    }
    if (lineEdit_bednum) {
        lineEdit_bednum->setMaxLength(20);
    }
    if (spinBox_label_Sample) {
        spinBox_label_Sample->setMinimum(1);
        spinBox_label_Sample->setMaximum(999999);
    }
}

void Inquire_Sql_Info::cleanupMemory()
{
    QMutexLocker locker(&m_dataMutex);
    if (!m_del.isEmpty()) {
        qDeleteAll(m_del.begin(), m_del.end());
        m_del.clear();
    }
}

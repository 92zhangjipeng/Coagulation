#include "correct_data.h"
#include "globaldata.h"
#include "ini_file.h"
//#include "ui_correct_data.h"
#include <QFuture>
#include <QMessageBox>
#include <QtConcurrent>
#include "QsLog/include/QsLog.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

bool Correct_Data::RetestHight = false;

Correct_Data::Correct_Data(QWidget *parent) :
    QWidget(parent),
    m_rows(0),
    m_Index(0),
    m_Samplename(""),
    m_originclickData(""),
    m_sampleDate("")
    //ui(new Ui::Correct_Data)
{
    //ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    // 动态创建UI（替代.ui文件中的布局）
    setupUI();
    applyStyles();

    initCustomTitleBar();

    this->setFixedSize(360, 150);

    // 安装事件过滤器
    m_lineEditChangeText->installEventFilter(this);
    m_lineEditChangeText->setEchoMode(QLineEdit::Normal);
}

Correct_Data::~Correct_Data()
{
    //delete ui;
}

void Correct_Data::setupUI()
{
    // 创建主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    // ==================== 内容区域（标签+输入框） ====================
    m_widgetText = new QWidget(this);
    m_widgetText->setObjectName("widget_text");

    QHBoxLayout *textLayout = new QHBoxLayout(m_widgetText);
    textLayout->setSpacing(8);
    textLayout->setContentsMargins(12, 12, 12, 12);

    // 标签
    m_labelText = new QLabel(tr("测量值"), m_widgetText);
    m_labelText->setMinimumSize(100, 28);
    m_labelText->setMaximumSize(16777215, 35);
    m_labelText->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    m_labelText->setObjectName("label_Text");

    // 输入框
    m_lineEditChangeText = new QLineEdit(m_widgetText);
    m_lineEditChangeText->setMinimumSize(150, 32);
    m_lineEditChangeText->setMaximumSize(200, 35);
    m_lineEditChangeText->setAlignment(Qt::AlignCenter);
    m_lineEditChangeText->setObjectName("lineEdit_changetext");

    // 水平弹性空间
    QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    textLayout->addWidget(m_labelText);
    textLayout->addWidget(m_lineEditChangeText);
    textLayout->addItem(horizontalSpacer);

    // ==================== 按钮区域 ====================
    m_widgetButtons = new QWidget(this);
    m_widgetButtons->setMaximumSize(16777215, 50);
    m_widgetButtons->setObjectName("widget");

    QHBoxLayout *buttonLayout = new QHBoxLayout(m_widgetButtons);
    buttonLayout->setSpacing(12);
    buttonLayout->setContentsMargins(12, 6, 12, 6);

    // 重测按钮
    m_toolButtonRetest = new QToolButton(m_widgetButtons);
    m_toolButtonRetest->setMaximumSize(80, 30);
    m_toolButtonRetest->setText(tr("重测高度"));
    m_toolButtonRetest->setObjectName("toolButton_Retest");

    // 确定按钮
    m_toolButtonOk = new QToolButton(m_widgetButtons);
    m_toolButtonOk->setMaximumSize(80, 30);
    m_toolButtonOk->setText(tr("确定"));
    m_toolButtonOk->setObjectName("toolButton_Ok");

    // 取消按钮
    m_toolButtonCancel = new QToolButton(m_widgetButtons);
    m_toolButtonCancel->setMaximumSize(80, 30);
    m_toolButtonCancel->setText(tr("取消"));
    m_toolButtonCancel->setObjectName("toolButton_Cancel");

    buttonLayout->addWidget(m_toolButtonRetest);
    buttonLayout->addWidget(m_toolButtonOk);
    buttonLayout->addWidget(m_toolButtonCancel);

    // 添加到主布局
    mainLayout->addWidget(m_widgetText);
    mainLayout->addWidget(m_widgetButtons);

    // 连接信号槽
    connect(m_toolButtonOk, &QToolButton::clicked, this, &Correct_Data::onClickokBtn);
    connect(m_toolButtonCancel, &QToolButton::clicked, this, &Correct_Data::onClickCancelBtn);
    connect(m_toolButtonRetest, &QToolButton::clicked, this, &Correct_Data::onClickResetHeighBtn);
}

void Correct_Data::applyStyles()
{
    // 主窗口样式
    this->setStyleSheet(
        "QWidget#Correct_Data {"
        "    background-color: #F5F8FC;"
        "}"
    );

    // 内容面板背景
    m_widgetText->setStyleSheet(
        "QWidget#widget_text {"
        "    background-color: #FFFFFF;"
        "    border: 1px solid #D9E5F0;"
        "    border-radius: 6px;"
        "}"
    );

    m_widgetButtons->setStyleSheet(
        "QWidget#widget {"
        "    background-color: #FFFFFF;"
        "    border: 1px solid #D9E5F0;"
        "    border-radius: 6px;"
        "}"
    );

    // 标签样式
    m_labelText->setStyleSheet(
        "QLabel {"
        "    background: transparent;"
        "    color: #1A5A8C;"
        "    font-weight: 500;"
        "    font-family: '微软雅黑';"
        "    font-size: 12px;"
        "}"
    );

    // 输入框样式
    m_lineEditChangeText->setStyleSheet(
        "QLineEdit {"
        "    border-radius: 4px;"
        "    padding: 4px 8px;"
        "    font: 11px '微软雅黑';"
        "    border: 1px solid #C5D9E8;"
        "    background-color: white;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #4A9FDC;"
        "    background-color: #FFFFFF;"
        "}"
        "QLineEdit:hover {"
        "    border: 1px solid #8FBCE0;"
        "}"
    );

    // 按钮统一样式
    QString buttonStyle =
        "QToolButton {"
        "    background-color: #E8F0FE;"
        "    border: 1px solid #B8D4F0;"
        "    border-radius: 4px;"
        "    color: #1A5A8C;"
        "    font-family: '微软雅黑';"
        "    font-size: 12px;"
        "    font-weight: normal;"
        "    padding: 6px 12px;"
        "    min-width: 70px;"
        "}"
        "QToolButton:hover {"
        "    background-color: #D0E4F8;"
        "    border: 1px solid #8FBCE0;"
        "    color: #0D3F66;"
        "}"
        "QToolButton:pressed {"
        "    background-color: #B8D4F0;"
        "    border: 1px solid #6A9FC0;"
        "}";

    m_toolButtonOk->setStyleSheet(buttonStyle);
    m_toolButtonCancel->setStyleSheet(buttonStyle);

    // 重测按钮单独样式（稍小）
    m_toolButtonRetest->setStyleSheet(
        "QToolButton {"
        "    background-color: #E8F0FE;"
        "    border: 1px solid #B8D4F0;"
        "    border-radius: 3px;"
        "    color: #1A5A8C;"
        "    font-family: '微软雅黑';"
        "    font-size: 10px;"
        "    padding: 4px 8px;"
        "}"
        "QToolButton:hover {"
        "    background-color: #D0E4F8;"
        "    border: 1px solid #8FBCE0;"
        "}"
        "QToolButton:pressed {"
        "    background-color: #B8D4F0;"
        "}"
    );
}

void Correct_Data::setrows(const int &row) { m_rows = row; }
void Correct_Data::setcols(const int &col) { m_Index = col; }

void Correct_Data::setorigindata(const QString &Originaldata)
{
    m_originclickData = Originaldata;
    repLoading();
}

void Correct_Data::repLoading()
{
    if (m_Index == Cols_Sample) {
        setWindowTitle(tr("修改样本编号"));

        int id;
        GlobalData::apartSampleId(m_originclickData, m_sampleDate, id);

        m_labelText->setText(tr("修改样本号:"));
        m_toolButtonRetest->hide();
        m_lineEditChangeText->setText(QString::number(id));

    } else if (m_Index == Cols_BloodHeight) {
        // 根据全血模式决定是否显示重测按钮
        bool isWholeBloodMode = INI_File().GetWholeBloodModel();
        m_toolButtonRetest->setVisible(isWholeBloodMode);

        QString titleName = tr("修正测高值(%1)").arg(m_originclickData);
        QString labelText = tr("修本样本测高值(%1)").arg(m_originclickData);

        setWindowTitle(titleName);
        m_labelText->setText(labelText);
    }
}

void Correct_Data::onClickokBtn()
{
    const QString inputText = m_lineEditChangeText->text().trimmed();

    // 通用空值检查
    if (inputText.isEmpty()) {
        showErrorMessage(m_Index);
        return;
    }

    bool processingSuccess = false;

    switch (m_Index) {
    case IndexCols::Cols_Sample:
        processingSuccess = processSampleChange(inputText);
        break;

    case IndexCols::Cols_BloodHeight:
        processingSuccess = processBloodHeightChange(inputText);
        break;

    default:
        QMessageBox::warning(this, tr("错误"), tr("未知的操作类型"));
        break;
    }

    if (processingSuccess) {
        m_lineEditChangeText->clear();
        close();
    }
}

void Correct_Data::onClickCancelBtn()
{
    m_lineEditChangeText->clear();
    this->close();
}

void Correct_Data::onClickResetHeighBtn()
{
    RetestHight = true;
    QString reminderText = QString("请重测%1血样管高度").arg(m_Samplename);
    QMessageBox::information(this, tr("测高重测"), reminderText);

    emit repTestHeight(m_Samplename);
}

void Correct_Data::ClickSamplename(const QString Samplename)
{
    m_Samplename = Samplename;
}

bool Correct_Data::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_lineEditChangeText) {
        if (QEvent::FocusIn == event->type()) {
            // 可在此添加焦点进入时的处理逻辑
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Correct_Data::closeEvent(QCloseEvent *)
{
    // 可在此添加关闭时的处理逻辑
}

bool Correct_Data::processSampleChange(const QString &inputText)
{
    bool ok = false;
    int changeId = inputText.toInt(&ok);

    if (!ok || changeId <= 0) {
        QMessageBox::information(this, tr("保存失败"), tr("样本号必须为正整数!"));
        return false;
    }

    emit ConfigureData(m_rows, m_Index, GlobalData::groupDateAndID(m_sampleDate, changeId));
    return true;
}

bool Correct_Data::processBloodHeightChange(const QString &inputText)
{
    if (inputText.trimmed().isEmpty()) {
        QMessageBox::information(this, tr("保存失败"), tr("修正测高值不能为空!"));
        return false;
    }

    bool isNumber = false;
    double heightValue = inputText.toDouble(&isNumber);
    if (!isNumber || heightValue < 0) {
        QMessageBox::information(this, tr("保存失败"), tr("请输入有效的数值!"));
        return false;
    }

    emit NotifyTestHeight(m_rows, m_Index, inputText);
    QLOG_DEBUG() << "修改第" << m_Index << "列，值:" << inputText;
    return true;
}

void Correct_Data::showErrorMessage(int index)
{
    switch (index) {
    case IndexCols::Cols_Sample:
        QMessageBox::information(this, tr("保存失败"), tr("样本号不能为空!"));
        break;
    case IndexCols::Cols_BloodHeight:
        QMessageBox::information(this, tr("保存失败"), tr("修正测高值不能为空!"));
        break;
    default:
        QMessageBox::warning(this, tr("错误"), tr("输入内容不能为空"));
        break;
    }
}

//=============================================================================
// 自定义标题栏相关函数
//=============================================================================

void Correct_Data::initCustomTitleBar()
{
    QLayout *existingLayout = layout();
    if (!existingLayout) {
        QVBoxLayout *newLayout = new QVBoxLayout(this);
        newLayout->setContentsMargins(0, 0, 0, 0);
        newLayout->setSpacing(0);
        existingLayout = newLayout;
    }

    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle("调整任务参数");

    connect(m_titleBar, &CustomTitleBar::closeRequested, this, &Correct_Data::onCloseRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &Correct_Data::onMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &Correct_Data::onMaximizeRequested);

    if (QBoxLayout *boxLayout = qobject_cast<QBoxLayout*>(existingLayout)) {
        boxLayout->insertWidget(0, m_titleBar);
    } else {
        QWidget().setLayout(existingLayout);
        QVBoxLayout *newLayout = new QVBoxLayout(this);
        newLayout->addWidget(m_titleBar);
        while (QLayoutItem *item = existingLayout->takeAt(0)) {
            if (item->widget()) {
                newLayout->addWidget(item->widget());
            }
            delete item;
        }
        delete existingLayout;
        existingLayout = newLayout;
    }

    existingLayout->setContentsMargins(0, 0, 0, 0);
    existingLayout->setSpacing(0);
}

void Correct_Data::onCloseRequested()
{
    close();
}

void Correct_Data::onMinimizeRequested()
{
    showMinimized();
}

void Correct_Data::onMaximizeRequested()
{
    if (isMaximized()) {
        showNormal();
        m_titleBar->updateMaximizeButton(false);
    } else {
        showMaximized();
        m_titleBar->updateMaximizeButton(true);
    }
}

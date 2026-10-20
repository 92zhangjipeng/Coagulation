// ParameterDialog.cpp
#include "ParameterDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

ParameterDialog::ParameterDialog(QWidget *parent)
    : QDialog(parent)
    , m_delayLabel(nullptr)
    , m_colDelaySpin(nullptr)
    , m_delayGroup(nullptr)
{
    // 初始化数据
    QStringList reagents = {"AA", "ADP", "EPI", "COL", "RIS"};
    for (const QString& name : reagents) {
        ReagentData data;
        data.offset = 0.0;
        data.colDelay = 0;
        data.triggerInvalid = 0;
        data.offsetRatio = 0;
        data.invalidMin = 0;
        data.invalidMax = 0;
        data.hasDelay = (name == "COL");
        m_reagentData[name] = data;
    }
    m_currentReagent = "AA";

    setupUI();
    setupStyle();
    setWindowTitle(tr("测试参数"));
    setModal(true);
    setFixedSize(520, 560);

    loadReagentData(m_currentReagent);
    updateDelayGroupVisibility(m_currentReagent);
}

void ParameterDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // ----------------------------------------------
    // 试剂选择下拉框
    // ----------------------------------------------
    QHBoxLayout *reagentSelectLayout = new QHBoxLayout;
    reagentSelectLayout->setSpacing(15);

    QLabel *reagentLabel = new QLabel(tr("选择试剂 :"), this);
    reagentLabel->setAlignment(Qt::AlignCenter);
    reagentLabel->setStyleSheet("font: bold 11pt 'Segoe UI'; color: #2c5f8a; background: transparent;");

    m_selectReagent = new QComboBox(this);
    m_selectReagent->addItems({"AA", "ADP", "EPI", "COL", "RIS"});
    m_selectReagent->setFixedWidth(150);
    m_selectReagent->setToolTip(tr("选择要查看/编辑参数的试剂"));

    reagentSelectLayout->addStretch();
    reagentSelectLayout->addWidget(reagentLabel);
    reagentSelectLayout->addWidget(m_selectReagent);
    reagentSelectLayout->addStretch();

    mainLayout->addLayout(reagentSelectLayout);

    // 分隔线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #b8d4e8; max-height: 1px;");
    mainLayout->addWidget(line);

    // ----------------------------------------------
    // 参数区域 - 使用表格布局整齐排列
    // ----------------------------------------------
    QGroupBox *paramGroup = new QGroupBox(tr("参数设置"), this);
    paramGroup->setStyleSheet("QGroupBox { background: rgba(255,255,255,0.5); border: 1px solid #b8d4e8; border-radius: 10px; }"
                              "QGroupBox::title { color: #1a5276; background: transparent; }");

    QGridLayout *paramGrid = new QGridLayout(paramGroup);
    paramGrid->setHorizontalSpacing(15);
    paramGrid->setVerticalSpacing(12);
    paramGrid->setContentsMargins(20, 25, 20, 20);

    // 创建标签
    QLabel *offsetLabel = new QLabel(tr("测试偏移 :"), this);
    QLabel *triggerLabel = new QLabel(tr("触发无效 :"), this);
    QLabel *offsetRatioLabel = new QLabel(tr("纠正偏移 :"), this);
    QLabel *minLabel = new QLabel(tr("起始时间 :"), this);
    QLabel *maxLabel = new QLabel(tr("终止时间 :"), this);

    // 设置标签样式
    QString labelStyle = "color: #1a5276; font: 10pt 'Segoe UI'; background: transparent;";
    offsetLabel->setStyleSheet(labelStyle);
    triggerLabel->setStyleSheet(labelStyle);
    offsetRatioLabel->setStyleSheet(labelStyle);
    minLabel->setStyleSheet(labelStyle);
    maxLabel->setStyleSheet(labelStyle);

    offsetLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    triggerLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    offsetRatioLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    minLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    maxLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 创建控件
    m_offsetSpin = new QDoubleSpinBox;
    m_offsetSpin->setRange(0.0, 100.0);
    m_offsetSpin->setDecimals(1);
    m_offsetSpin->setSuffix(" %");
    m_offsetSpin->setFixedWidth(130);
    m_offsetSpin->setAlignment(Qt::AlignCenter);

    m_triggerInvalidSpin = new QSpinBox;
    m_triggerInvalidSpin->setRange(0, 100);
    m_triggerInvalidSpin->setSuffix("%");
    m_triggerInvalidSpin->setFixedWidth(130);
    m_triggerInvalidSpin->setAlignment(Qt::AlignCenter);
    m_triggerInvalidSpin->setToolTip(tr("取值范围 0 ~ 100"));

    m_offsetRatioSpin = new QSpinBox;
    m_offsetRatioSpin->setRange(0, 100);
    m_offsetRatioSpin->setSuffix("%");
    m_offsetRatioSpin->setFixedWidth(130);
    m_offsetRatioSpin->setAlignment(Qt::AlignCenter);
    m_offsetRatioSpin->setToolTip(tr("取值范围 0 ~ 100"));

    m_invalidMinSpin = new QSpinBox;
    m_invalidMinSpin->setRange(0, 100);
    m_invalidMinSpin->setSuffix("s");
    m_invalidMinSpin->setFixedWidth(130);
    m_invalidMinSpin->setAlignment(Qt::AlignCenter);
    m_invalidMinSpin->setToolTip(tr("取值范围 0 ~ 100"));

    m_invalidMaxSpin = new QSpinBox;
    m_invalidMaxSpin->setRange(0, 100);
    m_invalidMaxSpin->setSuffix("s");
    m_invalidMaxSpin->setFixedWidth(130);
    m_invalidMaxSpin->setAlignment(Qt::AlignCenter);
    m_invalidMaxSpin->setToolTip(tr("取值范围 0 ~ 100"));

    // 设置控件样式（医疗蓝主题）
    QString spinStyle = R"(
        QDoubleSpinBox, QSpinBox {
            font: 10pt "Segoe UI";
            padding: 6px;
            border: 1px solid #b8d4e8;
            border-radius: 6px;
            background: #ffffff;
            color: #1a5276;
            selection-background-color: #85c1e9;
        }
        QDoubleSpinBox:focus, QSpinBox:focus {
            border-color: #5dade2;
            background: #f0f8ff;
        }
        QDoubleSpinBox::up-button, QSpinBox::up-button,
        QDoubleSpinBox::down-button, QSpinBox::down-button {
            background: #eaf2f8;
            border: none;
            width: 20px;
        }
        QDoubleSpinBox::up-button:hover, QSpinBox::up-button:hover,
        QDoubleSpinBox::down-button:hover, QSpinBox::down-button:hover {
            background: #d4e6f1;
        }
    )";

    m_offsetSpin->setStyleSheet(spinStyle);
    m_triggerInvalidSpin->setStyleSheet(spinStyle);
    m_offsetRatioSpin->setStyleSheet(spinStyle);
    m_invalidMinSpin->setStyleSheet(spinStyle);
    m_invalidMaxSpin->setStyleSheet(spinStyle);

    // =============================================
    // 使用栅格布局：4列 x 3行
    // 列0: 标签1 | 列1: 控件1 | 列2: 标签2 | 列3: 控件2
    // =============================================

    // 第1行: 偏移值 | 触发无效
    paramGrid->addWidget(offsetLabel, 0, 0);
    paramGrid->addWidget(m_offsetSpin, 0, 1);
    paramGrid->addWidget(triggerLabel, 0, 2);
    paramGrid->addWidget(m_triggerInvalidSpin, 0, 3);

    // 第2行: 偏移百分比 | 起始时间
    paramGrid->addWidget(offsetRatioLabel, 1, 0);
    paramGrid->addWidget(m_offsetRatioSpin, 1, 1);
    paramGrid->addWidget(minLabel, 1, 2);
    paramGrid->addWidget(m_invalidMinSpin, 1, 3);

    // 第3行: 空白占位 | 终止时间
    // 为了让布局更整齐，在第3行放一个空白标签和终止时间
    QLabel *emptyLabel = new QLabel("", this);
    emptyLabel->setStyleSheet("background: transparent;");
    paramGrid->addWidget(emptyLabel, 2, 0);
    paramGrid->addWidget(new QLabel("", this), 2, 1);  // 空白占位
    paramGrid->addWidget(maxLabel, 2, 2);
    paramGrid->addWidget(m_invalidMaxSpin, 2, 3);

    // 设置列拉伸比例，使布局均匀
    paramGrid->setColumnStretch(0, 1);
    paramGrid->setColumnStretch(1, 1);
    paramGrid->setColumnStretch(2, 1);
    paramGrid->setColumnStretch(3, 1);

    mainLayout->addWidget(paramGroup);

    // ----------------------------------------------
    // COL 专用延时区域（独立显示）
    // ----------------------------------------------
    m_delayGroup = new QGroupBox(tr("COL 延时设置"), this);
    m_delayGroup->setStyleSheet("QGroupBox { background: rgba(255,255,255,0.5); border: 1px solid #b8d4e8; border-radius: 10px; }"
                                "QGroupBox::title { color: #1a5276; background: transparent; }");

    QHBoxLayout *delayLayout = new QHBoxLayout(m_delayGroup);
    delayLayout->setSpacing(15);
    delayLayout->setContentsMargins(20, 15, 20, 15);

    m_delayLabel = new QLabel(tr("反应延时 :"), this);
    m_delayLabel->setStyleSheet("color: #1a5276; font: 10pt 'Segoe UI'; background: transparent;");
    m_delayLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_colDelaySpin = new QSpinBox(this);
    m_colDelaySpin->setRange(0, 300);
    m_colDelaySpin->setSuffix(tr(" 秒"));
    m_colDelaySpin->setFixedWidth(150);
    m_colDelaySpin->setAlignment(Qt::AlignCenter);
    m_colDelaySpin->setToolTip(tr("COL 试剂反应延时秒数"));
    m_colDelaySpin->setStyleSheet(spinStyle);

    QLabel *delayInfoLabel = new QLabel(tr("(仅 COL 试剂有效)"), this);
    delayInfoLabel->setStyleSheet("color: #5b7f9e; font: italic 9pt 'Segoe UI'; background: transparent;");

    delayLayout->addStretch();
    delayLayout->addWidget(m_delayLabel);
    delayLayout->addWidget(m_colDelaySpin);
    delayLayout->addWidget(delayInfoLabel);
    delayLayout->addStretch();

    mainLayout->addWidget(m_delayGroup);

    // ----------------------------------------------
    // 提示信息
    // ----------------------------------------------
    QLabel *infoLabel = new QLabel(tr("提示：切换试剂时自动保存当前试剂的参数"), this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("color: #5b7f9e; font: italic 9pt 'Segoe UI'; background: transparent;");
    mainLayout->addWidget(infoLabel);

    // ----------------------------------------------
    // 按钮区域
    // ----------------------------------------------
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(12);
    m_okBtn = new QPushButton(tr("确定"));
    m_cancelBtn = new QPushButton(tr("取消"));
    QPushButton *resetBtn = new QPushButton(tr("恢复默认"));

    m_okBtn->setDefault(true);
    m_okBtn->setFixedWidth(100);
    m_cancelBtn->setFixedWidth(100);
    resetBtn->setFixedWidth(100);

    // 按钮样式
    QString btnStyle = R"(
        QPushButton {
            font: bold 10pt "Segoe UI";
            padding: 8px 16px;
            border-radius: 6px;
            background: #ffffff;
            color: #1a5276;
            border: 1px solid #b8d4e8;
        }
        QPushButton:hover {
            background: #eaf2f8;
            border-color: #85c1e9;
        }
        QPushButton:pressed {
            background: #d4e6f1;
        }
        QPushButton:default {
            background: #2e86c1;
            color: white;
            border-color: #2e86c1;
        }
        QPushButton:default:hover {
            background: #2471a3;
        }
    )";

    m_okBtn->setStyleSheet(btnStyle);
    m_cancelBtn->setStyleSheet(btnStyle);
    resetBtn->setStyleSheet(btnStyle);

    btnLayout->addStretch();
    btnLayout->addWidget(resetBtn);
    btnLayout->addWidget(m_okBtn);
    btnLayout->addWidget(m_cancelBtn);

    mainLayout->addLayout(btnLayout);

    // 信号连接
    connect(m_okBtn, &QPushButton::clicked, this, &ParameterDialog::onAccepted);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(resetBtn, &QPushButton::clicked, this, &ParameterDialog::resetToDefault);
    connect(m_selectReagent, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &ParameterDialog::onReagentChanged);
}

void ParameterDialog::setupStyle()
{
    // 医疗蓝主题背景
    this->setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #d4e6f1,
                                        stop:0.5 #eaf2f8,
                                        stop:1 #d4e6f1);
            border-radius: 10px;
        }
        QGroupBox {
            font: bold 11pt "Segoe UI";
            border: 1px solid #b8d4e8;
            border-radius: 10px;
            margin-top: 15px;
            background: rgba(255,255,255,0.6);
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            top: -1px;
            left: 15px;
            padding: 0 12px;
            color: #1a5276;
            background: transparent;
        }
        QComboBox {
            font: 10pt "Segoe UI";
            padding: 6px 12px;
            border: 1px solid #b8d4e8;
            border-radius: 6px;
            background: #ffffff;
            color: #1a5276;
        }
        QComboBox:hover {
            border-color: #85c1e9;
        }
        QComboBox::drop-down {
            border: none;
            background: transparent;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #1a5276;
            margin-right: 8px;
        }
        QComboBox QAbstractItemView {
            border: 1px solid #b8d4e8;
            border-radius: 6px;
            background: #ffffff;
            color: #1a5276;
            selection-background-color: #85c1e9;
            selection-color: white;
        }
        QComboBox QAbstractItemView::item {
            padding: 6px 12px;
        }
        QComboBox QAbstractItemView::item:hover {
            background: #eaf2f8;
        }
        QLabel {
            background: transparent;
        }
    )");
}

void ParameterDialog::saveCurrentReagentData()
{
    if (!m_currentReagent.isEmpty() && m_reagentData.contains(m_currentReagent)) {
        ReagentData& data = m_reagentData[m_currentReagent];
        data.offset = m_offsetSpin->value();
        data.colDelay = m_colDelaySpin ? m_colDelaySpin->value() : 0;
        data.triggerInvalid = static_cast<quint8>(m_triggerInvalidSpin->value());
        data.offsetRatio = static_cast<quint8>(m_offsetRatioSpin->value());
        data.invalidMin = static_cast<quint8>(m_invalidMinSpin->value());
        data.invalidMax = static_cast<quint8>(m_invalidMaxSpin->value());
    }
}

void ParameterDialog::updateDelayGroupVisibility(const QString& reagent)
{
    if (!m_delayGroup) return;

    bool hasDelay = m_reagentData.contains(reagent) && m_reagentData[reagent].hasDelay;
    m_delayGroup->setVisible(hasDelay);
}

void ParameterDialog::onReagentChanged(const QString& reagent)
{
    // 保存当前数据
    saveCurrentReagentData();

    // 更新当前试剂
    m_currentReagent = reagent;

    // 控制延时区域显示
    updateDelayGroupVisibility(reagent);

    // 加载新试剂的数据
    loadReagentData(reagent);
}

void ParameterDialog::loadReagentData(const QString& reagent)
{
    if (!m_reagentData.contains(reagent)) return;

    const ReagentData& data = m_reagentData[reagent];
    m_offsetSpin->setValue(data.offset);
    if (m_colDelaySpin) {
        m_colDelaySpin->setValue(data.colDelay);
    }
    m_triggerInvalidSpin->setValue(data.triggerInvalid);
    m_offsetRatioSpin->setValue(data.offsetRatio);
    m_invalidMinSpin->setValue(data.invalidMin);
    m_invalidMaxSpin->setValue(data.invalidMax);
}

void ParameterDialog::onAccepted()
{
    saveCurrentReagentData();
    accept();
}

void ParameterDialog::resetToDefault()
{
    // 重置所有试剂的数据为0
    for (auto it = m_reagentData.begin(); it != m_reagentData.end(); ++it) {
        ReagentData& data = it.value();
        data.offset = 0.0;
        data.colDelay = 0;
        data.triggerInvalid = 0;
        data.offsetRatio = 0;
        data.invalidMin = 0;
        data.invalidMax = 0;
    }
    loadReagentData(m_currentReagent);
}

// ============ 取值和设置函数 ============
QString ParameterDialog::currentReagent() const
{
    return m_currentReagent;
}

void ParameterDialog::setCurrentReagent(const QString& reagent)
{
    if (m_reagentData.contains(reagent)) {
        m_selectReagent->setCurrentText(reagent);
    }
}

double ParameterDialog::offsetValue() const
{
    return m_offsetSpin->value();
}

void ParameterDialog::setOffsetValue(double val)
{
    m_offsetSpin->setValue(val);
}

int ParameterDialog::colDelayTime() const
{
    return m_colDelaySpin ? m_colDelaySpin->value() : 0;
}

void ParameterDialog::setColDelayTime(int val)
{
    if (m_colDelaySpin) {
        m_colDelaySpin->setValue(val);
    }
}

quint8 ParameterDialog::getTriggerInvalid() const
{
    return static_cast<quint8>(m_triggerInvalidSpin->value());
}

void ParameterDialog::setTriggerInvalid(quint8 percentage)
{
    m_triggerInvalidSpin->setValue(percentage);
}

quint8 ParameterDialog::getTriggerOffsetVal() const
{
    return static_cast<quint8>(m_offsetRatioSpin->value());
}

void ParameterDialog::setTriggerOffsetVal(quint8 val)
{
    m_offsetRatioSpin->setValue(val);
}

quint8 ParameterDialog::getInvalidMin() const
{
    return static_cast<quint8>(m_invalidMinSpin->value());
}

void ParameterDialog::setInvalidMin(quint8 minPerce)
{
    m_invalidMinSpin->setValue(minPerce);
}

quint8 ParameterDialog::getInvalidMax() const
{
    return static_cast<quint8>(m_invalidMaxSpin->value());
}

void ParameterDialog::setInvalidMax(quint8 maxPerce)
{
    m_invalidMaxSpin->setValue(maxPerce);
}

// ============ 批量数据接口 ============
void ParameterDialog::setReagentData(const QString& reagent, double offset, quint8 triggerInvalid,
                                     quint8 offsetRatio, quint8 invalidMin, quint8 invalidMax)
{
    if (!m_reagentData.contains(reagent)) return;
    ReagentData& data = m_reagentData[reagent];
    data.offset = offset;
    data.triggerInvalid = triggerInvalid;
    data.offsetRatio = offsetRatio;
    data.invalidMin = invalidMin;
    data.invalidMax = invalidMax;

    // 如果当前显示的是这个试剂，刷新界面
    if (m_currentReagent == reagent) {
        loadReagentData(reagent);
    }
}

void ParameterDialog::getReagentData(const QString& reagent, double& offset, quint8& triggerInvalid,
                                     quint8& offsetRatio, quint8& invalidMin, quint8& invalidMax) const
{
    if (!m_reagentData.contains(reagent)) return;
    const ReagentData& data = m_reagentData[reagent];
    offset = data.offset;
    triggerInvalid = data.triggerInvalid;
    offsetRatio = data.offsetRatio;
    invalidMin = data.invalidMin;
    invalidMax = data.invalidMax;
}

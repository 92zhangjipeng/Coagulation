#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include "globaldata.h"
#include "replacethetesttubetray.h"
#include "ui_replacethetesttubetray.h"
#include "warn_interface.h"
#include "QsLog/include/QsLog.h"
#include <QListWidget>
#include <QMenu>
#include <QStyle>
#include <QMessageBox>
#include <cglobal.h>
#include <QDateTime>
#include <QSet>
#include <operclass/fullyautomatedplatelets.h>
#include <custom_style/custommessagebox.h>


ReplaceTheTestTubeTray::ReplaceTheTestTubeTray(quint8 indexEquip,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReplaceTheTestTubeTray)
{
    ui->setupUi(this);
    this->setWindowTitle("指控");
    setWindowFlags(Qt::FramelessWindowHint);
    m_equipmentKind = indexEquip;


    QFile styleFileToolBtn(":/Picture/SetPng/wholeToolButton.qss");
    if(styleFileToolBtn.open(QIODevice::ReadOnly)) {
          QString toolBtnQss = QLatin1String(styleFileToolBtn.readAll());
          ui->toolButton->setStyleSheet(toolBtnQss);
          ui->toolButton->setText("开始");
          styleFileToolBtn.close();
    }

    QString settButtonQss;
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
      settButtonQss = QLatin1String(styleFile.readAll());
      styleFile.close();
    }

    QHash<QPushButton*, QString> pushButtonList = {
         {ui->pushButton_day,    tr("当天")},
         {ui->pushButton_month, tr("当月")},
         {ui->pushButton_year, tr("当年")},
         {ui->pushButton_all, tr("全部")}

    };

    for (auto it = pushButtonList.cbegin(); it != pushButtonList.cend(); ++it) {
         QPushButton* button = it.key();
         QString text = it.value();
         // 使用button和text进行操作
         button->setStyleSheet(settButtonQss);
         button->setText(text);
    }

    m_dayx.resize(MACHINE_SETTING_CHANNEL);
    m_dayx.clear();

    m_dayValy.resize(MACHINE_SETTING_CHANNEL);
    m_dayValy.clear();

    // 初始化界面后创建按钮
    createHorizontalButtons(m_equipmentKind);

    inittestingwidget(ui->tableWidget_test);
    inittestingwidget(ui->tableWidget_result);
    // 启用表格的上下文菜单策略
    ui->tableWidget_result->setContextMenuPolicy(Qt::CustomContextMenu);

        // 连接信号槽
    connect(ui->tableWidget_result, &QTableWidget::customContextMenuRequested,
            this, &ReplaceTheTestTubeTray::openPEdataWindow);


    setupAxes(ui->widget_PECurve);



	ui->dateEdit_resultCuve->blockSignals(true);  // 开始阻塞
    FullyAutomatedPlatelets::pinstanceInquiredata()->initDateEditUI(ui->dateEdit_resultCuve,
                                                                    ui->widgetbtn);
	ui->dateEdit_resultCuve->blockSignals(false);  // 结束阻塞

}

ReplaceTheTestTubeTray::~ReplaceTheTestTubeTray()
{
    delete ui;
}

// 工具函数
QTableWidgetItem* createCenteredItem(const QString &text) {
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

QString getCurrentTimestamp() {
    return QDateTime::currentDateTime().toString("yyyyMMddHHmm");
}
bool isKeyValid(const QString &key) {
    bool ok;
    key.toInt(&ok);
    return ok;
}
bool ReplaceTheTestTubeTray::isKeyExists(const QString &key) {
    for (int row = 0; row < ui->tableWidget_test->rowCount(); ++row) {
        if (ui->tableWidget_test->item(row, COL_CHANNEL)->text() == key) {
            return true;
        }
    }
    return false;
}

void ReplaceTheTestTubeTray::createHorizontalButtons(const int &numChannel)
{
    int channelCount = 12;
    switch (numChannel) {
        case KS600: channelCount = 4; break;
        case KS800: channelCount = 8; break;
        case KS1200: channelCount = 12; break;
        default: channelCount = 12; break;
    }

    // 确保groupBox存在且已初始化
    Q_ASSERT(ui->groupBox_testChannel != nullptr);

    // 创建水平布局并设置到groupBox
    QHBoxLayout *hLayout = new QHBoxLayout(ui->groupBox_testChannel);
    hLayout->setContentsMargins(20, 15, 20, 15); // 边距：左 上 右 下
    hLayout->setSpacing(15);                      // 按钮间距

    // 统一按钮样式表
    const QString buttonStyle = R"(
        QPushButton {
            background: #2196F3;
            border-radius: 30px;
            min-width: 60px;
            min-height: 60px;
            font: bold 14px;
            color: white;
            border: 2px solid #1976D2;
        }
        QPushButton:hover {
            background: #42A5F5;
            border-color: #2196F3;
        }
        QPushButton:pressed {
            background: #1976D2;
        }
        QPushButton:checked {
            background: #FF5722;
            border-color: #E64A19;
        }
    )";

    // 创建按钮组（可选）
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false); // 允许选择多个

    // 按机型生成通道按钮：KS600=4、KS800=8、KS1200=12
    for(int i = 0; i < channelCount; ++i) {
        QPushButton *btn = new QPushButton(QString::number(i+1));

        // 设置按钮属性
        btn->setCheckable(true);
        btn->setStyleSheet(buttonStyle);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        // 添加到布局和容器
        hLayout->addWidget(btn);
        m_channelButtons.append(btn);
        buttonGroup->addButton(btn, i); // 设置ID

        // 连接信号槽
        connect(btn, &QPushButton::clicked, [=](bool checked){
            QLOG_DEBUG() << "Channel" << i+1 << (checked ? "activated" : "deactivated");
            sycntestingChannel(QString("%1").arg(i+1),checked);
        });
    }

    ui->groupBox_testChannel->setTitle(QString("测试通道 1-%1").arg(channelCount));
    ui->groupBox_testChannel->setLayout(hLayout);

    // 添加伸缩因子保持按钮居中
    hLayout->insertStretch(0, 1);
    hLayout->addStretch(1);
}


// 扩展功能实现
void ReplaceTheTestTubeTray::resetAllChannels(bool resetAll, const QString& targetChannelText) {
	if (m_channelButtons.isEmpty()) return;
    const auto processButton = [](QPushButton* btn) {
        if (!btn) return; // 空指针保护
        btn->setEnabled(true);
        btn->setChecked(false);
    };

    if (resetAll) {
        for (auto* btn : qAsConst(m_channelButtons)) {
            QSignalBlocker blocker(btn);
            processButton(btn);
        }
    } else {
        // 精确匹配时增加容错处理
        bool found = false;
        for (auto* btn : qAsConst(m_channelButtons)) {
            if (btn && btn->text().trimmed().compare(targetChannelText, Qt::CaseInsensitive) == 0) {
                processButton(btn);
                found = true;
                break; // 如果设计上要求唯一性则保留 break
            }
        }
        if (!found) {
            QLOG_WARN() << "Channel not found:" << targetChannelText;
        }
    }
}




void ReplaceTheTestTubeTray::disableAllButtons() {
	if (m_channelButtons.isEmpty()) return;
	for (QPushButton* btn : m_channelButtons) {
       btn->setEnabled(false);  // 禁用按钮
   }
}


QVector<int> ReplaceTheTestTubeTray::getActiveChannels() const
{
    QVector<int> activeChannels;
    for(int i = 0; i < m_channelButtons.size(); ++i) {
        if(m_channelButtons[i]->isChecked()) {
            activeChannels.append(i+1);
        }
    }
    return activeChannels;
}


void ReplaceTheTestTubeTray::inittestingwidget(QTableWidget *ptable)
{

    QStringList headers{"添加时间","检测通道","高值","中值","低值"};
    ptable->setColumnCount(headers.size());
    ptable->setHorizontalHeaderLabels(headers);


    // 初始行数为0（动态添加）
    ptable->setRowCount(0);


    // 行为设置
    ptable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ptable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ptable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ptable->horizontalHeader()->setSectionResizeMode(COL_ID, QHeaderView::Fixed);
    ptable->setColumnWidth(COL_ID, 180);

    ptable->horizontalHeader()->setSectionResizeMode(COL_CHANNEL, QHeaderView::Fixed);
    ptable->setColumnWidth(COL_CHANNEL, 80);

    ptable->horizontalHeader()->setSectionResizeMode(COL_VALUE, QHeaderView::Stretch);
    ptable->horizontalHeader()->setSectionResizeMode(COL_VALUEMID, QHeaderView::Stretch);
    ptable->horizontalHeader()->setSectionResizeMode(COL_VALUELOW, QHeaderView::Stretch);



    // 设置全局抗锯齿
    QFont font = ptable->font();
    font.setStyleStrategy(QFont::PreferAntialias);
    ptable->setFont(font);

    // 启用平滑滚动
    ptable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ptable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    // 样式表
    ptable->setStyleSheet(
        // 全局表格样式
        "QTableWidget {"
        "   background: #FFFFFF;"          // 白色背景
        "   border: 1px solid #D0D0D0;"    // 浅灰色边框
        "   gridline-color: #E0E0E0;"      // 浅网格线
        "   font-family: '微软雅黑';"       // 更清晰的字体
        "   font-size: 13px;"              // 基础字号增大
        "   color: #333333;"               // 深灰色文字提升可读性
        "}"

        // 表头样式
        "QHeaderView::section {"
        "   background: #F8F9FA;"          // 浅灰背景
        "   padding: 6px 8px;"             // 增加内边距
        "   border: 1px solid #D0D0D0;"
        "   border-bottom: 2px solid #409EFF;" // 蓝色下划线
        "   font-size:  14px;"               // 表头字号更大
        "   font-weight: 500;"              // 中等字重
        "   color: #606266;"                // 深灰色文字
        "}"

        "QTableWidget::item:focus {"
        "   border: none;"         // 移除聚焦时的虚线框
        "   outline: none;"        // 移除某些系统下的额外轮廓
        "}"

        "QTableWidget::item:selected:focus {"
        "   background: #409EFF33;" // 保持选中背景色
        "}"
        // 选中项样式
        "QTableWidget::item:selected {"
        "   background-color: #409EFF33;"   // 品牌蓝透明背景（20%透明度）
        "   color: #303133;"                // 深黑色文字
        "   font-weight: 500;"              // 选中加粗
        "   font-size: 14px;"               // 选中时字号增大
        "}"

        // 鼠标悬停样式
        "QTableWidget::item:hover {"
        "   background-color: #F5F7FA;"    // 浅灰悬停背景
        "}"
        // 行高设置
        "QTableWidget::item {"
        "   min-height: 36px;"         // 最小行高36px
        "   padding: 6px 12px;"        // 单元格内边距
        "}"
        // 表头样式增强
       "QHeaderView::section {"
       "   background: #f8f9fa;"      // 浅灰背景
       "   min-height: 32px;"         // 表头高度增加32px（原建议28px）
       "   padding: 8px 12px;"        // 增加内边距
       "   border: none;"
       "   border-bottom: 2px solid #409eff;" // 品牌蓝下划线
       "   font-weight: 600;"         // 中等加粗（比bold稍轻）
       "   font-size: 14px;"          // 表头字号
       "   color: #303133;"           // 深灰文字
       "   qproperty-alignment: AlignCenter;"  // 文字居中
       "}"

        // 交替行颜色（可选）
        "QTableWidget::item:alternate {"
        "   background-color: #FAFAFA;"
        "}"
    );
}

void ReplaceTheTestTubeTray::sycntestingChannel(const QString &channle,bool addit){

    QList<QString> hadChannel;
    hadChannel.reserve(ui->tableWidget_test->rowCount());
    for(int i = 0; i < ui->tableWidget_test->rowCount(); ++i) {
        QTableWidgetItem *existingKeyItem = ui->tableWidget_test->item(i, COL_CHANNEL);
        if (existingKeyItem) {
           hadChannel.append(existingKeyItem->text());
        }
    }
    bool ishand = hadChannel.contains(channle);
    if(addit && !ishand){
        addNewRow(channle,"待测");
    }else if(addit && ishand){
        CustomMessageBox::warning(nullptr,"添加失败","已存在性能测试通道!");
        for (QPushButton* btn : m_channelButtons) {
            if (btn && btn->text().trimmed() == channle) {
                QSignalBlocker blocker(btn);
                btn->setChecked(false);
                break;
            }
        }
        return;
    }else if(!addit && ishand){
        removeDuplicateKeys(channle);
    }

}
void ReplaceTheTestTubeTray::removeDuplicateKeys(const QString &channle) {
    const int KEY_COLUMN = 1;  // 假设键在列0

    // 从最后一行向前遍历（避免删除导致的索引错位）
    for(int row = ui->tableWidget_test->rowCount() - 1; row >= 0; --row) {
        QTableWidgetItem *keyItem = ui->tableWidget_test->item(row, KEY_COLUMN);

        // 跳过空指针或空内容
        if (!keyItem || keyItem->text().isEmpty()) {
            continue;
        }

        const QString currentKey = keyItem->text();

        // 如果键已存在则删除整行
        if (channle == currentKey) {
            ui->tableWidget_test->removeRow(row);
            return;
        }
    }
}

// 动态添加行
void ReplaceTheTestTubeTray::addNewRow(const QString &key, const QString &value) {

    if (!isKeyValid(key)) {
        QLOG_DEBUG() << "Invalid key:" << key;
        return;
    }

    if (isKeyExists(key)) {
        QLOG_DEBUG() << "Key already exists:" << key;
        return;
    }

    QTableWidget *table = ui->tableWidget_test;
    table->setUpdatesEnabled(false); // 开始批量模式

    const int newRow = table->rowCount();
    table->insertRow(newRow);

       // 时间列
    const QString timestamp = getCurrentTimestamp();
    const QString formattedKey = QString("%1").arg(key.toInt(), 2, 10, QLatin1Char('0'));
    table->setItem(newRow, COL_ID, createCenteredItem(timestamp + formattedKey));

    // 键值列
    table->setItem(newRow, COL_CHANNEL,  createCenteredItem(key));
    table->setItem(newRow, COL_VALUE,    createCenteredItem(value));
    table->setItem(newRow, COL_VALUEMID, createCenteredItem(value));
    table->setItem(newRow, COL_VALUELOW, createCenteredItem(value));

    table->setUpdatesEnabled(true); // 结束批量模式
}

QMap<QString, quint8> ReplaceTheTestTubeTray::getColumnValues(QTableWidget *table, bool *insertOk) {
    QMap<QString, quint8> values;
    if (insertOk) {
        *insertOk = true;
    }
    if(!table) return values;

    for (int row = 0; row < table->rowCount(); ++row) {
       // 使用 const 引用避免拷贝
       const auto* keyItem = table->item(row, COL_ID);
       const auto* valItem = table->item(row, COL_CHANNEL);

       // 跳过无效行 (键或值为空)
       if (!keyItem || !valItem) {
           qWarning() << "Invalid row:" << row << "missing items";
           continue;
       }

       // 显式校验键值有效性
       const QString& skey = keyItem->text().trimmed();
       if (skey.isEmpty()) {
           QLOG_WARN() << "Empty key at row:" << row;
           continue;
       }

       // 安全数值转换
       bool ok = false;
       const quint8 ival = valItem->text().toUShort(&ok);
       if (!ok || ival > 0xFF) {
           QLOG_WARN() << "Invalid value at row:" << row << valItem->text();
           continue;
       }

       // 避免重复键覆盖
       if (values.contains(skey)) {
           qWarning() << "Duplicate key:" << skey << "at row:" << row;
           continue;
       }

       values.insert(skey, ival);
       if (!FullyAutomatedPlatelets::pinstancesqlData()->insertPETestEntry(
                   skey,ival,"waiting","waiting","waiting")) {
           QLOG_ERROR() << "PE写入完成记录失败" << skey;
           if (insertOk) {
               *insertOk = false;
           }
       }
    }
    return values;
}

//启动性能
void ReplaceTheTestTubeTray::on_toolButton_clicked()
{
    struct CheckCondition {
        bool condition;
        std::pair<QString, QString> message;
    };

    QTableWidget *table = ui->tableWidget_test;
    bool tablEmpty = (table->rowCount()<= 0)? true: false;

    const QVector<CheckCondition> conditions = {
        {
            !cglobal::gserialConnecStatus.load(),
            { tr("性能测试启动失败"), tr("仪器离线!") }
        },
        {
            cglobal::g_StartTesting.load(),
            { tr("性能测试启动失败"), tr("仪器常规测试中!") }
        },
        {
            Performanceverification::instance()->returnPerformanceTestFlag(),
            { tr("性能测试启动失败"), tr("仪器PE测试中,请等待测试PE完成!") }
        },
        {
            tablEmpty,
            {tr("性能测试启动失败"),tr("请先添加测试通道!")}
        }
    };

    for (const auto& check : conditions) {
       if (check.condition) {
           CustomMessageBox::warning(this,check.message.first,check.message.second);
           return;
       }
    }

    const int needTubes = table->rowCount() * 5;
    const int freeTubes = FullyAutomatedPlatelets::pinstancesqlData()
            ? FullyAutomatedPlatelets::pinstancesqlData()->BackEmptyTubeNum()
            : 0;
    if (freeTubes < needTubes) {
        CustomMessageBox::warning(this,
                                  tr("性能测试启动失败"),
                                  tr("空试管不足，当前空闲%1个，本次需要%2个（每个通道5个：水/标准物/高/中/低）")
                                  .arg(freeTubes)
                                  .arg(needTubes));
        return;
    }

    bool insertOk = true;
    QMap<QString,quint8> testChannel = getColumnValues(ui->tableWidget_test, &insertOk);
    if (!insertOk) {
        CustomMessageBox::warning(this, tr("性能测试启动失败"), tr("完成记录写入数据库失败，请检查 SWData.db"));
        return;
    }
    if (testChannel.isEmpty()) {
        CustomMessageBox::warning(this, tr("性能测试启动失败"), tr("没有有效测试通道!"));
        return;
    }

    disableAllButtons();

    //性能验证
    Performanceverification::instance()->gettestChannelList(testChannel);

}

void ReplaceTheTestTubeTray::fillData(const QString& id, quint8 channel, const std::array<double, 3>& ratios){
    QTableWidget* table = ui->tableWidget_test;
    if (!table) {
       qCritical() << "Table widget is null!";
       return;
    }

    // 1. 使用RAII管理表格更新状态
    TableUpdateGuard guard(table);

    // 2. 优化表格查找：使用哈希表加速查找
    static QHash<QString, int> rowCache;
    const int row = findRowForId(table, id, rowCache);

    if (row == -1) {
        QLOG_WARN() << "ID not found in table:" << id;
        return;
    }

    // 3. 批量更新表格项
    updateTableRow(table, row, channel, ratios);

    // 4. 更新数据库
    updateDatabase(id, channel, ratios);
}

// 辅助函数：使用哈希表加速行查找
int ReplaceTheTestTubeTray::findRowForId(QTableWidget* table, const QString& id, QHash<QString, int>& cache)
{
    if (cache.contains(id)) {
        const int row = cache[id];
        if (row < table->rowCount() && table->item(row, COL_ID)->text() == id) {
            return row;
        }
        cache.remove(id); // 缓存失效
    }

    const int totalRows = table->rowCount();
    for (int row = 0; row < totalRows; ++row) {
        if (auto* keyItem = table->item(row, COL_ID)) {
            const QString rowId = keyItem->text().trimmed();
            if (rowId.isEmpty()) {
                QLOG_WARN() << "Skipping empty/invalid key at row:" << row;
                continue;
            }

            if (rowId == id) {
                cache[id] = row; // 更新缓存
                return row;
            }
        }
    }
    return -1;
}

// 辅助函数：批量更新表格行
void ReplaceTheTestTubeTray::updateTableRow(QTableWidget* table, int row, quint8 channel, const std::array<double, 3>& ratios)
{
    // 通道号转换 (+1)
    table->setItem(row, COL_CHANNEL, createCenteredItem(QString::number(channel + 1)));

    // 批量设置三个比值项
    static constexpr std::array<int, 3> ratioColumns = {COL_VALUE, COL_VALUEMID, COL_VALUELOW};

    for (int i = 0; i < ratios.size(); ++i) {
        const QString formattedValue = QString::number(ratios[i], 'f', 2) + "%";
        table->setItem(row, ratioColumns[i], createCenteredItem(formattedValue));
    }
}

// 辅助函数：更新数据库
void ReplaceTheTestTubeTray::updateDatabase(const QString& id, quint8 channel, const std::array<double, 3>& ratios)
{
    // 准备格式化值
    const QString hValue = QString::number(ratios[0], 'f', 2) + "%";
    const QString mValue = QString::number(ratios[1], 'f', 2) + "%";
    const QString lValue = QString::number(ratios[2], 'f', 2) + "%";

    // 批量更新数据库
    FullyAutomatedPlatelets::pinstancesqlData()->updatePETestEntry(
        id, channel + 1, hValue, mValue, lValue);

    QLOG_DEBUG() << "Updated ID:" << id
                 << "| Channel:" << channel + 1
                 << "| H:" << hValue
                 << "| M:" << mValue
                 << "| L:" << lValue;
}


void ReplaceTheTestTubeTray::handlecompletedPETest(){
    resetAllChannels(true,"");
    ui->tableWidget_test->clearContents();
    ui->tableWidget_test->setRowCount(0);
    // 完成记录从数据库重载全部历史，避免只留下本轮、点「全部」又查空表
    on_pushButton_all_clicked();
}

// 转移数据
void ReplaceTheTestTubeTray::allfinishshowResult(){
    ui->tableWidget_result->setRowCount(0);  // 直接清空所有行
    int srcRow = ui->tableWidget_test->rowCount();
    int srcCol = ui->tableWidget_test->columnCount();
    for (int row = 0; row < srcRow; ++row) {
        ui->tableWidget_result->insertRow(ui->tableWidget_result->rowCount());
        for (int col = 0; col < srcCol; ++col) {
            QTableWidgetItem* srcItem = ui->tableWidget_test->item(row, col);
            if (srcItem) {
                QTableWidgetItem* newItem = new QTableWidgetItem(srcItem->text());
                newItem->setFlags(srcItem->flags());
                newItem->setTextAlignment(srcItem->textAlignment());
                ui->tableWidget_result->setItem(ui->tableWidget_result->rowCount()-1, col, newItem);
            }
        }
    }
    ui->tableWidget_test->clearContents();
    ui->tableWidget_test->setRowCount(0);
}

void ReplaceTheTestTubeTray::on_pushButton_day_clicked()
{
     ui->tableWidget_result->setRowCount(0);
     const QString trage = QDateTime::currentDateTime().toString("yyyyMMdd");
	 auto todayResult = FullyAutomatedPlatelets::pinstancesqlData()->selectPETestToday(trage);
     for (const auto& entry : todayResult) {
         addResultNewRow(entry.PEid,QString::number(entry.TestCannel),entry.TestRatioH,
                         entry.TestRatioM,entry.TestRatioL);
     }
}

void ReplaceTheTestTubeTray::on_pushButton_month_clicked()
{
     ui->tableWidget_result->setRowCount(0);
     const QString trage = QDateTime::currentDateTime().toString("yyyyMM");
	 auto monthResult = FullyAutomatedPlatelets::pinstancesqlData()->selectPETestThisMonth(trage);
     for (const auto& entry : monthResult) {
         addResultNewRow(entry.PEid,QString::number(entry.TestCannel),entry.TestRatioH,
                         entry.TestRatioM,entry.TestRatioL);
     }
}

void ReplaceTheTestTubeTray::on_pushButton_year_clicked()
{
     ui->tableWidget_result->setRowCount(0);
     const QString trage = QDateTime::currentDateTime().toString("yyyy");
	 auto yearResult = FullyAutomatedPlatelets::pinstancesqlData()->selectPETestThisYear(trage);
     for (const auto& entry : yearResult) {
         addResultNewRow(entry.PEid,QString::number(entry.TestCannel),entry.TestRatioH,
                         entry.TestRatioM,entry.TestRatioL);
     }
}

void ReplaceTheTestTubeTray::on_pushButton_all_clicked()
{
     ui->tableWidget_result->setRowCount(0);
     // 获取所有记录
     auto allEntries = FullyAutomatedPlatelets::pinstancesqlData()->selectAllPETestEntries();
     // 遍历结果
     for (const auto& entry : allEntries) {
         addResultNewRow(entry.PEid,QString::number(entry.TestCannel),entry.TestRatioH,
                         entry.TestRatioM,entry.TestRatioL);
     }

}

void ReplaceTheTestTubeTray::addResultNewRow(const QString &id,const QString &channel,
                                               const QString &outratio,
                                               const QString &outratioM,
                                               const QString &outratioL){
    QTableWidget *table = ui->tableWidget_result;
    table->setUpdatesEnabled(false); // 开始批量模式
    const int newRow = table->rowCount();
    table->insertRow(newRow);
    table->setItem(newRow, COL_ID,      createCenteredItem(id));
    table->setItem(newRow, COL_CHANNEL, createCenteredItem(channel));
    table->setItem(newRow, COL_VALUE,   createCenteredItem(outratio));
    table->setItem(newRow, COL_VALUEMID,   createCenteredItem(outratioM));
    table->setItem(newRow, COL_VALUELOW,   createCenteredItem(outratioL));
    table->setUpdatesEnabled(true); // 结束批量模式
}


void ReplaceTheTestTubeTray::setupAxes(QCustomPlot* customPlot) {
    /***纵轴配置***/
   customPlot->yAxis->setRange(-20, 100);
   customPlot->yAxis->setLabel("百分比 (%)");
   customPlot->yAxis->grid()->setPen(QPen(QColor(180,180,180), 1, Qt::DashLine));

   /***横轴时间配置***/
   /*QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
   fixedTicker->setTickStep(1.0); // 强制每天一个刻度
   fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone); // 禁用自动缩放
   fixedTicker->setTickCount(QDate::currentDate().daysInMonth()+1);
   customPlot->xAxis->setTicker(fixedTicker);
   QString monthLabel = QDate::currentDate().toString("时间:yyyy-MM");
   customPlot->xAxis->setLabel(monthLabel);
   customPlot->xAxis->setRange(1, QDate::currentDate().daysInMonth());*/


   //-- 横轴网格样式 --
   QPen xGridPen(QColor(200, 200, 200), 1, Qt::DotLine);
   xGridPen.setCosmetic(true);
   customPlot->xAxis->grid()->setPen(xGridPen);
   customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
   // 交互功能
   //customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
   QDate currentMonth = QDate::currentDate();
   viewPEResult(true,"",currentMonth);
}



void ReplaceTheTestTubeTray::generateMultiDemoData(
    QCustomPlot* customPlot,
    const QVector<double>& x,
    const QVector<QVector<double>>& yValues, // 多条曲线的y值，例如{y_high, y_mid, y_low}
    const QDate& month,
    const QVector<QPen>& pens // 每条曲线的样式
) {
    // 设置坐标轴（共享设置，只执行一次）
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(1.0);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    fixedTicker->setTickCount(month.daysInMonth() + 1);
    customPlot->xAxis->setTicker(fixedTicker);
    QString monthLabel = month.toString("时间:yyyy-MM");
    customPlot->xAxis->setLabel(monthLabel);
    customPlot->xAxis->setRange(1, month.daysInMonth());

    // 清除旧图形（一次性操作）
    customPlot->clearGraphs();
	customPlot->replot();
    // 添加并配置多条曲线
    for (int i = 0; i < yValues.size(); ++i) {
        customPlot->addGraph(); // 添加新图形
        customPlot->graph(i)->setData(x, yValues[i]); // 设置数据

        // 设置曲线样式：使用传入的QPen，或默认样式
        QPen graphPen = (i < pens.size()) ? pens[i] : QPen(QColor(255, 110, 40), 2);
        QColor pointColor = (i < pens.size()) ? pens[i].color() : QColor(255, 110, 40);

        graphPen.setStyle(Qt::SolidLine);
        customPlot->graph(i)->setPen(graphPen);
        customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, pointColor, Qt::white, 8));
    }

    // 单次重绘（性能关键）
    customPlot->replot(QCustomPlot::rpImmediateRefresh);
}

void ReplaceTheTestTubeTray::openPEdataWindow(const QPoint &pos)
{
   QTableWidget *table = ui->tableWidget_result;
   if (table) {
       const QModelIndex idx = table->indexAt(pos);
       if (idx.isValid() && table->selectionModel()
               && !table->selectionModel()->isRowSelected(idx.row(), QModelIndex())) {
           table->selectRow(idx.row());
       }
   }

   QMap<int,QVector<double> > vecdata;
   vecdata.clear();
   SelectedRows(vecdata);

   if (vecdata.value(HEIGHVALUE).isEmpty()
           && vecdata.value(MIDVALUE).isEmpty()
           && vecdata.value(LOWVALUE).isEmpty()) {
       return;
   }

    // 使用 QPointer 跟踪窗口状态（防野指针）
    static QPointer<PEdata> peWindow;

    if (!peWindow) {
        // 创建窗口并设置自动析构属性
        peWindow = new PEdata(nullptr); // 无父对象的顶级窗口
        connect(peWindow,&PEdata::makesureResult,this,
                &ReplaceTheTestTubeTray::handlemakesureResult);
        peWindow->setAttribute(Qt::WA_DeleteOnClose); // 核心设置

        peWindow->setWindowTitle("PE Data Analyzer");
        peWindow->resize(570, 180);
    }

    peWindow->insertdata(vecdata);
    peWindow->show();
    peWindow->activateWindow();
}

void ReplaceTheTestTubeTray::SelectedRows(QMap<int,QVector<double> >& outdata) {
    QTableWidget *table = ui->tableWidget_result;
    QVector<double> outputdata;
    QVector<double> outputdataM;
    QVector<double> outputdataL;

    QSet<int> selectedRowSet;
    QItemSelectionModel *selectionModel = table->selectionModel();
    if (selectionModel) {
        const QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
        for (const QModelIndex &index : selectedIndexes) {
            selectedRowSet.insert(index.row());
        }
        const QModelIndexList selectedRows = selectionModel->selectedRows();
        for (const QModelIndex &index : selectedRows) {
            selectedRowSet.insert(index.row());
        }
    }

    QList<int> rowList = selectedRowSet.values();
    std::sort(rowList.begin(), rowList.end());

    outputdata.reserve(rowList.size());
    outputdataM.reserve(rowList.size());
    outputdataL.reserve(rowList.size());

    auto convertItemTextToDouble = [](QTableWidgetItem *item, bool *ok) -> double {
        if (ok) {
            *ok = false;
        }
        if (!item || item->text().isEmpty()) {
            return 0.0;
        }
        QString cleanText = item->text().replace("%", "").trimmed();
        bool convertOK = false;
        const double convertedValue = cleanText.toDouble(&convertOK);
        if (ok) {
            *ok = convertOK;
        }
        return convertOK ? convertedValue : 0.0;
    };

    for (int row : rowList) {
        bool okH = false;
        bool okM = false;
        bool okL = false;
        const double hVal = convertItemTextToDouble(table->item(row, COL_VALUE), &okH);
        const double mVal = convertItemTextToDouble(table->item(row, COL_VALUEMID), &okM);
        const double lVal = convertItemTextToDouble(table->item(row, COL_VALUELOW), &okL);
        if (!okH || !okM || !okL) {
            continue;
        }

        outputdata.append(hVal);
        outputdataM.append(mVal);
        outputdataL.append(lVal);
    }
    outdata.insert(HEIGHVALUE,outputdata);
    outdata.insert(MIDVALUE,outputdataM);
    outdata.insert(LOWVALUE,outputdataL);
    return;
}

void ReplaceTheTestTubeTray::handlemakesureResult(const QString &resultdata,const QString &resultdataMid,
                                                   const QString &resultdataLow){
    QString keydate = QDate::currentDate().toString("yyyyMMdd");
    if(FullyAutomatedPlatelets::pinstancesqlData()->insertPEDayResult(keydate, resultdata,resultdataMid,resultdataLow)){
       QDate currentMonth = QDate::currentDate();
       viewPEResult(true,"",currentMonth);
    }else{
       QLOG_WARN()<<"设置PE"<<keydate<<"失败";
    }
}

void ReplaceTheTestTubeTray::viewPEResult(const bool isnow,const QString tratDate, const QDate &tragmonth){
    m_dayx.clear();
    m_dayValy.clear();
    m_dayMidValy.clear();
    m_dayLowValy.clear();
    QString keydate;
    keydate = isnow? QDate::currentDate().toString("yyyyMM"): tratDate;
    FullyAutomatedPlatelets::pinstancesqlData()->selectPEResultThisMonth(keydate,m_dayx,m_dayValy,m_dayMidValy,m_dayLowValy);

    // 定义样式：高值（橙色）、中值（蓝色）、低值（绿色）
    QVector<QPen> curvePens;
    curvePens << QPen(QColor(255, 110, 40), 2) // 高值
              << QPen(QColor(0, 0, 255), 2)    // 中值
              << QPen(QColor(0, 255, 0), 2);  // 低值

    // 多条曲线的y值数据
    QVector<QVector<double>> yValues;
    yValues << m_dayValy << m_dayMidValy << m_dayLowValy;

    // 单次调用批量函数
    generateMultiDemoData(ui->widget_PECurve, m_dayx, yValues, tragmonth, curvePens);

    //generateDemoData(ui->widget_PECurve, m_dayx, m_dayValy,tragmonth);
}


void ReplaceTheTestTubeTray::on_dateEdit_resultCuve_dateChanged(const QDate &date)
{
	if (!date.isValid()) return;

	QString dateStr = date.toString("yyyyMM");
    viewPEResult(false, dateStr,date);
}

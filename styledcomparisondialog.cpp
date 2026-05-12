#pragma execution_character_set("utf-8")
#include "StyledComparisonDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextEdit>
#include <QProgressBar>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QCursor>
#include <QScreen>
#include <QGuiApplication>

StyledComparisonDialog::StyledComparisonDialog(QWidget *parent)
    : QDialog(parent)
    , m_tolerance(1)
    , m_totalItems(0)
    , m_passedItems(0)
{
    setupUI();
    setWindowTitle("坐标对比报告");
    setMinimumSize(900, 700);

    // 设置窗口样式
    setStyleSheet(R"(
        QDialog {
            background-color: #f5f5f5;
        }
        QTabWidget::pane {
            border: 1px solid #ddd;
            background-color: white;
            border-radius: 4px;
        }
        QTabBar::tab {
            background-color: #e0e0e0;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background-color: #4CAF50;
            color: white;
        }
        QTreeWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
            alternate-background-color: #f9f9f9;
        }
        QTreeWidget::item {
            padding: 4px;
        }
        QTreeWidget::item:hover {
            background-color: #e3f2fd;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton#calibrateBtn {
            background-color: #ff9800;
        }
        QPushButton#calibrateBtn:hover {
            background-color: #fb8c00;
        }
        QPushButton#exportBtn {
            background-color: #2196F3;
        }
        QPushButton#exportBtn:hover {
            background-color: #1976D2;
        }
        QPushButton#closeBtn {
            background-color: #9e9e9e;
        }
        QPushButton#closeBtn:hover {
            background-color: #757575;
        }
        QProgressBar {
            border: 1px solid #ddd;
            border-radius: 4px;
            text-align: center;
            height: 25px;
        }
        QProgressBar::chunk {
            background-color: #4CAF50;
            border-radius: 3px;
        }
        QLabel#statusLabel {
            font-size: 14px;
            font-weight: bold;
            padding: 8px;
            border-radius: 4px;
        }
    )");
}

void StyledComparisonDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    // 只在第一次显示时居中
    static bool firstShow = true;
    if (firstShow) {
        firstShow = false;
        adjustSize();
        QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
        if (!screen) screen = QGuiApplication::primaryScreen();
        move(screen->geometry().center() - rect().center());
    }
}
void StyledComparisonDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 标题区域
    QLabel* titleLabel = new QLabel("坐标校验报告", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 进度和状态区域
    QHBoxLayout* statusLayout = new QHBoxLayout();

    m_overallProgress = new QProgressBar(this);
    m_overallProgress->setRange(0, 100);
    m_overallProgress->setValue(0);
    statusLayout->addWidget(m_overallProgress);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName("statusLabel");
    statusLayout->addWidget(m_statusLabel);

    mainLayout->addLayout(statusLayout);

    // 过滤选项
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("显示:", this));
    QComboBox* filterCombo = new QComboBox(this);
    filterCombo->addItems({"全部", "仅显示差异", "仅显示通过"});
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StyledComparisonDialog::onFilterChanged);
    filterLayout->addWidget(filterCombo);
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);

    // Tab 控件
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    // 创建各个标签页
    createBasicTab();
    createChannelsTab();
    createTrayTab();
    createSummaryTab();

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton* exportBtn = new QPushButton("导出报告", this);
    exportBtn->setObjectName("exportBtn");
    connect(exportBtn, &QPushButton::clicked, this, &StyledComparisonDialog::onExportClicked);
    buttonLayout->addWidget(exportBtn);

    QPushButton* calibrateBtn = new QPushButton("执行校准", this);
    calibrateBtn->setObjectName("calibrateBtn");
    connect(calibrateBtn, &QPushButton::clicked, this, &StyledComparisonDialog::onCalibrateClicked);
    buttonLayout->addWidget(calibrateBtn);

    QPushButton* closeBtn = new QPushButton("关闭", this);
    closeBtn->setObjectName("closeBtn");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);

    mainLayout->addLayout(buttonLayout);
}

void StyledComparisonDialog::createBasicTab()
{
    QWidget* basicWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(basicWidget);

    m_basicTree = new QTreeWidget(this);
    m_basicTree->setAlternatingRowColors(true);
    m_basicTree->setHeaderLabels({"坐标项", "校验坐标", "实际坐标", "偏差", "状态", "公差"});
    m_basicTree->header()->setStretchLastSection(false);
    m_basicTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    for(int i = 1; i <= 3; i++) {
        m_basicTree->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }

    layout->addWidget(m_basicTree);
    m_tabWidget->addTab(basicWidget, "基础坐标");
}

void StyledComparisonDialog::createChannelsTab()
{
    QWidget* channelsWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(channelsWidget);

    m_channelsTree = new QTreeWidget(this);
    m_channelsTree->setAlternatingRowColors(true);
    m_channelsTree->setHeaderLabels({"通道", "类型", "校验坐标", "实际坐标", "偏差", "状态"});
    m_channelsTree->header()->setStretchLastSection(false);
    m_channelsTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_channelsTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_channelsTree->header()->setSectionResizeMode(2, QHeaderView::Stretch);

    layout->addWidget(m_channelsTree);
    m_tabWidget->addTab(channelsWidget, "通道坐标");
}

void StyledComparisonDialog::createTrayTab()
{
    QWidget* trayWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(trayWidget);

    m_trayTree = new QTreeWidget(this);
    m_trayTree->setAlternatingRowColors(true);
    m_trayTree->setHeaderLabels({"托盘", "类型", "校验坐标", "实际坐标", "偏差", "状态"});
    m_trayTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_trayTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_trayTree->header()->setSectionResizeMode(2, QHeaderView::Stretch);

    layout->addWidget(m_trayTree);
    m_tabWidget->addTab(trayWidget, "托盘坐标");
}

void StyledComparisonDialog::createSummaryTab()
{
    QWidget* summaryWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(summaryWidget);

    m_summaryText = new QTextEdit(this);
    m_summaryText->setReadOnly(true);
    m_summaryText->setStyleSheet("font-family: monospace; font-size: 11px;");

    layout->addWidget(m_summaryText);
    m_tabWidget->addTab(summaryWidget, "汇总报告");
}

void StyledComparisonDialog::setComparisonData(
    const QMap<QString, QPair<QPoint, QPoint>>& basicPoints,
    const QMap<int, QPair<QPoint, QPoint>>& handsChannels,
    const QMap<int, QPair<QPoint, QPoint>>& reagentChannels,
    const QMap<int, QPair<QPoint, QPoint>>& trayHands,
    const QMap<int, QPair<QPoint, QPoint>>& trayBlood)
{
    m_basicPoints = basicPoints;
    m_handsChannels = handsChannels;
    m_reagentChannels = reagentChannels;
    m_trayHands = trayHands;
    m_trayBlood = trayBlood;

    // 计算总数
    m_totalItems = basicPoints.size() + handsChannels.size() +
                   reagentChannels.size() + trayHands.size() + trayBlood.size();

    populateBasicTable();
    populateChannelsTable();
    populateTrayTable();
    updateSummary();
}

void StyledComparisonDialog::populateBasicTable()
{
    m_basicTree->clear();

    for(auto it = m_basicPoints.begin(); it != m_basicPoints.end(); ++it) {
        const QString& name = it.key();
        const QPoint& expected = it.value().first;
        const QPoint& actual = it.value().second;

        int dx = actual.x() - expected.x();
        int dy = actual.y() - expected.y();
        bool passed = (abs(dx) <= m_tolerance && abs(dy) <= m_tolerance);

        if(passed) m_passedItems++;

        QTreeWidgetItem* item = new QTreeWidgetItem(m_basicTree);
        item->setText(0, name);
        item->setText(1, QString("(%1, %2)").arg(expected.x()).arg(expected.y()));
        item->setText(2, QString("(%1, %2)").arg(actual.x()).arg(actual.y()));
        item->setText(3, QString("(%1, %2)").arg(dx).arg(dy));
        item->setText(4, getStatusIcon(passed));
        item->setText(5, QString("±%1").arg(m_tolerance));

        if(!passed) {
            QColor warningColor(255, 200, 200);
            for(int i = 0; i < 6; i++) {
                item->setBackground(i, warningColor);
            }
            item->setToolTip(0, QString("偏差超出公差范围！\nX偏差: %1\nY偏差: %2")
                              .arg(dx).arg(dy));
        }

        // 设置状态列颜色
        item->setForeground(4, passed ? QColor(76, 175, 80) : QColor(244, 67, 54));
    }
}

void StyledComparisonDialog::populateChannelsTable()
{
    m_channelsTree->clear();

    auto addChannelItems = [this](const QMap<int, QPair<QPoint, QPoint>>& channels,
                                   const QString& type) {
        for(auto it = channels.begin(); it != channels.end(); ++it) {
            int channel = it.key();
            const QPoint& expected = it.value().first;
            const QPoint& actual = it.value().second;

            int dx = actual.x() - expected.x();
            int dy = actual.y() - expected.y();
            bool passed = (abs(dx) <= m_tolerance && abs(dy) <= m_tolerance);

            if(passed) m_passedItems++;

            QTreeWidgetItem* item = new QTreeWidgetItem(m_channelsTree);
            item->setText(0, QString::number(channel + 1));
            item->setText(1, type);
            item->setText(2, QString("(%1, %2)").arg(expected.x()).arg(expected.y()));
            item->setText(3, QString("(%1, %2)").arg(actual.x()).arg(actual.y()));
            item->setText(4, QString("(%1, %2)").arg(dx).arg(dy));
            item->setText(5, getStatusIcon(passed));

            if(!passed) {
                QColor warningColor(255, 200, 200);
                for(int i = 0; i < 6; i++) {
                    item->setBackground(i, warningColor);
                }
            }

            item->setForeground(5, passed ? QColor(76, 175, 80) : QColor(244, 67, 54));
        }
    };

    addChannelItems(m_handsChannels, "抓手");
    addChannelItems(m_reagentChannels, "试剂针");
}

void StyledComparisonDialog::populateTrayTable()
{
    m_trayTree->clear();

    auto addTrayItems = [this](const QMap<int, QPair<QPoint, QPoint>>& trays,
                                const QString& type) {
        for(auto it = trays.begin(); it != trays.end(); ++it) {
            int tray = it.key();
            const QPoint& expected = it.value().first;
            const QPoint& actual = it.value().second;

            int dx = actual.x() - expected.x();
            int dy = actual.y() - expected.y();
            bool passed = (abs(dx) <= m_tolerance && abs(dy) <= m_tolerance);

            if(passed) m_passedItems++;

            QTreeWidgetItem* item = new QTreeWidgetItem(m_trayTree);
            item->setText(0, QString::number(tray + 1));
            item->setText(1, type);
            item->setText(2, QString("(%1, %2)").arg(expected.x()).arg(expected.y()));
            item->setText(3, QString("(%1, %2)").arg(actual.x()).arg(actual.y()));
            item->setText(4, QString("(%1, %2)").arg(dx).arg(dy));
            item->setText(5, getStatusIcon(passed));

            if(!passed) {
                QColor warningColor(255, 200, 200);
                for(int i = 0; i < 6; i++) {
                    item->setBackground(i, warningColor);
                }
            }

            item->setForeground(5, passed ? QColor(76, 175, 80) : QColor(244, 67, 54));
        }
    };

    addTrayItems(m_trayHands, "抓手");
    addTrayItems(m_trayBlood, "血针");
}

void StyledComparisonDialog::updateSummary()
{
    m_summaryText->clear();

    int passedCount = m_passedItems;
    int failedCount = m_totalItems - passedCount;
    double passRate = m_totalItems > 0 ? (passedCount * 100.0 / m_totalItems) : 0;

    // 更新进度条
    m_overallProgress->setValue(static_cast<int>(passRate));

    // 更新状态标签
    if(failedCount == 0) {
        m_statusLabel->setText( QString::fromUtf8("✓ 所有坐标验证通过"));
        m_statusLabel->setStyleSheet("background-color: #d4edda; color: #155724;");
    } else {
        m_statusLabel->setText( QString::fromUtf8("✗ 发现 %1 个坐标偏差，通过率: %2%")
                               .arg(failedCount).arg(passRate, 0, 'f', 1));
        m_statusLabel->setStyleSheet("background-color: #f8d7da; color: #721c24;");
    }

    QString summary;
    QTextStream stream(&summary);

    stream <<  QString::fromUtf8("═══════════════════════════════════════════════════════════\n");
    stream << QString::fromUtf8("                      坐标校验汇总报告                        \n");
    stream << QString::fromUtf8("═══════════════════════════════════════════════════════════\n\n");

    stream << QString("校验时间: %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    stream << QString("公差范围: ±%1\n\n").arg(m_tolerance);

    stream << QString::fromUtf8("───────────────────────────────────────────────────────────\n");
    stream << QString("总检查项: %1\n").arg(m_totalItems);
    stream << QString("通过数量: %1\n").arg(passedCount);
    stream << QString("失败数量: %1\n").arg(failedCount);
    stream << QString("通过率: %1%\n").arg(passRate, 0, 'f', 2);
    stream << QString::fromUtf8("───────────────────────────────────────────────────────────\n\n");

    if(failedCount > 0) {
        stream << QString::fromUtf8("【失败项详情】\n\n");

        // 收集失败项
        struct FailItem {
            QString category;
            QString name;
            QPoint expected;
            QPoint actual;
            int dx;
            int dy;
        };
        QList<FailItem> failures;

        // 检查基础坐标
        for(auto it = m_basicPoints.begin(); it != m_basicPoints.end(); ++it) {
            const QPoint& expected = it.value().first;
            const QPoint& actual = it.value().second;
            int dx = actual.x() - expected.x();
            int dy = actual.y() - expected.y();
            if(abs(dx) > m_tolerance || abs(dy) > m_tolerance) {
                failures.append({"基础坐标", it.key(), expected, actual, dx, dy});
            }
        }

        // 检查通道坐标
        auto addChannelFailures = [&](const QMap<int, QPair<QPoint, QPoint>>& channels,
                                       const QString& type) {
            for(auto it = channels.begin(); it != channels.end(); ++it) {
                const QPoint& expected = it.value().first;
                const QPoint& actual = it.value().second;
                int dx = actual.x() - expected.x();
                int dy = actual.y() - expected.y();
                if(abs(dx) > m_tolerance || abs(dy) > m_tolerance) {
                    failures.append({type, QString("通道 %1").arg(it.key() + 1),
                                   expected, actual, dx, dy});
                }
            }
        };

        addChannelFailures(m_handsChannels, "抓手通道");
        addChannelFailures(m_reagentChannels, "试剂针通道");

        auto addTrayFailures = [&](const QMap<int, QPair<QPoint, QPoint>>& trays,
                                    const QString& type) {
            for(auto it = trays.begin(); it != trays.end(); ++it) {
                const QPoint& expected = it.value().first;
                const QPoint& actual = it.value().second;
                int dx = actual.x() - expected.x();
                int dy = actual.y() - expected.y();
                if(abs(dx) > m_tolerance || abs(dy) > m_tolerance) {
                    failures.append({type, QString("托盘 %1").arg(it.key() + 1),
                                   expected, actual, dx, dy});
                }
            }
        };

        addTrayFailures(m_trayHands, "托盘抓手");
        addTrayFailures(m_trayBlood, "托盘血针");

        for(const auto& fail : failures) {
            stream << QString("  [%1] %2\n").arg(fail.category).arg(fail.name);
            stream << QString("    校验: (%1, %2) → 实际: (%3, %4)\n")
                      .arg(fail.expected.x()).arg(fail.expected.y())
                      .arg(fail.actual.x()).arg(fail.actual.y());
            stream << QString("    偏差: (%1, %2) %3\n")
                      .arg(fail.dx).arg(fail.dy)
                      .arg(abs(fail.dx) > m_tolerance || abs(fail.dy) > m_tolerance ? "⚠ 超差" : "");
        }
        stream << "\n";
    }

    stream << QString::fromUtf8("═══════════════════════════════════════════════════════════\n");
    stream << QString::fromUtf8("建议：\n");
    if(failedCount > 0) {
        stream << QString::fromUtf8("  1. 请检查机械结构是否正常\n");
        stream << QString::fromUtf8("  2. 建议执行坐标校准操作\n");
        stream << QString::fromUtf8("  3. 校准后请重新校验确认\n");
    } else {
        stream << QString::fromUtf8("  所有坐标正常，设备可以正常运行。\n");
    }

    m_summaryText->setPlainText(summary);
}

QString StyledComparisonDialog::getStatusIcon(bool passed)
{
    return passed ? "✓ 通过" : "✗ 失败";
}

QString StyledComparisonDialog::getStatusColor(bool passed)
{
    return passed ? "#4CAF50" : "#F44336";
}

void StyledComparisonDialog::onFilterChanged(int index)
{
    // 实现过滤功能
    auto filterTree = [index](QTreeWidget* tree) {
        if(!tree) return;
        for(int i = 0; i < tree->topLevelItemCount(); i++) {
            QTreeWidgetItem* item = tree->topLevelItem(i);
            bool isPassed = item->text(4).contains("通过");
            if(index == 1) { // 仅显示差异
                item->setHidden(isPassed);
            } else if(index == 2) { // 仅显示通过
                item->setHidden(!isPassed);
            } else { // 全部
                item->setHidden(false);
            }
        }
    };

    filterTree(m_basicTree);
    filterTree(m_channelsTree);
    filterTree(m_trayTree);
}

void StyledComparisonDialog::onCalibrateClicked()
{
    if(m_calibrationCallback) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "确认校准",
            "此操作将使用仪器当前坐标覆盖配置文件中的坐标。\n\n"
            "原配置文件将被备份为 .bak 文件。\n\n"
            "确定要继续吗？",
            QMessageBox::Yes | QMessageBox::No
        );

        if(reply == QMessageBox::Yes) {
            if(m_calibrationCallback()) {
                QMessageBox::information(this, "校准完成",
                                        "轴坐标校准成功！\n配置文件已更新。");
                accept();
            } else {
                QMessageBox::critical(this, "校准失败",
                                     "轴坐标校准失败，请检查日志。");
            }
        }
    }
}

void StyledComparisonDialog::onExportClicked()
{
    exportToFile();
}

void StyledComparisonDialog::exportToFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "导出报告",
        QString("Coordinate_Report_%1.html")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "HTML文件 (*.html);;文本文件 (*.txt);;所有文件 (*)"
    );

    if(fileName.isEmpty()) return;

    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        if(fileName.endsWith(".html")) {
            // 导出 HTML 格式
            stream << "<!DOCTYPE html>\n";
            stream << "<html>\n<head>\n";
            stream << "<meta charset=\"UTF-8\">\n";
            stream << QString::fromUtf8("<title>坐标校验报告</title>\n");
            stream << "<style>\n";
            stream << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
            stream << "h1 { color: #333; }\n";
            stream << "table { border-collapse: collapse; width: 100%; margin: 10px 0; }\n";
            stream << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
            stream << "th { background-color: #4CAF50; color: white; }\n";
            stream << "tr:nth-child(even) { background-color: #f2f2f2; }\n";
            stream << ".passed { color: green; font-weight: bold; }\n";
            stream << ".failed { color: red; font-weight: bold; }\n";
            stream << "</style>\n";
            stream << "</head>\n<body>\n";

            stream << QString::fromUtf8("<h1>坐标校验报告</h1>\n");
            stream << QString::fromUtf8("<p>生成时间: ") << QDateTime::currentDateTime().toString() << "</p>\n";
            stream << QString::fromUtf8("<p>公差范围: ±") << m_tolerance << "</p>\n";

            // 添加表格
            stream << QString::fromUtf8("<h2>基础坐标对比</h2>\n");
            stream << QString::fromUtf8("<table>\n<tr><th>坐标项</th><th>校验坐标</th><th>实际坐标</th><th>偏差</th><th>状态</th></tr>\n");
            for(int i = 0; i < m_basicTree->topLevelItemCount(); i++) {
                auto* item = m_basicTree->topLevelItem(i);
                stream << "<tr>";
                for(int j = 0; j < 5; j++) {
                    QString text = item->text(j);
                    if(j == 4) {
                        bool passed = text.contains("通过");
                        stream << QString("<td class=\"%1\">%2</td>")
                                  .arg(passed ? "passed" : "failed").arg(text);
                    } else {
                        stream << "<td>" << text << "</td>";
                    }
                }
                stream << "</tr>\n";
            }
            stream << "</table>\n";

            stream << QString::fromUtf8("<h2>汇总信息</h2>\n");
            stream << "<pre>" << m_summaryText->toPlainText() << "</pre>\n";

            stream << "</body>\n</html>\n";
        } else {
            // 导出文本格式
            stream << m_summaryText->toPlainText();
        }

        file.close();
        QMessageBox::information(this, "导出成功",
                                QString("报告已保存到:\n%1").arg(fileName));
    }
}

#include "CustomMessageBox.h"
#include <QStyle>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMap>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

CustomMessageBox::CustomMessageBox(const QString &title, const QString &text,
                                   QMessageBox::Icon icon,
                                   QMessageBox::StandardButtons buttons,
                                   QWidget *parent)
    : QDialog(parent), m_result(QMessageBox::NoButton)
{
    this->setWindowTitle(title);
    this->setAttribute(Qt::WA_StyledBackground, true);
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::WindowModal);
    this->setModal(true);

    // 样式表保持不变（略，同原代码）
    this->setStyleSheet(R"(
        QDialog {
            background-color: #FFFFFF;
            border: 1px solid #D0D5DB;
            border-radius: 4px;
        }
        QLabel#messageLabel {
            color: #1F2A3E;
            border: none;
            font: 14pt "Microsoft YaHei";
            background-color: transparent;
        }
        QPushButton {
            background-color: #F2F4F7;
            color: #1F2A3E;
            border: 1px solid #CBD0D8;
            border-radius: 3px;
            padding: 4px 20px;
            min-width: 90px;
            font: 12pt "Microsoft YaHei";
            height: 32px;
        }
        QPushButton:hover {
            background-color: #E6E9F0;
            border-color: #A9B2BC;
        }
        QPushButton:pressed {
            background-color: #DCE0E8;
        }
        QPushButton#okButton {
            background-color: #0078D7;
            color: white;
            border-color: #005A9E;
            font-weight: bold;
        }
        QPushButton#okButton:hover {
            background-color: #106EBE;
        }
        QPushButton#okButton:pressed {
            background-color: #005A9E;
        }
    )");

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(15);

    // 内容区域（图标+文本）
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);
    contentLayout->setAlignment(Qt::AlignCenter);

    QLabel *iconLabel = new QLabel(this);
    iconLabel->setFixedSize(48, 48);
    iconLabel->setScaledContents(true);
    iconLabel->setAttribute(Qt::WA_TranslucentBackground);
    QIcon tmpIcon;
    switch (icon) {
        case QMessageBox::Warning:
            tmpIcon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
            break;
        case QMessageBox::Critical:
            tmpIcon = style()->standardIcon(QStyle::SP_MessageBoxCritical);
            break;
        case QMessageBox::Information:
            tmpIcon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
            break;
        case QMessageBox::Question:
            tmpIcon = style()->standardIcon(QStyle::SP_MessageBoxQuestion);
            break;
        default:
            tmpIcon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
            break;
    }
    if (!tmpIcon.isNull()) {
        iconLabel->setPixmap(tmpIcon.pixmap(48, 48));
    } else {
        iconLabel->hide();
        contentLayout->setSpacing(0);
    }

    QLabel *messageLabel = new QLabel(text, this);
    messageLabel->setObjectName("messageLabel");
    messageLabel->setWordWrap(true);
    messageLabel->setMinimumWidth(260);
    messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(messageLabel, 1);

    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addLayout(contentLayout);
    centerLayout->addStretch();
    mainLayout->addLayout(centerLayout);

    // 分隔线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #E5E7EB; max-height: 1px;");
    mainLayout->addWidget(line);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);
    buttonLayout->addStretch();

    // 根据 buttons 参数动态创建按钮
    QMap<QMessageBox::StandardButton, QPushButton*> buttonMap;
    if (buttons & QMessageBox::Ok) {
        QPushButton *btn = new QPushButton(tr("确定"), this);
        btn->setObjectName("okButton");
        buttonMap[QMessageBox::Ok] = btn;
        buttonLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this, btn]() { m_result = QMessageBox::Ok; accept(); });
    }
    if (buttons & QMessageBox::Cancel) {
        QPushButton *btn = new QPushButton(tr("取消"), this);
        buttonMap[QMessageBox::Cancel] = btn;
        buttonLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this, btn]() { m_result = QMessageBox::Cancel; reject(); });
    }
    if (buttons & QMessageBox::Yes) {
        QPushButton *btn = new QPushButton(tr("是"), this);
        buttonMap[QMessageBox::Yes] = btn;
        buttonLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this, btn]() { m_result = QMessageBox::Yes; accept(); });
    }
    if (buttons & QMessageBox::No) {
        QPushButton *btn = new QPushButton(tr("否"), this);
        buttonMap[QMessageBox::No] = btn;
        buttonLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this, btn]() { m_result = QMessageBox::No; accept(); });
    }
    // 可扩展其他按钮...

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    this->adjustSize();
    this->setMinimumWidth(350);
    this->setMaximumWidth(450);
    this->setMaximumHeight(300);

    // 如果没有指定任何按钮，默认添加一个“确定”
    if (buttonMap.isEmpty()) {
        QPushButton *btn = new QPushButton(tr("确定"), this);
        buttonMap[QMessageBox::Ok] = btn;
        buttonLayout->insertWidget(0, btn);
        connect(btn, &QPushButton::clicked, [this, btn]() { m_result = QMessageBox::Ok; accept(); });
    }
}

QMessageBox::StandardButton CustomMessageBox::question(QWidget *parent, const QString &title, const QString &text,
                                                       QMessageBox::StandardButtons buttons)
{
    CustomMessageBox msgBox(title, text, QMessageBox::Question, buttons, parent);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.show();
    msgBox.raise();
    msgBox.activateWindow();
    msgBox.exec();
    return msgBox.m_result;
}

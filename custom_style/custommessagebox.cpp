// CustomMessageBox.cpp
#include "CustomMessageBox.h"
#include <QStyle>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

CustomMessageBox::CustomMessageBox(const QString &title, const QString &text,
                                   QMessageBox::Icon icon, QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle(title);
    this->setModal(true);
    this->setAttribute(Qt::WA_StyledBackground, true);

    // 医疗仪器专用提示框样式
    this->setStyleSheet(R"(
        QDialog {
            background-color: #FFFFFF;
            border: 1px solid #D0D5DB;
            border-radius: 4px;
        }
        QLabel#messageLabel {
            color: #1F2A3E;
            font: 14pt "Microsoft YaHei";
            background-color: transparent;
        }
        QPushButton {
            background-color: #F2F4F7;
            color: #1F2A3E;
            border: 1px solid #CBD0D8;
            border-radius: 3px;
            padding: 8px 20px;
            min-width: 90px;
            font: 12pt "Microsoft YaHei";
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

    // 主布局 - 使用更合理的边距
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(15);

    // ========== 内容区域（图标+文本）==========
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);
    contentLayout->setAlignment(Qt::AlignCenter);  // 左对齐垂直居中

    // 图标 - 固定大小
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setFixedSize(48, 48);  // 加大图标更醒目
    iconLabel->setScaledContents(true);
    iconLabel->setAttribute(Qt::WA_TranslucentBackground);  // 透明背景
    // 获取系统图标
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
        contentLayout->setSpacing(0);  // 消除多余间距
    }

    // 文本标签
    QLabel *messageLabel = new QLabel(text, this);
    messageLabel->setObjectName("messageLabel");
    messageLabel->setWordWrap(true);
    messageLabel->setMinimumWidth(260);
    messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(messageLabel, 1);  // 文本可拉伸

    // 外层居中布局
    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addLayout(contentLayout);
    centerLayout->addStretch();

    mainLayout->addLayout(centerLayout);


    // 分隔线（可选，让界面更有层次感）
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #E5E7EB; max-height: 1px;");
    mainLayout->addWidget(line);

    // ========== 按钮区域 ==========
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);
    buttonLayout->addStretch();

    QPushButton *okButton = new QPushButton(tr("确定"), this);
    okButton->setObjectName("okButton");
    okButton->setFixedSize(100, 38);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(okButton);
    // 按钮默认和快捷键
    okButton->setDefault(true);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // ========== 窗口大小调整 ==========
    this->adjustSize();
    this->setMinimumWidth(350);
    this->setMaximumWidth(550);
    this->setMaximumHeight(300);
}
void CustomMessageBox::information(QWidget *parent, const QString &title, const QString &text)
{
    CustomMessageBox msgBox(title, text, QMessageBox::Information, parent);
    msgBox.exec();
}



void CustomMessageBox::critical(QWidget *parent, const QString &title, const QString &text)
{
    CustomMessageBox msgBox(title, text, QMessageBox::Critical, parent);
    msgBox.exec();
}

void CustomMessageBox::question(QWidget *parent, const QString &title, const QString &text)
{
    CustomMessageBox msgBox(title, text, QMessageBox::Question, parent);
    msgBox.exec();
}

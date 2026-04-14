#include "IndustrialImageGallery.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QIcon>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPainter>
#include <QDateTime>
#include <QFileInfo>

IndustrialImageGallery::IndustrialImageGallery(QWidget *parent)
    : QWidget(parent)
    , m_titleBar(nullptr)
    , m_thumbList(nullptr)
    , m_imageLabel(nullptr)
    , m_infoTextEdit(nullptr)
    , m_pathEdit(nullptr)
    , m_selectButton(nullptr)
{
    // 设置窗口无边框，使用自定义标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    setupUI();
    applyMedicalBlueStyle();
}

IndustrialImageGallery::~IndustrialImageGallery()
{
}

void IndustrialImageGallery::onCloseRequested()
{
    close();
}

void IndustrialImageGallery::onMinimizeRequested()
{
    showMinimized();
}

void IndustrialImageGallery::onMaximizeRequested()
{
    if (isMaximized()) {
        showNormal();
        m_titleBar->updateMaximizeButton(false);
    } else {
        showMaximized();
        m_titleBar->updateMaximizeButton(true);
    }
}

void IndustrialImageGallery::setupUI()
{
    // ========== 顶部：自定义标题栏 ==========
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle("医疗图像库");

    // ========== 上方区域：左侧图片 + 右侧信息 ==========
    // 左侧图片显示区域
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setScaledContents(false);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_imageLabel->setMinimumSize(400, 300);

    // 右侧图片信息显示（使用只读 QTextEdit，支持富文本）
    m_infoTextEdit = new QTextEdit(this);
    m_infoTextEdit->setReadOnly(true);
    m_infoTextEdit->setMaximumWidth(250);          // 固定宽度
    m_infoTextEdit->setPlaceholderText(tr("点击缩略图查看图片信息"));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(m_imageLabel, 1);        // 左侧图片区，拉伸
    topLayout->addWidget(m_infoTextEdit, 0);      // 右侧信息区，固定宽度

    // ========== 中间：文件夹选择栏 ==========
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setReadOnly(true);
    m_pathEdit->setPlaceholderText(tr("未选择文件夹"));
    m_selectButton = new QPushButton(tr("选择文件夹"), this);

    QHBoxLayout *toolLayout = new QHBoxLayout;
    toolLayout->addWidget(m_pathEdit);
    toolLayout->addWidget(m_selectButton);

    // ========== 下方：缩略图列表（水平滚动） ==========
    m_thumbList = new QListWidget(this);
    m_thumbList->setViewMode(QListView::IconMode);
    m_thumbList->setFlow(QListView::LeftToRight);      // 水平流向
    m_thumbList->setWrapping(false);                   // 不换行，产生水平滚动条
    m_thumbList->setIconSize(QSize(100, 100));
    m_thumbList->setGridSize(QSize(120, 130));
    m_thumbList->setResizeMode(QListView::Adjust);
    m_thumbList->setMovement(QListView::Static);
    m_thumbList->setSpacing(8);
    m_thumbList->setWordWrap(true);
    m_thumbList->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_thumbList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用垂直滚动条

    // ========== 主布局（垂直排列） ==========
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);        // 移除边距，让标题栏贴边
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_titleBar);                  // 标题栏
    mainLayout->addLayout(topLayout, 2);      // 上方区域占2份高度
    mainLayout->addLayout(toolLayout);        // 工具栏占自适应高度
    mainLayout->addWidget(m_thumbList, 1);    // 缩略图区域占1份高度
    setLayout(mainLayout);

    // 连接信号槽
    connect(m_selectButton, &QPushButton::clicked, this, &IndustrialImageGallery::onSelectFolder);
    connect(m_thumbList, &QListWidget::itemClicked, this, &IndustrialImageGallery::onThumbnailClicked);
    
    // 连接标题栏信号
    connect(m_titleBar, &CustomTitleBar::closeRequested, this, &IndustrialImageGallery::onCloseRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &IndustrialImageGallery::onMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &IndustrialImageGallery::onMaximizeRequested);
}

void IndustrialImageGallery::applyMedicalBlueStyle()
{
    QString style = R"(
        /* 医疗蓝色主题 - 专业医疗风格 */
        QWidget {
            background-color: #f0f8ff;  /* 浅蓝色背景，医疗感 */
            color: #2c3e50;             /* 深蓝色文字 */
            font-family: "Segoe UI", "Microsoft YaHei";
            font-size: 12px;
        }
        QListWidget {
            background-color: #e6f2ff;  /* 浅蓝色背景 */
            border: 1px solid #b3d9ff;  /* 浅蓝色边框 */
            outline: none;
            border-radius: 4px;
        }
        QListWidget::item {
            background-color: #ffffff;  /* 白色背景 */
            border: 1px solid #d1e7ff;  /* 浅蓝色边框 */
            border-radius: 4px;
            padding: 5px;
            margin: 2px;
        }
        QListWidget::item:selected {
            background-color: #4a90e2;  /* 医疗蓝色 */
            border: 1px solid #2c6fb8;
            color: white;
        }
        QListWidget::item:hover {
            background-color: #e6f7ff;  /* 悬停浅蓝色 */
            border-color: #4a90e2;
        }
        QLabel {
            background-color: #ffffff;  /* 白色背景 */
            border: 2px solid #b3d9ff;  /* 浅蓝色边框 */
            border-radius: 4px;
        }
        QPushButton {
            background-color: #4a90e2;  /* 医疗蓝色 */
            border: 1px solid #2c6fb8;
            padding: 8px 16px;
            min-width: 100px;
            color: white;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #5a9ae2;  /* 稍亮的蓝色 */
            border-color: #3c7fc8;
        }
        QPushButton:pressed {
            background-color: #3a80d2;  /* 深蓝色 */
            border-color: #1c5fa8;
        }
        QLineEdit {
            background-color: #ffffff;  /* 白色背景 */
            border: 1px solid #b3d9ff;  /* 浅蓝色边框 */
            padding: 6px;
            color: #2c3e50;             /* 深蓝色文字 */
            border-radius: 4px;
        }
        QScrollBar:horizontal {
            background: #e6f2ff;        /* 浅蓝色背景 */
            height: 12px;
            margin: 0px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal {
            background: #4a90e2;        /* 医疗蓝色 */
            min-width: 20px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #5a9ae2;        /* 悬停时稍亮 */
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
        QTextEdit {
            background-color: #ffffff;  /* 白色背景 */
            border: 1px solid #b3d9ff;  /* 浅蓝色边框 */
            color: #2c3e50;             /* 深蓝色文字 */
            border-radius: 4px;
            padding: 8px;
        }
    )";
    setStyleSheet(style);
}

void IndustrialImageGallery::loadDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        QMessageBox::warning(this, tr("错误"), tr("文件夹不存在: %1").arg(path));
        return;
    }

    m_thumbList->clear();
    m_currentImagePath.clear();
    m_imageLabel->clear();
    m_infoTextEdit->clear();

    QStringList filters;
    filters << "*.prt" << "*.PRT";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files, QDir::Name);

    if (fileList.isEmpty()) {
        m_pathEdit->setText(tr("%1 (无.prt文件)").arg(path));
        return;
    }

    m_pathEdit->setText(path);
    setCursor(Qt::WaitCursor);

    for (const QFileInfo &fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        QListWidgetItem *item = createThumbnailItem(filePath);
        if (item) {
            m_thumbList->addItem(item);
        }
    }

    setCursor(Qt::ArrowCursor);
}

QListWidgetItem* IndustrialImageGallery::createThumbnailItem(const QString &filePath)
{
    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    QSize thumbSize(100, 100);
    reader.setScaledSize(thumbSize);

    QImage thumbnail = reader.read();
    QPixmap iconPixmap;

    if (!thumbnail.isNull()) {
        iconPixmap = QPixmap::fromImage(thumbnail);
    } else {
        QPixmap placeholder(thumbSize);
        placeholder.fill(QColor(60, 60, 60));
        QPainter painter(&placeholder);
        painter.setPen(QColor(200, 200, 200));
        painter.drawText(placeholder.rect(), Qt::AlignCenter, "?");
        iconPixmap = placeholder;
    }

    QListWidgetItem *item = new QListWidgetItem;
    item->setIcon(QIcon(iconPixmap));
    item->setText(QFileInfo(filePath).baseName());
    item->setData(Qt::UserRole, filePath);
    item->setToolTip(filePath);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

void IndustrialImageGallery::onThumbnailClicked(QListWidgetItem *item)
{
    if (!item) return;
    QString filePath = item->data(Qt::UserRole).toString();
    if (!filePath.isEmpty()) {
        showImage(filePath);
        updateImageInfo(filePath);
        m_currentImagePath = filePath;
    }
}

void IndustrialImageGallery::showImage(const QString &filePath)
{
    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    QImage image = reader.read();

    if (image.isNull()) {
        m_imageLabel->setText(tr("无法加载图片\n%1").arg(filePath));
        return;
    }

    QSize labelSize = m_imageLabel->size();
    if (labelSize.width() <= 0 || labelSize.height() <= 0) {
        labelSize = QSize(400, 300);
    }
    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap scaled = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_imageLabel->setPixmap(scaled);
}

void IndustrialImageGallery::updateImageInfo(const QString &filePath)
{
    QFileInfo info(filePath);
    QString infoText;

    infoText += QString("<b>文件名：</b> %1<br>").arg(info.fileName());
    infoText += QString("<b>大小：</b> %1 KB<br>").arg(info.size() / 1024.0, 0, 'f', 2);
    infoText += QString("<b>路径：</b> %1<br>").arg(info.absolutePath());
    infoText += QString("<b>修改时间：</b> %1<br>").arg(info.lastModified().toString("yyyy-MM-dd hh:mm:ss"));

    // 尝试读取图片实际尺寸
    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    QSize imageSize = reader.size();
    if (imageSize.isValid()) {
        infoText += QString("<b>图片尺寸：</b> %1 x %2 像素<br>").arg(imageSize.width()).arg(imageSize.height());
    } else {
        // 如果读取失败，尝试直接读取图像获取尺寸
        QImage image = reader.read();
        if (!image.isNull()) {
            infoText += QString("<b>图片尺寸：</b> %1 x %2 像素<br>").arg(image.width()).arg(image.height());
        }
    }

    m_infoTextEdit->setHtml(infoText);
}

void IndustrialImageGallery::onSelectFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("选择包含 .prt 图片的文件夹"),
                                                    m_pathEdit->text(),
                                                    QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        loadDirectory(dir);
    }
}

void IndustrialImageGallery::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_currentImagePath.isEmpty() && m_imageLabel) {
        showImage(m_currentImagePath);
    }
}

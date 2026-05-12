#ifndef INDUSTRIALIMAGEGALLERY_H
#define INDUSTRIALIMAGEGALLERY_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include "customtitlebar.h"

class IndustrialImageGallery : public QWidget
{
    Q_OBJECT

public:
    explicit IndustrialImageGallery(QWidget *parent = nullptr);
    ~IndustrialImageGallery();

public slots:
    void loadDirectory(const QString &path);
    void onSelectFolder();

private slots:
    void onThumbnailClicked(QListWidgetItem *item);
    void resizeEvent(QResizeEvent *event);
    void onCloseRequested();
    void onMinimizeRequested();
    void onMaximizeRequested();

private:
    void setupUI();
    void applyMedicalBlueStyle();
    QListWidgetItem* createThumbnailItem(const QString &filePath);
    void showImage(const QString &filePath);
    void updateImageInfo(const QString &filePath);

    // UI 组件
    CustomTitleBar *m_titleBar;    // 自定义标题栏
    QListWidget *m_thumbList;      // 缩略图列表（下方，水平滚动）
    QLabel      *m_imageLabel;     // 图片显示区（上方右侧）
    QTextEdit   *m_infoTextEdit;   // 图片信息显示区（上方左侧）
    QLineEdit   *m_pathEdit;       // 当前文件夹路径编辑框
    QPushButton *m_selectButton;   // 选择文件夹按钮

    QString      m_currentImagePath; // 当前显示的原图路径
};

#endif // INDUSTRIALIMAGEGALLERY_H

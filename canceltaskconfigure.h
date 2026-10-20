#ifndef CANCELTASKCONFIGURE_H
#define CANCELTASKCONFIGURE_H

#include <QDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPainter>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMimeData>
#include <QDrag>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include "opencvFindRBC/customtitlebar.h"

namespace Ui {
class CancelTaskConfigure;
}

class CancelTaskConfigure : public QDialog
{
    Q_OBJECT

public:
    explicit CancelTaskConfigure(QWidget *parent, QList<int> click_sampleid);
    ~CancelTaskConfigure();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();
    void onOkClicked();
    void onCancelClicked();
    void delenter(QTableWidgetItem *item);

signals:
    void makesureCacelIdList(QList<int>);

private:
    void initUI();
    void initStyle();
    void initTable();
    void SetColumnText(QTableWidget *tablewiget, int row, int col, QString text);

    Ui::CancelTaskConfigure *ui;
    CustomTitleBar *m_titleBar;

    // 主布局
    QVBoxLayout *m_mainLayout;
    QWidget *m_centerWidget;
    QVBoxLayout *m_centerLayout;

    // 控件
    QTableWidget *m_tableWidget;
    QPushButton *m_cancelBtn;
    QPushButton *m_okBtn;

    QList<int> mclickHole;
    QList<int> deleteitem;
    bool m_isMaximized;
    QRect m_normalGeometry;
};

#endif // CANCELTASKCONFIGURE_H

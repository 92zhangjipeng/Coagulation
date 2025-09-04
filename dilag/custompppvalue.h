#ifndef CUSTOMPPPVALUE_H
#define CUSTOMPPPVALUE_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <aligndelegate.h>
#include <QMap>




namespace Ui {
class customPPPValue;
}

class customPPPValue : public QWidget
{
    Q_OBJECT

public:
    explicit customPPPValue(QWidget *parent = 0);
    ~customPPPValue();

    static QMap<quint8, quint16> getChannelPPPValues();
    static quint16 getChannelPPPValues(quint8 channel);

private slots:
    void onSaveButtonClicked();

private:
    void initTableWidget();
    void loadSycninitPPPvalue();

    void updateTableFromGlobalData();
    void saveToCSVFile();
    void createDefaultCSVFile();
    QString getCSVFilePath();

private:
    Ui::customPPPValue *ui;

    // 使用静态成员变量作为全局容器
    static QMap<quint8, quint16> m_channelPPPValues;

};

#endif // CUSTOMPPPVALUE_H

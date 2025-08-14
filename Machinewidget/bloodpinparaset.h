#ifndef BLOODPINPARASET_H
#define BLOODPINPARASET_H

#include <QWidget>
#include <QTableWidget>


struct bloodPinData {
    // 参数类型维度
    QString paraKind = "BloodPressure";    // 参数类型
    int paraKindRows = 0;    // 参数类型表格行数
    int paraKindCols = 0;    // 参数类型表格列数

    // 参数数据维度
    QVariant paradata;    // 参数数据
    int paradataRows = 0;    // 数据存储行数
    int paradataCols = 0;    // 数据存储列数

    // 默认构造函数
    bloodPinData() = default;
};

namespace Ui {
class BloodPinParaSet;
}

class BloodPinParaSet : public QWidget
{
    Q_OBJECT

public:
    explicit BloodPinParaSet(QWidget *parent = 0);
    ~BloodPinParaSet();

private:
    void initializeTable();
    void insertParaBlood(QString types,int rows,int cols,
                         QVariant data,int datarows,int datacols);

private:
    Ui::BloodPinParaSet *ui;
    QTableWidget *ptable = nullptr;
};

#endif // BLOODPINPARASET_H

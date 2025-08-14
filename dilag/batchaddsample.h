#ifndef BATCHADDSAMPLE_H
#define BATCHADDSAMPLE_H

#include <QTableWidget>
#include <QVariantList>
#include <QWidget>


struct SAMPLEIDINFO{
    QString idsample;
    QString pindownHeight;
    QString testProtect;
};
Q_DECLARE_METATYPE(SAMPLEIDINFO)

namespace Ui {
class batchAddSample;
}

class batchAddSample : public QWidget
{
    Q_OBJECT

public:
    explicit batchAddSample(QWidget *parent = 0);
    ~batchAddSample();

    void obtainFirstSampleId(QTableWidget *pSampleIdTable);

    void obtainSelectTestProject();
protected:
    virtual void closeEvent(QCloseEvent* event) override;

signals:
    void exitWidget();
    void send_askData(QVariant dataVar);

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_save_clicked();

private:
    Ui::batchAddSample *ui;
};

#endif // BATCHADDSAMPLE_H

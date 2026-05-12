#ifndef PRINTTHEREPORT_H
#define PRINTTHEREPORT_H

#include <QObject>
#include <QPdfWriter>
#include <QPixmap>
#include <QPrinter>
#include <QTextEdit>
#include <QThread>
#include <QVariant>

class Printthereport : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit Printthereport(QObject *parent = nullptr);
    ~Printthereport();

    void _Start();

    enum Page_Size{A4,A3,A5,CUSTOM_PAGE};
  
private slots:

    void _threadruningstart();

public slots :

    void slotwritePdf(QString pathload);

    void slotprintoutresult();


public:
    void  initpara(); //初始化参数

    void _settingSampleId(const QString idunm);

    void _setpixmapcurve(QPixmap pixmapcurve);

    void _setsamplename(const QString );

    void _setsampleSex(const QString sexindex);

    void _setsampleBednum(const QString bednum);

    void _setsampleAges(const quint8 ages);

    void _setsampledepartment(const QString sampledepartment);

    void _setsampleBarcode(const QString barcodenum);

    void _setrefertithedoctor(const QString refertithedoctor);

    void _setreviewdoctors(const QString);

    void _setTestCurvedata(QVariant data);

private:

    void  printDocument(QPrinter *printer);


    void  CreatPdfFileLayout(const QString& file_path);

    /**
     * @brief switchReagentName
     * @param pPainter   画笔
     * @param indexReag  试剂下标
     * @param n          要绘制的第N个
     * @param textbottomy 上一行首文字的最小y
     * @param width       绘制宽
     * @param height      绘制高
     * @param spacesize   做边距X
     * @return 返回试剂名称的最下y
     */
    QPoint switchReagentName(QPainter *pPainter, quint8 indexReag,quint8 n,
                              int textbottomy, int width, int height, int spacesize); //映射试剂名称


    void fullintopdfreagresult(QPainter *pPainter, QString resultreagdata,
                               int k, QPoint bottompos, int itemWidth,
                               int itemHeight);


    //单位
    void intsertwriteunit(QPainter *pPainter,QPoint bottompos,
                          int itemWidth,int itemHeight,const QString keyUnit);

    //参考结果
    void insertwriteoffset(QPainter *painter, const QPoint &bottomPos, const int itemWidth,
                           const int itemHeight, const QString &resultValue,
                           const QVariant &refMinVal, const QVariant &refMaxVal);


    //参考值
    void insertReferenceValues(QPainter *painter, const QPoint &bottomPos,
                               const int itemWidth, const int itemHeight,
                               const QVariant &minValue, const QVariant &maxValue);

    void syncReferenceValue(const quint8 reagentIndex, const bool isMale,
                        QVariant &maxValue, QVariant &minValue, QString &unit);


    void  CreateFolder(QString folderPath);
    void  _NotifyPageSizeKind(Page_Size page_type, QPdfWriter *pWriter);
    //void  InsertReagentName(quint8 Index,QStringList &liststr);
    void  switchreagtestdata(QMap<quint8,QString> _data, QMap<quint8, QStringList> &_outdata_);


private:

    Page_Size m_page_type;
    QPixmap m_pixmap;
    QString m_sampleid;
    QString  m_samplename;
    QString m_sampleSex;
    QString m_sampleBednum;
    quint8 m_sampleAges;
    QString m_sampledepartment; //科室
    QString m_samplebarcode;
    QString m_refertithedoctor; //送检医生
    QString m_reviewdoctors; //审核医生
    QVariant m_testReagResult;

    QTextEdit *textEdit;
    QString m_pdfPath;

};

#endif // PRINTTHEREPORT_H

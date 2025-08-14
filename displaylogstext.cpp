#include "displaylogstext.h"
#include "ui_displaylogstext.h"
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDir>
#include <QElapsedTimer>
#include <QGroupBox>
#include <QPainter>
#include <QTextCodec>
#include "quiutils.h"
#include "QsLog/include/QsLog.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

displayLogsText::displayLogsText(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::displayLogsText)
{
    ui->setupUi(this);
    bgmcColor.setRgb(187, 186 ,183);
    setWindowFlags(Qt::FramelessWindowHint);

    QPixmap pixmapmouse(":/Picture/test_hover.png");
    QCursor cursor(pixmapmouse);
    this->setCursor(cursor);

    QPalette pe;
    pe.setColor(QPalette::WindowText,QColor(0 ,0 ,0));
    ui->Title_TEXT->setPalette(pe);
    ui->Title_TEXT->setText(tr("日志"));

    ui->label_showImage->setFixedSize(32,32);
    QPixmap *pixmap = new QPixmap(":/Picture/Login.png");
    pixmap->scaled(ui->label_showImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_showImage->setScaledContents(true);
    ui->label_showImage->setPixmap(*pixmap);
    delete pixmap;
    pixmap = nullptr;


    QLOG_TRACE()<<"构造函数,日志设置";
    QStringList LogLevelText;
    LogLevelText.push_back("Trace跟踪最低等级,用于打开所有日志记录");
    LogLevelText.push_back("Debug调试,打印一些细粒度调试运行信息");
    LogLevelText.push_back("Info信息,打印粗粒度信息,突出强调程序的运行过程");
    LogLevelText.push_back("Warn警告,表明会出现潜在错误的情形");
    LogLevelText.push_back("Error指出虽然发生错误,但仍然不影响系统的继续运行");
    LogLevelText.push_back("Fatal致命的,发生严重错误,将导致应用程序的退出");
    ui->progressBar_loading->setMaximum(100);  // 最大值
    ui->progressBar_loading->setValue(0);
    _initStyle();
}

displayLogsText::~displayLogsText()
{
    delete ui;
    QLOG_DEBUG()<<"析构日志查看对话框";
}

void displayLogsText::_initStyle()
{
    _mminshow = false;
    connect(ui->Close_Log,&QToolButton::clicked,this,[=](){
       close();
    });
    connect(ui->toolButton_showmax,&QToolButton::clicked,this,[=](){
        if(_mminshow == false){
            showMaximized();
            ui->toolButton_showmax->setIcon(QIcon(":/Picture/icon_title/已经最大化.png"));
            _mminshow = true;
        }else{
            showNormal();
            ui->toolButton_showmax->setIcon(QIcon(":/Picture/icon_title/最大化.png"));
            _mminshow = false;
        }
    });

    connect(ui->toolButton_update,&QToolButton::clicked,this,[=](){
        _loadLogFile();
    });

     connect(ui->toolButton_clean,&QToolButton::clicked,this,[=](){
         QString file_path = QCoreApplication::applicationDirPath() + "/log.txt";
         QFile logFile(file_path);
         logFile.open(QFile::WriteOnly|QFile::Truncate);
         logFile.close();
         ui->textBrowser_showlog->clear();
     });
}


void displayLogsText::mousePressEvent(QMouseEvent *event)
{
    event->accept();

    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = event->pos();

    }
}
void displayLogsText::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QRect desktopRc = QApplication::desktop()->availableGeometry();

        QPoint curPoint = event->globalPos() - mouseStartPoint;

        if (event->globalY() > desktopRc.height())
        {
            curPoint.setY(desktopRc.height() - mouseStartPoint.y());
        }
        move(curPoint);
    }

    QWidget::mouseMoveEvent(event);
}
void displayLogsText::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();

    }
    QWidget::mousePressEvent(event);
}
void displayLogsText::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor colorBackGround = bgmcColor;
    p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setBrush(colorBackGround);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}
void displayLogsText::closeEvent(QCloseEvent *event)
{
    event->accept();
    close();
}


void displayLogsText::_loadLogFile()
{
    ui->textBrowser_showlog->clear();
    ui->label_showtime->clear();
    ui->progressBar_loading->setValue(0);
    QDateTime datetime_ = QDateTime::currentDateTime();
    QString finder_file = datetime_.toString("yyyyMMdd");

    QString logPath =  finder_file + "logs.txt";

    QString file_path = QString("%1/%2").arg(QString(QDir(QCoreApplication::applicationDirPath()).filePath("suoweiLogFile")))
    .arg(logPath);


    QFile logFile(file_path);
    if(!logFile.exists()){
        QLOG_ERROR()<<"日志文件不存在!"<<endl;
        return;
    }
    if(!logFile.open(QIODevice::ReadOnly)){
        QLOG_ERROR()<<"读取LOG文件异常"<<logFile.errorString()<<endl;
        return;
    }

    QElapsedTimer lost_time;
    lost_time.start();
    QQueue<QString> log_queue;
    QString read_data ="";

    QTextStream out(&logFile);
	out.setCodec("UTF-8");
    while(!out.atEnd()){
        read_data = out.readLine();
        log_queue.enqueue(read_data);
    }

    int _size = log_queue.size();
    int n = 1;
    QQueue<QString>::iterator it;
    for (it = log_queue.begin(); it != log_queue.end(); ++it) {
		QString log_data = (*it);
        _appendtextshow(log_data);
        double pro_ = (n*100.00)/_size;
        ui->progressBar_loading->setValue(pro_);
        ui->progressBar_loading->setFormat(QString("当前进度为: %1%").arg(QString::number(pro_, 'f', 1)));
        n++;
    }

    for(const QString &logstr : log_queue){
       _appendtextshow(logstr);
       double pro_ = (n*100.00)/_size;
       ui->progressBar_loading->setValue(pro_);
       ui->progressBar_loading->setFormat(QString("当前进度为：%1%").arg(QString::number(pro_, 'f', 1)));
       n++;
    }

    QString qfile_lost_time = "读取log文件耗时: ";
    qfile_lost_time += QString::number(lost_time.elapsed());
    qfile_lost_time += "毫秒";
    ui->label_showtime->setText(qfile_lost_time);
    logFile.close();

    return;
}

void displayLogsText::_appendtextshow(const QString logdata)
{
    ui->textBrowser_showlog->append("<font color=\"#8470FF\">"+ logdata + "</font>");
}




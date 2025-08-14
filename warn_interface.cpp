#pragma execution_character_set("utf-8")

#include "warn_interface.h"
#include "ui_warn_interface.h"
#include <QDesktopWidget>

warn_interface::warn_interface(QString titlestr, QString  warmtext,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::warn_interface)
{
    ui->setupUi(this);
    setMouseTracking(true);

    this->setWindowTitle("提示");
    this->setWindowIcon(QIcon(":/Picture/suowei.png"));

    ui->label_icon->setFixedSize(32,32);
   
    QPixmap pixmapmouse(":/Picture/test_hover.png");
    QCursor cursor(pixmapmouse);
    this->setCursor(cursor);

    ui->label_name->setText(titlestr);
    ui->label_warmtext->setText(warmtext);
    setWindowFlags(Qt::Tool |
                   Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint);
    this->setConnectBtn();
    this->InitStyle();
    m_myborder = new MyBorderContainer(this);
}

warn_interface::~warn_interface()
{
    delete ui;
}

void warn_interface::showTransientWarning(const QString& title, const QString& message) {
    warn_interface* warn = new warn_interface(title, message);
    warn->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint);
    warn->setAttribute(Qt::WA_DeleteOnClose);
    warn->setWindowModality(Qt::NonModal);
    warn->show();
    warn->raise();
    warn->activateWindow();
}

void warn_interface::settitle(QString title_)
{
    ui->label_name->setText(title_);
}

void warn_interface::replaceSupplyIndex(const int Index)
{
    m_indexSuppiles = Index;
}

void warn_interface::setremtext(QString outputText)
{
     ui->label_warmtext->setText(outputText);
}

void warn_interface::setConnectBtn(){
    connect(ui->toolButton_min,&QToolButton::clicked,this,[=](){
        this->showMinimized();
    });

	connect(ui->toolButton_max, &QToolButton::clicked, this, [=]() {
		onBtnMenuMaxClicked();
	});
       

    connect(ui->toolButton_close,&QToolButton::clicked,this,[=](){
        emit this->makesure(-1,ui->label_name->text());
        close();
    });
}
void warn_interface::onBtnMenuMaxClicked( )
{
	if (max) {
		this->setGeometry(location);
		ui->toolButton_max->setToolTip("最大化");
		ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/最大化.png"));
	}
	else {
		QDesktopWidget* desktop = QApplication::desktop();
		int N = desktop->screenCount();
		if (N == 2)
		{
			location = this->geometry();
			this->setGeometry(desktop->screenGeometry(1));
			ui->toolButton_max->setToolTip("还原");
			ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/已经最大化.png"));

		}
		else {
			location = this->geometry();
			this->setGeometry(qApp->desktop()->availableGeometry());
			ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/已经最大化.png"));
			ui->toolButton_max->setToolTip("还原");
		}
	}
	max = !max;
}

void warn_interface::InitStyle()
{
    max = false;
    this->location = this->geometry();
    mousePressed = false;

}

bool warn_interface::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->onBtnMenuMaxClicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void warn_interface::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void warn_interface::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void warn_interface::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void warn_interface::on_toolButton_clicked()
{
    emit this->makesure(m_indexSuppiles,ui->label_name->text());
    this->close();
}

void warn_interface::closeEvent(QCloseEvent *event)
{
    event->accept();
    delete this;
}


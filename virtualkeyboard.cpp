#include "virtualkeyboard.h"
#include "ui_virtualkeyboard.h"
#include <QShortcut>
#include <QDebug>

Virtualkeyboard * Virtualkeyboard::_instance = nullptr;

Virtualkeyboard::Virtualkeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Virtualkeyboard)
{
    ui->setupUi(this);

    Init("blue",20);

    ui->Btnclear->setFocus();

    ui->Btnclear->setShortcut(QKeySequence::InsertParagraphSeparator);

    ui->Btnclear->setShortcut(Qt::Key_Enter);

    ui->Btnclear->setShortcut(Qt::Key_Return);
}

Virtualkeyboard::~Virtualkeyboard()
{
    delete ui;
}

void Virtualkeyboard::Init(QString style, int fontSize)
{
    //设置窗口无边框且窗口显示在最顶层
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint );

    isFirst = true; //是否首次加载
    isPressBackBtn = false; //是否长按退格键
    //退格键定时器
    backBtnTimert = new QTimer(this);
    connect(backBtnTimert, SIGNAL(timeout()), this, SLOT(reClicked()));
    currentWidget = nullptr;//当前焦点的对象

    //输入法面板字体大小,如果需要更改面板字体大小,该这里即可
    this->currentFontSize = fontSize;

    //如果需要更改输入法面板的样式,改变style这个形式参数即可
    //blue--淡蓝色  dev--dev风格  black--黑色  brown--灰黑色  lightgray--浅灰色  darkgray--深灰色  gray--灰色  silvery--银色
    this->ChangeStyle(style);

    //初始化小键盘上各按键属性
    ui->Btn0->setProperty("btnNum", true);
    ui->Btn1->setProperty("btnNum", true);
    ui->Btn2->setProperty("btnNum", true);
    ui->Btn3->setProperty("btnNum", true);
    ui->Btn4->setProperty("btnNum", true);
    ui->Btn5->setProperty("btnNum", true);
    ui->Btn6->setProperty("btnNum", true);
    ui->Btn7->setProperty("btnNum", true);
    ui->Btn8->setProperty("btnNum", true);
    ui->Btn9->setProperty("btnNum", true);
    ui->Btnpoint->setProperty("btnNum",true);
    ui->Btndelete->setProperty("btnOther", true);

    //链接小键盘上各数字键与功能键的点击信号到点击槽函数上
    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton * b, btn)
    {
        connect(b, SIGNAL(clicked()), this, SLOT(btn_clicked()));
    }

    //绑定全局改变焦点信号槽
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
                this, SLOT(focusChanged(QWidget *, QWidget *)));

    //绑定按键事件过滤器
    qApp->installEventFilter(this);

}

void Virtualkeyboard::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
    //qDebug() << "oldWidget:" << oldWidget << " nowWidget:" << nowWidget;
    if (nowWidget != nullptr && !this->isAncestorOf(nowWidget))
    {
        #ifndef __arm__
        if (oldWidget == nullptr && !isFirst)
        {
            return;
        }
        #endif

        isFirst = false;
        if (nowWidget->inherits("QLineEdit"))
        {
            currentLineEdit = static_cast<QLineEdit *>(nowWidget);
            currentEditType = "QLineEdit";
            this->setVisible(true);
        }
        else
        {
            currentWidget = nullptr;
            currentLineEdit = nullptr;
            currentEditType = "";
            this->setVisible(false);
        }

        QRect rect = nowWidget->rect();
        QPoint pos = QPoint(rect.left(), rect.bottom() + 2);
        pos = nowWidget->mapToGlobal(pos);
        this->setGeometry(pos.x(), pos.y(), this->width(), this->height());
    }

    Q_UNUSED(oldWidget);//未使用参数
}

bool Virtualkeyboard::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        //确保每次点击输入栏都弹出虚拟键盘
        if (currentEditType == "QLineEdit")
        {
            if (obj != ui->Btnclear)
            {
                this->setVisible(true);
            }
            btnPress = static_cast<QPushButton *>(obj);

            if (checkPress())
            {
                isPressBackBtn = true;
                backBtnTimert->start(500);
            }
        }

        return false;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        btnPress = static_cast<QPushButton *>(obj);

        if (checkPress())
        {
            isPressBackBtn = false;
            backBtnTimert->stop();
        }

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

//校验当前长按的按钮
bool Virtualkeyboard::checkPress()
{
    //只有属于数字键盘的合法按钮才继续处理
    bool num_ok = btnPress->property("btnNum").toBool();
    bool other_ok = btnPress->property("btnOther").toBool();
    if (num_ok || other_ok)
    {
        return true;
    }

    return false;
}

//定时器处理退格键
void Virtualkeyboard::reClicked()
{
    if (isPressBackBtn)
    {
        backBtnTimert->setInterval(30);
        btnPress->click();
    }
}

//小键盘按键处理槽函数
void Virtualkeyboard::btn_clicked()
{
    //如果当前焦点控件类型为空,则返回不需要继续处理
    if (currentEditType == "")
    {
        return;
    }

    QPushButton *btn = static_cast<QPushButton *>(sender());
    QString objectName = btn->objectName();
    if (objectName == "Btndelete")
    {
        this->deleteValue();
    }
    else if (objectName == "Btnclear")
    {
        this->clearValue();
    }
    else
    {
        QString value = btn->text();
        this->insertValue(value);
    }
}

//插入值到当前焦点控件
void Virtualkeyboard::insertValue(QString value)
{
    if (currentEditType == "QLineEdit")
    {
        currentLineEdit->insert(value);
    }
}

//删除当前焦点控件的一个字符
void Virtualkeyboard::deleteValue()
{
    if (currentEditType == "QLineEdit")
    {
        currentLineEdit->backspace();
    }
}

//清空当前焦点控件的所有字符
void Virtualkeyboard::clearValue()
{
    if (currentEditType == "QLineEdit")
    {
        currentLineEdit->clear();
    }
    CloseWidget();
}

//改变样式
void Virtualkeyboard::ChangeStyle(QString currentStyle)
{
    if (currentStyle == "blue")
    {
        changeStyle("#DEF0FE", "#C0DEF6", "#C0DCF2", "#386487");
    }
    else if (currentStyle == "dev")
    {
        changeStyle("#C0D3EB", "#BCCFE7", "#B4C2D7", "#324C6C");
    }
    else if (currentStyle == "gray")
    {
        changeStyle("#E4E4E4", "#A2A2A2", "#A9A9A9", "#000000");
    }
    else if (currentStyle == "lightgray")
    {
        changeStyle("#EEEEEE", "#E5E5E5", "#D4D0C8", "#6F6F6F");
    }
    else if (currentStyle == "darkgray")
    {
        changeStyle("#D8D9DE", "#C8C8D0", "#A9ACB5", "#5D5C6C");
    }
    else if (currentStyle == "black")
    {
        changeStyle("#4D4D4D", "#292929", "#D9D9D9", "#CACAD0");
    }
    else if (currentStyle == "brown")
    {
        changeStyle("#667481", "#566373", "#C2CCD8", "#E7ECF0");
    }
    else if (currentStyle == "silvery")
    {
        changeStyle("#E1E4E6", "#CCD3D9", "#B2B6B9", "#000000");
    }
}

//改变小键盘样式
void Virtualkeyboard::changeStyle(QString topColor, QString bottomColor, QString borderColor, QString textColor)
{
    QStringList qss;
    qss.append(QString("QWidget#frmNum{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(topColor).arg(bottomColor));
    qss.append("QPushButton{padding:5px;border-radius:3px;}");
    qss.append(QString("QPushButton:hover{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(topColor).arg(bottomColor));
    qss.append(QString("QLabel,QPushButton{font-size:%1pt;color:%2;}")
               .arg(currentFontSize).arg(textColor));
    qss.append(QString("QPushButton#btnPre,QPushButton#btnNext,QPushButton#btnClose{padding:5px;}"));
    qss.append(QString("QPushButton{border:1px solid %1;}")
               .arg(borderColor));
    qss.append(QString("QLineEdit{border:1px solid %1;border-radius:5px;padding:2px;background:none;selection-background-color:%2;selection-color:%3;}")
               .arg(borderColor).arg(bottomColor).arg(topColor));
    this->setStyleSheet(qss.join(""));
}
void Virtualkeyboard::ShowPointBtnSlot(bool showPoint)
{
    if(showPoint)
    {
        ui->Btnpoint->show();
    }
    else
    {
        ui->Btnpoint->hide();
    }
}
void Virtualkeyboard::CloseWidget()
{
	if (currentWidget != nullptr &&  currentLineEdit != nullptr)
	{
		currentWidget = nullptr;
		currentLineEdit = nullptr;
		currentEditType = "";
	}
    this->setVisible(false);
	this->close();
}

//拖拽操作
void Virtualkeyboard::mousePressEvent(QMouseEvent *event)
{
    event->accept();

    if (event->button() == Qt::LeftButton)
    {

        mouseStartPoint = event->pos();

    }
}

void Virtualkeyboard::mouseMoveEvent(QMouseEvent *event)
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

void Virtualkeyboard::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

#include "workthreadreadtsakfile.h"
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>


WorkThreadReadTsakFile::WorkThreadReadTsakFile(QObject *parent):QThread(parent)
{
    //qDebug()<<"创建读取任务文件线程:"<<QThread::currentThreadId();
}
WorkThreadReadTsakFile::~WorkThreadReadTsakFile()
{
    //qDebug()<<"关闭读取任务文件线程:"<<QThread::currentThreadId();
}

void WorkThreadReadTsakFile::run()
{

}

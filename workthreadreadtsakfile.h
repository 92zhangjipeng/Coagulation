#ifndef WORKTHREADREADTSAKFILE_H
#define WORKTHREADREADTSAKFILE_H
#include <QMutex>
#include <QTableWidget>
#include <QThread>

#define  CHECK_ROW  0
#define  SAMLPE_NAME 1
#define  HEIGHT_DATA 2
#define  RICHBLOOD_HOLE 3
#define  PROJECT_ITEM   4

class WorkThreadReadTsakFile :public QThread
{
    Q_OBJECT
public:
    WorkThreadReadTsakFile(QObject* parent = nullptr);
    ~WorkThreadReadTsakFile();
private:
    virtual void run();
public slots:

signals:


private:
   bool m_loaded = false;
   QMutex mutex;
};

#endif // WORKTHREADREADTSAKFILE_H

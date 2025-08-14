#ifndef THREADINFOVIRTUALBASIC_H
#define THREADINFOVIRTUALBASIC_H

#include <QString>



using namespace std;
class threadInfoVirtualBasic
{
public:
    threadInfoVirtualBasic(QString,int);
    virtual  ~threadInfoVirtualBasic(){}
    void SetFunction(QString &);
    QString GetFunction();
    void SetDataLength(int &);
    int GetDataLength();
    virtual QString RecveFunctionCode() = 0; //纯虚函数
private:
    int m_DataLength;
    QString m_FunctionCode;
};

#endif // THREADINFOVIRTUALBASIC_H

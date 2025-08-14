#ifndef MACHINEREADRUNNABLE_H
#define MACHINEREADRUNNABLE_H

#include "suoweiserialport.h"
#include "testing.h"
#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include "QsLog/include/QsLog.h"


class MachineReadRunnable : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit MachineReadRunnable(QObject *parent = nullptr);
    ~MachineReadRunnable();

protected:

signals:

public slots:

private:


};

//读取模组信息
class ReadModulInfo : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit ReadModulInfo(int id,QObject *parent = nullptr);
    ~ReadModulInfo();
protected:
    void run() override;  
public slots:



signals:
    void  ReadMachineState(const QByteArrayList &dataList,
                           const QString &info);  //读取模组数据消息
public:
    void sycnpSerialPort(SuoweiSerialPort *pSerialpotr); //同步串口线程
    void StopThread();

private:
    QByteArrayList m_modulcode;  //模组数据
    SuoweiSerialPort *m_SerialPort = nullptr;
    int m_idthread;
    bool mstop_;
    QMutex pauseLock;
};


///////////////////////////发送机器运动命令线程/////////////////////////////////////
class SendMachineSportInfo : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit SendMachineSportInfo(int id, QObject *parent = nullptr);
    ~SendMachineSportInfo();
     void sycnSerialclass(SuoweiSerialPort * Port);
protected:
    void run() override;

public slots:
    void willSendcode(const QByteArrayList codearry, const bool);


private:
    QByteArrayList m_machinespotr;  //数据
    SuoweiSerialPort *m_SerialPort_ = nullptr;
    int     m_idthread;
    QQueue<QByteArrayList> m_cachecode; //容量满时候缓存命令
signals:
    void  sendMachineState(const QByteArrayList &dataList,
                           const QString &info);

};



////////////////测高条形码//////////////////////
/// \brief The Heightmeasurement class
///
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <QMessageBox>
#include <io.h>
#include <string>
#include <QRect>
#include <QWaitCondition>
#include "correct_data.h"



class Heightmeasurement : public QObject , public QRunnable
{
    Q_OBJECT
public:
    explicit Heightmeasurement(int id, QObject *parent = nullptr);
    ~Heightmeasurement();
protected:
    void run() override;
private slots:
    /*
    * 创建存贮测高图片文件夹
    */
    void    AlreadyFilebox();

private:

    /** 蓝色块
    * @brief BaseLineFinded
    * @param LefthalfBlueModelled
    * @param DisplayImage
    * @return
    */
    bool BaseLineFinded(cv::Mat LefthalfBlueModelled, cv::Mat &DisplayImage);

	/** 图像左半边蓝色标准物的Top像素点 和 图像比例
	* @br  LefthalfBlueModelled 截取的左半边图像
	*/
    int LefthalfBlueModelledTop(cv::Mat LefthalfBlueModelled, cv::Mat &DisplayImage);

    /**
    * @brief FindBloodTube 找有血样的试管宽度
    */
    int FindBloodTube(bool FindYellowLinque, cv::Mat image_findtube);

    /**
    * @brief AdjustHSVvalue
    * @param HSVimageTrackBar发生改变调整HSV值
    */
    void AdjustHSVvalue(cv::Mat HSVimage, int index);

    /**
    * @brief OutPrintTestHigh
    * @param OutSaveImage
    * @param savepath
    */
    void OutPrintTestHigh(cv::Mat OutSaveImage,string savepath);

    /**
    * @brief TestHeightFailed
    * @param savepath
    */
    void TestHeightFailed(string savepath, cv::Mat OutSaveImage);

    /**
    * @brief InitPara 初始化参数
    */
    void InitPara();

public slots:
    /*
    *  判断打开摄像头内、外部
    */
   void  RecvOpenInstrumentCamera(const quint8 CameraPositioning);

private:
   int m_idthread;
   QString ImagePath;
   quint8  m_OpenCameraPosition; //打开摄像头位置
   cv::VideoCapture  mcap;

   QString m_pxdpieverymm;      //像素比例一像素(px/mm)
   int m_BlueModelled_top;      //蓝色参照物顶点
   int m_BlueModelled_bottom;   //蓝色参照物底部
   int m_BlueModelled_right;    //蓝色参照物最右侧

   QPoint m_tubeLeft_top;     //黑色血样左上角
   QPoint m_tube_Right_bottom;
   QPoint m_tube_Right_top;

   int m_Yellow_Top;   //PPP顶部
   int m_Yellow_bottom; //黄色底部Y
   int m_Yellow_width;  //黄色宽

   const int Image_Width =  480;
   const int Image_Height = 640;



signals:
   void FinishTestHigh(QString Path,double);
   void Testheightfinish(const bool );
   void RetestHeight(const QString Path,const double); //重测高度
};





////////////////////////通讯配置机器参数//////////////////////////////
class ConfigureWriteParameter : public QObject ,public QRunnable
{
   Q_OBJECT
public:
   explicit ConfigureWriteParameter(int id, QObject *parent = nullptr);
    ~ConfigureWriteParameter();
    void sycnSerialclass(SuoweiSerialPort * port);
protected:
    void run() override;

public slots:
    void WriteCodearray(QByteArrayList);
    void slotDelayed_sending(const QByteArrayList , int delayedmS, bool needDelayed);
    void slotWhileReadChannelParam(const int Fromchannel);
private:
	void Delay_MSec_Suspend(unsigned int msec);
private:
   QByteArrayList m_machinespotr;  //数据
   SuoweiSerialPort *Serial_port = nullptr;
   int m_DelayedMS;
   bool m_bNeedDelayed;
   int m_id;
   int m_BeginChannel;


signals:
    void  WriteMachineAction(const QByteArrayList);


};





#endif // MACHINEREADRUNNABLE_H

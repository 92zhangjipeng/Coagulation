#pragma execution_character_set("utf-8")

#include "machinereadrunnable.h"
#include "cglobal.h"
#include <QElapsedTimer>
#include <QDir>
#include "ConnectionUtil.h"
#include "loadequipmentpos.h"


QMutex  mlockmutex;

MachineReadRunnable::MachineReadRunnable(QObject *parent) : QObject(parent)
{

}

MachineReadRunnable::~MachineReadRunnable()
{
    
}









/////////////////////模组线程 /////////////////////////////
/// \brief ReadModulInfo::ReadModulInfo
/// \param id
/// \param parent
///
ReadModulInfo::ReadModulInfo(int  id, QObject *parent) : QObject(parent)
{
    mstop_ = false;
    m_idthread = id; 
}
ReadModulInfo::~ReadModulInfo()
{
    QLOG_DEBUG()<<"析构线程池模组线程ID:"<<m_idthread;
}

void ReadModulInfo::run()
{
    while(!mstop_)
    {
        pauseLock.lock();
        if(m_modulcode.size() == 3)
        {
           emit ReadMachineState(m_modulcode,"获取模组数据");
//           for(int i = 0 ;i <m_modulcode.size(); i++)
//           {
//               QString framedata  = m_modulcode.at(i).toHex(' ').trimmed().toUpper();
//               QLOG_DEBUG()<<"读取模组数据："<< framedata<<endl;
//           }
        }
        QThread::msleep(100);
        pauseLock.unlock();
		if (mstop_)
				break;
    }
}

void ReadModulInfo::sycnpSerialPort(SuoweiSerialPort *pSerialpotr)
{
    m_SerialPort = pSerialpotr;
    connect(this, &ReadModulInfo::ReadMachineState,
            m_SerialPort, &SuoweiSerialPort::slotwrite_instructions_group);
    quint8 equipmentKind = 0;
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmentKind);
    m_modulcode.clear();
    m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(0, 0, 0, false));
    switch(equipmentKind)
    {
        case KS600:
        break;
        case KS800:
            m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(1, 0, 0, false));
        break;
        case KS1200:
            m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(1, 0, 0, false));
            m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(2, 0, 0, false));
        break;
    }

}

QByteArray intToByte(int leng,int number)
{
    QByteArray abyte0;

    abyte0.resize(leng);

    if(leng == 1)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
    }
    else if(leng == 2)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
        abyte0[1] = (uchar) ((0x000000ff & number) >> 8);
    }
    else if(leng == 4)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
        abyte0[1] = (uchar) ((0x0000ff00 & number) >> 8);
        abyte0[2] = (uchar) ((0x00ff0000 & number) >> 16);
        abyte0[3] = (uchar) ((0xff000000 & number) >> 24);
    }

    return abyte0;
}




void ReadModulInfo::StopThread()
{
	mstop_ = true;
  
}


///////////////////////////发送机器运动命令线程/////////////////////////////////////
/// \brief SendMachineSportInfo::SendMachineSportInfo
/// \param id
/// \param parent
///
SendMachineSportInfo::SendMachineSportInfo(int  id, QObject *parent) : QObject(parent)
{
    m_idthread = id;
    m_cachecode.clear();
}
SendMachineSportInfo::~SendMachineSportInfo()
{
    QLOG_TRACE()<<"退出发送机器命令线程ID:"<<m_idthread;
}
void SendMachineSportInfo::sycnSerialclass(SuoweiSerialPort * Port)
{
    if(m_SerialPort_) return;
    if(m_SerialPort_ == nullptr)
    {
        m_SerialPort_ = Port;
        connect(this, &SendMachineSportInfo::sendMachineState,
                m_SerialPort_, &SuoweiSerialPort::slotwrite_instructions_group);
    }
    return;
}

void SendMachineSportInfo::willSendcode(const QByteArrayList codearry, const bool /*Delaytime*/)
{
    QMutexLocker locker(&mlockmutex);
    m_machinespotr = codearry;
    return;
}


void SendMachineSportInfo::run()
{
    QByteArrayList SentCommand;
    SentCommand.clear();
    if( m_cachecode.isEmpty())
    {
        SentCommand = m_machinespotr;
        emit sendMachineState(SentCommand,"未定义指令");
        QThread::msleep(10);
    }
    else if(!m_cachecode.isEmpty())
    {
        SentCommand = m_cachecode.dequeue();
        emit sendMachineState(SentCommand,"未定义指令"); //出队
        QThread::msleep(10);
        m_cachecode.enqueue(m_machinespotr);
    }

}


/////////////////////////测高&&条形码////////////////////////////////////////
#include <QTime>
#include <QVector>
int t_index = 0;
int LH = 45;
int LS = 142;
int LV = 51;
int HH = 180;
int HS = 255;
int HV = 225;

Heightmeasurement::Heightmeasurement(int  id, QObject *parent) : QObject(parent)
{
    m_idthread = id;

    AlreadyFilebox();

    InitPara();
}

Heightmeasurement::~Heightmeasurement()
{
    QLOG_DEBUG()<<"退出测高线程ID: %1"<<m_idthread;
}
/* 判断打开摄像头内、外部*/
void  Heightmeasurement::RecvOpenInstrumentCamera(const quint8 CameraPositioning)
{
    switch(CameraPositioning)
    {
        case OPEN_DEFAULT:
                m_OpenCameraPosition = OPEN_DEFAULT;
                QLOG_DEBUG()<<"仪器电脑只有一个摄像头在线==打开默认摄像头";
        break;
        case OPEN_EXTERNAL:
                m_OpenCameraPosition = OPEN_EXTERNAL;
                QLOG_DEBUG()<<"仪器电脑大于一个摄像头在线==打开外置摄像头";
        break;
    default:
         m_OpenCameraPosition = OPEN_DEFAULT;
        break;
    }
    return;
}

/* 创建存贮测高图片文件夹*/
void  Heightmeasurement::AlreadyFilebox()
{
    ImagePath = QCoreApplication::applicationDirPath() + "/FindHighImage";
    QDir dir(ImagePath);
    if (!dir.exists())
    {
        bool ismkdir = QDir().mkdir(ImagePath);
        if (!ismkdir)
            QLOG_DEBUG() << "创建测高图片文件夹失败" << endl;
        else
            QLOG_DEBUG() << "创建测高图片文件夹成功" << endl;
    }
}

/*
* 图片旋转
*/
int pinwheel(cv::Mat cutimage, int degree ,cv::Mat &OutMat)
{
    cv::Mat dstimg;
    cv::Mat rotimg;
    int row, col;
    cv::Point2f srcTri[3];
    cv::Point2f dstTri[3];
    cv::Mat warpMat(2, 3, CV_32FC1);
    cv::Mat srcimg = cutimage;
    if (!srcimg.data)
    {
        qDebug() << "读取错误" << endl;
        return false;
    }
    row = srcimg.rows;
    col = srcimg.cols;

    srcTri[0] = cv::Point2f(0, 0);
    srcTri[1] = cv::Point2f(col - 1, 0);
    srcTri[2] = cv::Point2f(0, row - 1);

    dstTri[0] = cv::Point2f(col*0.0, row*0.33);
    dstTri[1] = cv::Point2f(col*0.85, row*0.25);
    dstTri[2] = cv::Point2f(col*0.15, row*0.7);

    warpMat = cv::getAffineTransform(srcTri, dstTri);

    cv::warpAffine(srcimg, dstimg, warpMat, dstimg.size());

    cv::Point2f center(col / 2, row / 2);
    double angle = degree*CV_PI / 180; //转换弧度
    double scale = 1;
    double a = sin(angle);
    double b = cos(angle);
    int width = col;
    int height = row;

    int rewidth = int(height*fabs(a) + width*fabs(b));
    int reheight = int(width*fabs(a) + height*fabs(b));
    cv::Mat rot = cv::getRotationMatrix2D(center, degree, scale);
    double *p = (double*)rot.data;
    p[2] += (rewidth - width) / 2;
    p[5] += (reheight - height) / 2;
    cv::warpAffine(srcimg, rotimg, rot, cv::Size(rewidth, reheight), 0, 0, 0);
    //cv::imshow("旋转后", rotimg);
    OutMat = rotimg;
    return true;
}


int mAdjustvalue = 0;
void ChangeBrightValue(int pos, void* userdata)
{
    cv::Mat src = *(cv::Mat*)(userdata);
    cv::Mat dst;
    qDebug()<<"阈值调整"<<pos;
    mAdjustvalue = pos;
    cv::threshold(src,dst,pos,255,cv::THRESH_BINARY_INV);
    cv::imshow("123",dst);

}

void onLH(int,void* userdata){
    cv::Mat src = *(cv::Mat*)(userdata);
    cv::Mat dst;
    cv::inRange(src,cv::Scalar(LH,LS,LV),cv::Scalar(HH,HS,HV),dst);
    switch(t_index)
    {
        case 0:  cv::imshow("0000",dst); break;
        case 1:  cv::imshow("0001",dst); break;
        case 2:  cv::imshow("0002",dst); break;
        case 3:  cv::imshow("0003",dst); break;
    }

}
void onLS(int, void* userdata)
{
    cv::Mat src = *(cv::Mat*)(userdata);
    cv::Mat dst;
    cv::inRange(src,cv::Scalar(LH,LS,LV),cv::Scalar(HH,HS,HV),dst);
    switch(t_index)
    {
        case 0:  cv::imshow("0000",dst); break;
        case 1:  cv::imshow("0001",dst); break;
        case 2:  cv::imshow("0002",dst); break;
        case 3:  cv::imshow("0003",dst); break;
    }
}
void onLV(int, void* userdata)
{
    cv::Mat src = *(cv::Mat*)(userdata);
    cv::Mat dst;
    cv::inRange(src,cv::Scalar(LH,LS,LV),cv::Scalar(HH,HS,HV),dst);
    switch(t_index)
    {
        case 0:  cv::imshow("0000",dst); break;
        case 1:  cv::imshow("0001",dst); break;
        case 2:  cv::imshow("0002",dst); break;
        case 3:  cv::imshow("0003",dst); break;
    }
}
void onHH(int, void* userdata)
{
    cv::Mat src = *(cv::Mat*)(userdata);
    cv::Mat dst;
    cv::inRange(src,cv::Scalar(LH,LS,LV),cv::Scalar(HH,HS,HV),dst);
    switch(t_index)
    {
        case 0:  cv::imshow("0000",dst); break;
        case 1:  cv::imshow("0001",dst); break;
        case 2:  cv::imshow("0002",dst); break;
        case 3:  cv::imshow("0003",dst); break;
    }
}
void onHS(int, void* userdata)
{
    cv::Mat src = *(cv::Mat*)(userdata);
    cv::Mat dst;
    cv::inRange(src,cv::Scalar(LH,LS,LV),cv::Scalar(HH,HS,HV),dst);
    switch(t_index)
    {
        case 0:  cv::imshow("0000",dst); break;
        case 1:  cv::imshow("0001",dst); break;
        case 2:  cv::imshow("0002",dst); break;
        case 3:  cv::imshow("0003",dst); break;
    }
}
void onHV(int, void* userdata)
{
    cv::Mat src = *(cv::Mat*)(userdata);
    cv::Mat dst;
    cv::inRange(src,cv::Scalar(LH,LS,LV),cv::Scalar(HH,HS,HV),dst);
    switch(t_index)
    {
        case 0:  cv::imshow("0000",dst); break;
        case 1:  cv::imshow("0001",dst); break;
        case 2:  cv::imshow("0002",dst); break;
        case 3:  cv::imshow("0003",dst); break;
    }
}


/*线程处理函数*/
void Heightmeasurement::run()
{
    std::string savePath;  //临时图像位置
    cv::Mat readImage; //打开图片
    cv::Mat rotatingimage;
    QLOG_DEBUG() << "打开摄像头参数==" << m_OpenCameraPosition<<endl;
    switch (m_OpenCameraPosition)
    {
        case OPEN_DEFAULT:  mcap.open(OPEN_DEFAULT); break;
        case OPEN_EXTERNAL: mcap.open(OPEN_EXTERNAL); break;
        default:
            mcap.open(OPEN_DEFAULT);
        break;
    }
    //设置采集图像大小；具体大小根据相机设置
    mcap.set(cv::CAP_PROP_FRAME_WIDTH, Image_Width);
    mcap.set(cv::CAP_PROP_FRAME_HEIGHT,Image_Height);
    if(!mcap.isOpened())
    {
        QLOG_WARN()<<"摄像头,打开失败!"<<endl;
        return ;
    }
    while(true)
    {
        mcap >> readImage ;
        cv::waitKey(1);
        savePath = QString("%1/%2.jpg").arg(ImagePath).arg("temp").toStdString();
        pinwheel(readImage, 90, rotatingimage); //旋转 -90
        mcap.release();
        break;
    }
    if (!rotatingimage.data || rotatingimage.channels() != 3)
    {
        return;
    }
    cv::Mat temp_image = rotatingimage.clone();
    //cv::imshow("ORIGIN",temp_image);
    //灰度值求阈值


    //AdjustHSVvalue(outdst,0); /*调整HSV*/
    //截取蓝色基准物图像
    int BaseImage_width = Image_Width/3+30;
    cv::Rect LefthalfBlueModelledRcet(10, 0, BaseImage_width, Image_Height );
    cv::Mat  LefthalfBlueModelled_image = temp_image(LefthalfBlueModelledRcet);

    cv::Mat  LighAddBase;
    cv::convertScaleAbs(LefthalfBlueModelled_image,LighAddBase,1,1); //亮度增强
    bool b_baseLinefind = BaseLineFinded(LighAddBase,temp_image);
    if(!b_baseLinefind)
    {
       TestHeightFailed(savePath,temp_image);
       return;
    }
    //cv::imshow("ending",temp_image);
    //血样试管宽
    cv::Rect FindTubeImage_width(m_BlueModelled_right, 0, Image_Width - m_BlueModelled_right, Image_Height);
    cv::Mat image_findtube_width = rotatingimage(FindTubeImage_width);
    //cv::imshow("tubeimage",image_findtube_width);
    cv::Mat outdst;
    cv::convertScaleAbs(image_findtube_width,outdst,2,10);// 对比度 && 亮度增强
    //cv::imshow("changeLigh",outdst);
    if(FindBloodTube(false,outdst) >= 0 && FindBloodTube(true,outdst) >= 0)
    {
        quint8 bloodtop_abs_yellowbottom = qAbs(m_Yellow_bottom - m_tube_Right_top.y()); //血样顶部余黄液体底部绝对值
        if(bloodtop_abs_yellowbottom < 15)
        {
          OutPrintTestHigh(temp_image,savePath);
        }
        else if(m_Yellow_width > 20)
        {
           OutPrintTestHigh(temp_image,savePath);
        }
        else
        {
            TestHeightFailed(savePath,temp_image);
        }
    }
    else
    {
        TestHeightFailed(savePath,temp_image);
    }
    //cv::imshow("ending",temp_image);
    //cv::waitKey();
    return;

    cv::imshow("lfBlueModelled",LefthalfBlueModelled_image);
    if(LefthalfBlueModelledTop(LefthalfBlueModelled_image,temp_image) < 0)
    {
       TestHeightFailed(savePath,temp_image);
       return;
    }
    //截取除蓝色参照物剩余图片==找到试管宽度黑色血样的参数
    cv::Rect FindTubeRcet(m_BlueModelled_right + 10, 0, Image_Width - m_BlueModelled_right, Image_Height );
    cv::Mat image_findtube = outdst(FindTubeRcet);
    //cv::imshow("findblood",image_findtube);
    if(FindBloodTube(false,image_findtube) >= 0 && FindBloodTube(true,image_findtube) >= 0)
    {
        quint8 bloodtop_abs_yellowbottom = qAbs(m_Yellow_bottom - m_tube_Right_top.y()); //血样顶部余黄液体底部绝对值
        if(bloodtop_abs_yellowbottom < 15)
        {
            OutPrintTestHigh(temp_image,savePath);
        }
        else if(m_Yellow_width > 20)
        {
           OutPrintTestHigh(temp_image,savePath);
        }
        else
        {
            TestHeightFailed(savePath,temp_image);
        }
    }
    else
    {
        TestHeightFailed(savePath,temp_image);
    }
    //cv::imshow("ending",temp_image);
    //cv::waitKey();
    return;
}



/**  截取参照物图片部分，增加亮度 阈值 识别区域
 * @brief Heightmeasurement::BaseLineFinded
 * @param BaseImage
 * @param OriginImage
 * @return
 */
bool Heightmeasurement::BaseLineFinded(cv::Mat BaseImage, cv::Mat &OriginImage)
{
    cv::Mat gray;
    cv::cvtColor(BaseImage, gray, cv::COLOR_BGR2GRAY);//先转为灰度图
    cv::Mat dst_baseline;

    int posTrackbar = 100;
    int maxValue = 255;
    //cv::Mat graydst;
    //cv::namedWindow("123",cv::WINDOW_AUTOSIZE);
    //cv::cvtColor(LefthalfBlueModelled_image, graydst, cv::COLOR_BGR2GRAY);//先转为灰度图
    //cv::createTrackbar("pos","123",&posTrackbar,maxValue,ChangeBrightValue,&gray);
    //cv::waitKey();

    cv::threshold(gray, dst_baseline, /*mAdjustvalue*/posTrackbar, maxValue, cv::THRESH_BINARY_INV);//反二值化阈值处理
    //cv::imshow("dst_baseline",dst_baseline);
    //cv::waitKey();

    QVector<int> listArea;
    listArea.clear();
    vector<vector<cv::Point>> contours_blue;
    vector<cv::Vec4i> hierarchy_blue;
    cv::findContours(dst_baseline, contours_blue, hierarchy_blue, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point());
    //遍历所有轮廓，绘制缺陷轮廓
    for (int i = 0; i < contours_blue.size(); i++)
    {
        int aero = contourArea(contours_blue[i]);//计算轮廓面积
        listArea.append(aero);
       // qDebug()<< i << "aero:" << aero<<endl;
        //drawContours(BaseImage, contours_blue, i, cv::Scalar(30,120,255), -1, 8, hierarchy_blue);//绘制轮廓
    }
    //cv::imshow("33333333333333333",BaseImage);
    if(listArea.isEmpty())
    {
        return false;
    }
    //最大值
    QVector<int>::iterator maxvalue = std::max_element(std::begin(listArea),std::end(listArea));
    int biggest = *maxvalue;
    auto positionmax = std::distance(std::begin(listArea),maxvalue);//最大值下标
    int posmax = positionmax;
    cv::RotatedRect BlueModelled = cv::minAreaRect(contours_blue[posmax]);
    int BlueModelled_height = BlueModelled.boundingRect().height;
    qDebug()<< "最大值面积"<<biggest << "面积最大值下标:" << posmax<<"参照物高度:"<<BlueModelled_height<<endl;
    if(BlueModelled_height < 100 )
    {
        return false;
    }
    m_BlueModelled_top = BlueModelled.center.y - (BlueModelled.boundingRect().height / 2) + 4; //蓝色参照物顶点
    m_BlueModelled_bottom = BlueModelled.center.y + (BlueModelled.boundingRect().height / 2); //最底部
    m_BlueModelled_right = BlueModelled.center.x + (BlueModelled.boundingRect().width / 2);  //最右侧
    double ratio = (REFERENCE_HEIGHT*100.00 / BlueModelled_height)/100.00;
    m_pxdpieverymm = QString::number(ratio ,'f',2);

    //蓝色基准物顶点top画的黑线
    cv::line(OriginImage, cv::Point(0, m_BlueModelled_top), cv::Point(OriginImage.cols, m_BlueModelled_top),cv::Scalar(0, 0, 0), 1);
    //写下降高度
    double downText = INI_File().GetFixedHigh();
    QString downTextline = QString::number(downText ,'f',2);
    cv::putText(OriginImage, downTextline.toStdString(), cv::Point(0, m_BlueModelled_top - 5),cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255));

    QLOG_DEBUG()<<"参照物最顶点Y:"<< m_BlueModelled_top<<"底部"<<m_BlueModelled_bottom<<"像素比例每像素(px/mm):"<< m_pxdpieverymm<<endl;
    return true;
}





/** 图像左半边蓝色标准物的Top像素点 和 图像比例
*
*/
int Heightmeasurement::LefthalfBlueModelledTop(cv::Mat LefthalfBlueModelled ,cv::Mat &DisplayImage)
{

    cv::Mat mat_knowlength;
    cv::cvtColor(LefthalfBlueModelled, mat_knowlength, cv::COLOR_BGR2HSV);
    // 在HSV空间中定义蓝色
    //cv::Scalar lower_blue = cv::Scalar(100, 80, 52);
    //cv::Scalar upper_blue = cv::Scalar(180, 255, 255);
    cv::Scalar lower_blue = cv::Scalar(100, 38, 46);
    cv::Scalar upper_blue = cv::Scalar(124, 255, 255);
    //调试阈值
    //LH = 100 ;LS = 65;LV =52 ;HH = 124;HS =255; HV =255 ;
    //LH = 96 ;LS = 38;LV =  92 ;HH = 125;HS = 255; HV = 255 ;
    //AdjustHSVvalue(mat_knowlength,1);
    cv::Mat blue_mask;
    // blue_mask为单通道，blue_mask.channels==1
    cv::inRange(mat_knowlength, lower_blue, upper_blue, blue_mask);
    // 将原图像和mask(掩膜)进行按位与
    cv::Mat blue_res;
    // 三通道图像进行单通道掩模操作后，输出图像还是三通道。相当于对三通道都做了掩模。
    cv::bitwise_and(LefthalfBlueModelled, LefthalfBlueModelled, blue_res, blue_mask);
    qDebug()<<"blue_mask.channels=" << blue_mask.channels() <<endl;
    cv::imshow("findBlue",blue_mask);
    vector<vector<cv::Point>> contours_blue;
    vector<cv::Vec4i> hierarchy_blue;
    cv::findContours(blue_mask, contours_blue, hierarchy_blue, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE/*, cv::Point()*/);
    QVector<int> listArea;
    //遍历所有轮廓，绘制缺陷轮廓
    for (int i = 0; i < contours_blue.size(); i++)
    {
        int aero = contourArea(contours_blue[i]);//计算轮廓面积
        listArea.append(aero);
        qDebug()<< i << "aero:" << aero<<endl;
        //drawContours(LefthalfBlueModelled, contours_blue, i, cv::Scalar(30,120,255), -1, 8, hierarchy_blue);//绘制轮廓
    }
    if(listArea.isEmpty())
    {
        return -1;
    }
    //最大值
    QVector<int>::iterator maxvalue = std::max_element(std::begin(listArea),std::end(listArea));
    int biggest = *maxvalue;
    //最大值下标
    auto positionmax = std::distance(std::begin(listArea),maxvalue);
    int posmax = positionmax;
    cv::RotatedRect BlueModelled = cv::minAreaRect(contours_blue[posmax]);

    int BlueModelled_height = BlueModelled.boundingRect().height;
    qDebug()<< "最大值面积"<<biggest << "面积最大值下标:" << posmax<<"参照物高度:"<<BlueModelled_height<<endl;
    if(BlueModelled_height < 100)
    {
        return -1;
    }
    m_BlueModelled_top = BlueModelled.center.y - (BlueModelled.boundingRect().height / 2) + 4; //蓝色参照物顶点
    m_BlueModelled_bottom = BlueModelled.center.y + (BlueModelled.boundingRect().height / 2); //最底部
    m_BlueModelled_right = BlueModelled.center.x + (BlueModelled.boundingRect().width / 2);  //最右侧
    double ratio = (REFERENCE_HEIGHT*100.00 / BlueModelled_height)/100.00;
    m_pxdpieverymm = QString::number(ratio ,'f',2);
    cv::line(DisplayImage, cv::Point(0, m_BlueModelled_top), cv::Point(DisplayImage.cols, m_BlueModelled_top),
             cv::Scalar(0, 0, 0), 1);

    //写下降高度
    double downText = INI_File().GetFixedHigh();
    QString downTextline = QString::number(downText ,'f',2);
    cv::putText(DisplayImage, downTextline.toStdString(), cv::Point(0, m_BlueModelled_top - 5),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255));
    qDebug()<< "参照物最顶点Y:" << m_BlueModelled_top<<"底部"<<m_BlueModelled_bottom
             << "像素比例每像素(px/mm):"<< m_pxdpieverymm<<endl;
    return 0;
}

void Heightmeasurement::AdjustHSVvalue(cv::Mat HSVimage ,int index )
{
    // cv::waitKey(0);
    //int posTrackbar = 0;
    //int maxValue = 255;
    //cv::createTrackbar("pos","The modified image",&posTrackbar,maxValue,ChangeBrightValue,0);
    //ChangeBrightValue(posTrackbar,0);
    t_index = index;
    cv::Mat mat_knowlength;
    cv::cvtColor(HSVimage, mat_knowlength, cv::COLOR_BGR2HSV);
    if(index == 0)
    {
        cv::namedWindow("0000");
        cv::imshow("000",HSVimage);
        cv::createTrackbar("LH", "0000", &LH,  180, onLH, &mat_knowlength);
        cv::createTrackbar("HH", "0000", &HH,  180, onHH,&mat_knowlength);
        cv::createTrackbar("LS", "0000", &LS,  255, onLS,&mat_knowlength);
        cv::createTrackbar("HS", "0000", &HS,  255, onHS,&mat_knowlength);
        cv::createTrackbar("LV", "0000", &LV,  255, onLV,&mat_knowlength);
        cv::createTrackbar("HV", "0000", &HV,  255, onHV,&mat_knowlength);
    }
    else if(index == 1)
    {
        cv::namedWindow("0001");
        cv::imshow("001",HSVimage);
        cv::createTrackbar("LH", "0001", &LH,  180, onLH, &mat_knowlength);
        cv::createTrackbar("HH", "0001", &HH,  180, onHH,&mat_knowlength);
        cv::createTrackbar("LS", "0001", &LS,  255, onLS,&mat_knowlength);
        cv::createTrackbar("HS", "0001", &HS,  255, onHS,&mat_knowlength);
        cv::createTrackbar("LV", "0001", &LV,  255, onLV,&mat_knowlength);
        cv::createTrackbar("HV", "0001", &HV,  255, onHV,&mat_knowlength);
    }
    else if(index == 2)
    {
        cv::namedWindow("0002");
        cv::imshow("002",HSVimage);
        cv::createTrackbar("LH", "0002", &LH,  180, onLH, &mat_knowlength);
        cv::createTrackbar("HH", "0002", &HH,  180, onHH,&mat_knowlength);
        cv::createTrackbar("LS", "0002", &LS,  255, onLS,&mat_knowlength);
        cv::createTrackbar("HS", "0002", &HS,  255, onHS,&mat_knowlength);
        cv::createTrackbar("LV", "0002", &LV,  255, onLV,&mat_knowlength);
        cv::createTrackbar("HV", "0002", &HV,  255, onHV,&mat_knowlength);
    }
    else if(index == 3)
    {
        cv::namedWindow("0003");
        cv::imshow("003",HSVimage);
        cv::createTrackbar("LH", "0003", &LH,  180, onLH, &mat_knowlength);
        cv::createTrackbar("HH", "0003", &HH,  180, onHH,&mat_knowlength);
        cv::createTrackbar("LS", "0003", &LS,  255, onLS,&mat_knowlength);
        cv::createTrackbar("HS", "0003", &HS,  255, onHS,&mat_knowlength);
        cv::createTrackbar("LV", "0003", &LV,  255, onLV,&mat_knowlength);
        cv::createTrackbar("HV", "0003", &HV,  255, onHV,&mat_knowlength);
    }
    cv::waitKey(0);
}

int Heightmeasurement::FindBloodTube(bool FindYellowLinque,cv::Mat image_findtube)
{
    cv::Mat mat_knowlength;
    cv::Scalar lower_blood_1;
    cv::Scalar upper_blood_1;
    cv::cvtColor(image_findtube, mat_knowlength, cv::COLOR_BGR2HSV);
    //黑色血样宽度 截取到试管宽度
    if(FindYellowLinque == false)
    {
         lower_blood_1 = cv::Scalar(0,  0,  0);
         upper_blood_1 = cv::Scalar(180,  255, 102);
         //调试阈值
         //LH = 117;LS = 47;LV =0 ;HH = 180;HS = 255; HV =255 ;
         //AdjustHSVvalue(mat_knowlength,2);
    }
    else
    {
        lower_blood_1 = cv::Scalar(0,  43,  46);
        upper_blood_1 = cv::Scalar(34,  255, 255);
        //调试阈值
        //LH = 0; LS = 78 ;LV = 0 ;HH = 61;HS = 255; HV = 255 ;
        //AdjustHSVvalue(mat_knowlength,3);
    }
    cv::Mat  blood_mask;
    cv::inRange(mat_knowlength, lower_blood_1, upper_blood_1, blood_mask);
    cv::Mat blood_res;
    cv::bitwise_and(image_findtube, image_findtube, blood_res, blood_mask);
    vector<vector<cv::Point>> contours_bloodtube;
    vector<cv::Vec4i> hierarchy_bloodtube;
    cv::findContours(blood_mask, contours_bloodtube, hierarchy_bloodtube, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point());
    QVector<int> listArea;
    listArea.clear();
    //遍历所有轮廓，绘制缺陷轮廓
    for (int i = 0; i < contours_bloodtube.size(); i++)
    {
        int aero = contourArea(contours_bloodtube[i]);//计算轮廓面积
        listArea.append(aero);
    }
    if(listArea.isEmpty())
    {
        return -1;
    }
    //最大值
    QVector<int>::iterator maxvalue = std::max_element(std::begin(listArea),std::end(listArea));
    int biggest = *maxvalue;
    //最大值下标
    auto positionmax = std::distance(std::begin(listArea),maxvalue);
    int posmax = positionmax;
    QLOG_DEBUG()<< "找试管血样最大值面积"<<biggest << "找试管血样最大值面积下标:" << posmax<<endl;
    cv::RotatedRect BloodRect = cv::minAreaRect(contours_bloodtube[posmax]);
    if(FindYellowLinque == false)
        drawContours(image_findtube, contours_bloodtube, posmax, cv::Scalar(110,110,112), -1, 8, hierarchy_bloodtube);//绘制轮廓
    else
        drawContours(image_findtube, contours_bloodtube, posmax, cv::Scalar(255, 250, 205),  -1, 8, hierarchy_bloodtube);
    double w_length = BloodRect.boundingRect().width/2;
    double h_length = BloodRect.boundingRect().height/2;
    //左上，右下
    if(FindYellowLinque == false)
    {
        m_tubeLeft_top = QPoint(BloodRect.center.x - w_length, BloodRect.center.y - h_length);
        m_tube_Right_bottom = QPoint(BloodRect.center.x + w_length, BloodRect.center.y + h_length);
        m_tube_Right_top = QPoint(BloodRect.center.x + w_length, BloodRect.center.y - h_length);
        QLOG_DEBUG()<<"血块左上:"<<m_tubeLeft_top<<"血块右下"<<m_tube_Right_bottom<<"血块右上"<<m_tube_Right_top<<"jiaodu000 =="<<BloodRect.angle<<endl;
        //cv::imshow("ffffffffff", image_findtube);
    }
    else
    {
        m_Yellow_bottom = BloodRect.center.y + h_length;
        m_Yellow_width = BloodRect.boundingRect().width;
        m_Yellow_Top = BloodRect.center.y - h_length;

        QLOG_DEBUG()<<"黄色底部Y="<<m_Yellow_bottom<<"黄色宽:"<<m_Yellow_width<<"PPPtop"<<m_Yellow_Top<<endl;
       // cv::imshow("zzzzzzzxx", image_findtube);
    }
    return 0;
}


void Heightmeasurement::OutPrintTestHigh(cv::Mat OutSaveImage, string savepath)
{
    double Zero_to_yellowbottom = (m_BlueModelled_top - m_Yellow_bottom) * m_pxdpieverymm.toDouble(); //零点到血样最顶点
    double NeddleDownHigh = INI_File().GetFixedHigh() - Zero_to_yellowbottom;
    QLOG_DEBUG() << "零点到黄色血样最底点间距像素点:" << Zero_to_yellowbottom<<"测高下针高度"<<NeddleDownHigh<<endl;
    QString downNeddlehigh = QString::number(NeddleDownHigh, 'f', 2);
    //PPPtop 画线
    cv::line(OutSaveImage,cv::Point(0,m_Yellow_Top),cv::Point(OutSaveImage.cols, m_Yellow_Top),cv::Scalar(0, 255, 255), 1,cv::LINE_AA);

    //pppBottom
    cv::line(OutSaveImage, cv::Point(0,m_Yellow_bottom), cv::Point(OutSaveImage.cols, m_Yellow_bottom), cv::Scalar(170, 255, 0), 1,cv::LINE_AA);

    //写下降高度
    cv::putText(OutSaveImage, downNeddlehigh.toStdString(), cv::Point(0, m_Yellow_bottom - 5),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255));
    //蜂鸣器响一声
    emit Testheightfinish(true);

    //保存临时测高图片
    cv::imwrite(savepath, OutSaveImage);
    if(!Correct_Data::RetestHight)
    {
        emit FinishTestHigh(QString::fromStdString(savepath), NeddleDownHigh);//测高值绑定样本
    }
    else
    {
        emit RetestHeight(QString::fromStdString(savepath), NeddleDownHigh);//重测
    }
    InitPara();
}

void Heightmeasurement::TestHeightFailed(string savepath,cv::Mat OutSaveImage)
{
    //蜂鸣器响3声声
    emit Testheightfinish(false);
    //保存临时测高图片
    cv::imwrite(savepath, OutSaveImage);   //保存图片
    if(!Correct_Data::RetestHight)
    {
        emit FinishTestHigh(QString::fromStdString(savepath), 0);//测高值绑定样本
    }
    else
    {
        emit RetestHeight(QString::fromStdString(savepath), 0);//重测
    }
}

void Heightmeasurement::InitPara()
{
   m_BlueModelled_top = 0;      //蓝色参照物顶点
   m_BlueModelled_bottom = 0;   //蓝色参照物底部
   m_BlueModelled_right = 0;    //蓝色参照物最右侧

   m_tubeLeft_top.setX(0);     //黑色血样左上角
   m_tube_Right_bottom.setX(0);
   m_tube_Right_top.setX(0);
   m_tubeLeft_top.setY(0);     //黑色血样左上角
   m_tube_Right_bottom.setY(0);
   m_tube_Right_top.setY(0);

   m_Yellow_bottom = 0; //黄色底部Y
   m_Yellow_width = 0;  //黄色宽
}




















































///////////////////////////////配置机器参数//////////////////////////////////////////////
ConfigureWriteParameter::ConfigureWriteParameter(int id, QObject *parent) : QObject(parent)
{
    m_id = id;
    m_DelayedMS = 0;
    m_bNeedDelayed = false;
    m_BeginChannel = 0;

    m_machinespotr.clear();
}

ConfigureWriteParameter::~ConfigureWriteParameter()
{
    QLOG_DEBUG()<<"退出发送机器命令线程ID:"<<m_id;
}

QStringList Char2String(char *data, int size)
{
    QByteArray byArr;
    QString back_data;
    byArr.resize(size);
    for (int i = 0; i<size; i++)
        byArr[i] = data[i];
    QDataStream out(&byArr, QIODevice::ReadWrite);
    while (!out.atEnd())
    {
        qint8 outchar = 0;
        out >> outchar;
        QString str = QString("%1").arg(outchar & 0xFF, 2, 16, QLatin1Char('0')).toUpper() + QString((" "));
        back_data += str;
    }
    back_data.simplified();
    QStringList outlist = back_data.split(" ");
    if (outlist.last().isEmpty())
        outlist.removeLast();
    return outlist;
}

void bytearry_2_String(QByteArray arry,QStringList &Machinedata){

    char buf[PROTOCOL_LENGTH];//数组
    int len_array = arry.size();
    int len_buf = sizeof(buf);
    int len = qMin(len_array, len_buf);
    memcpy(buf, arry, len);// 转化
    Machinedata = Char2String(buf, PROTOCOL_LENGTH);
    return;
}
void OutPrintSendCode(QByteArrayList Shaparry)
{
    QStringList OutPrintcode;
    OutPrintcode.clear();
    auto iter = Shaparry.constBegin();
    while (iter != Shaparry.constEnd())
    {
        bytearry_2_String(*iter, OutPrintcode);
        qDebug() << ("发送到机器命令:") << QThread::currentThread() << "ID" << OutPrintcode;
        iter++;
    }
    return;
}

void ConfigureWriteParameter::Delay_MSec_Suspend(unsigned int msec)
{
    qDebug()<<msec;
    emit WriteMachineAction(m_machinespotr);

    m_machinespotr.clear();

}

void ConfigureWriteParameter::run()
{

    QMutexLocker locker(&mlockmutex);
    if (!m_machinespotr.isEmpty() && m_machinespotr.size() >= 1)
    {
        if(m_bNeedDelayed)
        {
            Delay_MSec_Suspend(m_DelayedMS); //死等
        }
        else
        {
            QThread::msleep(1);
            emit WriteMachineAction(m_machinespotr);
            m_machinespotr.clear();
        }
    }
}

/*开始测试后机器运动发送数据终端*/
void ConfigureWriteParameter::sycnSerialclass(SuoweiSerialPort * port)
{
    if(Serial_port) return;
    if(Serial_port == nullptr)
    {
        Serial_port = port;
        connect(this, &ConfigureWriteParameter::WriteMachineAction, Serial_port, &SuoweiSerialPort::write_data);
    }
    return;
}

void ConfigureWriteParameter::WriteCodearray(QByteArrayList arraycode)
{
    QMutexLocker locker(&mlockmutex);
    m_machinespotr = arraycode;
    m_DelayedMS = 0;
    m_bNeedDelayed = false;
    return;
}

void ConfigureWriteParameter::slotDelayed_sending(const QByteArrayList arraycode,int delayedmS, bool needDelayed )
{
    QMutexLocker locker(&mlockmutex);
    m_machinespotr = arraycode;
    m_DelayedMS = delayedmS;
    m_bNeedDelayed = needDelayed;
    return;
}

void ConfigureWriteParameter::slotWhileReadChannelParam(const int Fromchannel)
{
    QMutexLocker locker(&mlockmutex);
    m_BeginChannel = Fromchannel;
    return;
}

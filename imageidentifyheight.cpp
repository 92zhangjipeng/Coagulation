#include "imageidentifyheight.h"
#include "ui_imageidentifyheight.h"
#include <QCameraViewfinder>

ImageiDentifyHeight::ImageiDentifyHeight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageiDentifyHeight)
{
    ui->setupUi(this);
	
	cameras = QCameraInfo::availableCameras();
	foreach (const QCameraInfo &cameraInfo,cameras)
	{
		qDebug() << "CameraInfo:" << cameraInfo;
		ui->cameraType->addItem(cameraInfo.description());
	}
	mcapture = new QCamera();
	//mviewfinder = new QCameraViewfinder(ui->widget);
	//mImageCapture = new QCameraImageCapture(mcapture);
	//QCameraViewfinderSettings viewFinderSettings;
	connect(mImageCapture, &QCameraImageCapture::imageCaptured, this, &ImageiDentifyHeight::displayImage);
	mcapture->start();
}

ImageiDentifyHeight::~ImageiDentifyHeight()
{
    delete ui;
}

void  ImageiDentifyHeight::openCamara()
{
	mImageCapture->capture();
	return;
}

void  ImageiDentifyHeight::displayImage(int id,  QImage image)
{
	//ui->displaylabe->setPixmap(QPixmap::fromImage(image));
}

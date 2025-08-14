#ifndef IMAGEIDENTIFYHEIGHT_H
#define IMAGEIDENTIFYHEIGHT_H

#include <QDialog>
#include <QImage>
#include <QTimer>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QPixmap>
#include <QCameraViewfinder>



namespace Ui {
class ImageiDentifyHeight;
}


class ImageiDentifyHeight : public QDialog
{
    Q_OBJECT

public:
    explicit ImageiDentifyHeight(QWidget *parent = 0);
    ~ImageiDentifyHeight();

public slots:
	void openCamara();
	void displayImage(int id, QImage preview);

private:
    Ui::ImageiDentifyHeight *ui;
	QCamera *mcapture;
	//QCameraViewfinder  * mviewfinder;//摄像头取景器部件
	QCameraImageCapture *mImageCapture;
	QList<QCameraInfo> cameras;
};

#endif // IMAGEIDENTIFYHEIGHT_H

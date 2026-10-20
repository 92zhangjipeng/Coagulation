#ifndef QRCODEREADER_H
#define QRCODEREADER_H

#include <QObject>
#include <QImage>
#include <QString>

// 前置声明，避免在头文件中包含 QZXing.h（可选）
class QZXing;

// 定义一个结构体，用于保存详细的解码结果
struct DetailedResult {
    bool success;
    QString text;       // 解码出的内容
    QString format;     // 码制名称，如 "QR_CODE", "EAN_13"
};


class QRCodeReader : public QObject
{
    Q_OBJECT

public:
    explicit QRCodeReader(QObject *parent = nullptr);
    ~QRCodeReader();

    /**
     * @brief 从图片文件识别二维码
     * @param filePath 图片文件路径
     * @return 识别出的字符串，失败返回空字符串
     */
    QString readFromFile(const QString &filePath);

    /**
     * @brief 从 QImage 对象识别二维码
     * @param image 已经加载的 QImage
     * @return 识别出的字符串，失败返回空字符串
     */
    QString readFromImage(const QImage &image);

    /**
     * @brief 获取最后一次错误信息
     */
    QString lastError() const { return m_lastError; }

signals:
    /**
     * @brief 识别完成时发出的信号
     * @param success 是否成功
     * @param result 识别结果（成功时有意义）
     */
    void decodingFinished(bool success, const QString &result);

    // 新的信号，可以传递详细的解码信息
    void decodingFinishedWithDetails(const DetailedResult &result);

private:
    void initDecoder();
    QString performDecoding(const QImage &image);

    QZXing *m_decoder;      // QZXing 解码器对象
    QString m_lastError;    // 最后一次错误信息
};

#endif // QRCODEREADER_H

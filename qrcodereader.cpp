#include "QRCodeReader.h"
#include "includeZXing/QZXing.h"      // 注意：确保你的 INCLUDEPATH 已包含 QZXing 头文件目录
#include <QFile>
#include <QDebug>

QRCodeReader::QRCodeReader(QObject *parent)
    : QObject(parent)
    , m_decoder(nullptr)
{
    initDecoder();
}

QRCodeReader::~QRCodeReader()
{
    if (m_decoder) {
        delete m_decoder;
        m_decoder = nullptr;
    }
}

void QRCodeReader::initDecoder()
{
    m_decoder = new QZXing(this);
    // 设置解码格式，开启多种一维码和二维码格式
    uint enabledFormats = QZXing::DecoderFormat_QR_CODE |          // 二维码
                          QZXing::DecoderFormat_CODE_128 |        // Code 128
                          QZXing::DecoderFormat_EAN_13 |          // EAN-13
                          QZXing::DecoderFormat_CODE_39 |         // Code 39
                          QZXing::DecoderFormat_UPC_A |           // UPC-A
                          QZXing::DecoderFormat_EAN_8;            // EAN-8
    m_decoder->setDecoder(enabledFormats);
    // 可选：提高解码成功率，但会增加耗时
    // m_decoder->setTryHarder(true);
}

QString QRCodeReader::readFromFile(const QString &filePath)
{
    m_lastError.clear();

    if (filePath.isEmpty()) {
        m_lastError = "文件路径为空";
        emit decodingFinished(false, QString());
        return QString();
    }

    QImage image;
    if (!image.load(filePath)) {
        m_lastError = QString("无法加载图片: %1").arg(filePath);
        qWarning() << m_lastError;
        emit decodingFinished(false, QString());
        return QString();
    }

    return readFromImage(image);
}

QString QRCodeReader::readFromImage(const QImage &image)
{
    m_lastError.clear();
    if (image.isNull()) {
        m_lastError = "传入的 QImage 为空";
        emit decodingFinished(false, QString());
        emit decodingFinishedWithDetails({false, "", ""});
        return QString();
    }

    QString fullResult = performDecoding(image);
    bool success = !fullResult.isEmpty();

    DetailedResult details;
    details.success = success;

    if (success) {
        // 解析返回值，分离格式和内容
        int colonPos = fullResult.indexOf(':');
        if (colonPos != -1) {
            details.format = fullResult.left(colonPos);
            details.text = fullResult.mid(colonPos + 1);
        } else {
            // 兼容旧的纯文本返回值
            details.format = "UNKNOWN";
            details.text = fullResult;
        }
        qDebug() << "识别成功 - 格式:" << details.format << "内容:" << details.text;
    } else {
        m_lastError = "未在图片中找到有效的条码";
        details.text = "";
        details.format = "";
    }

    emit decodingFinished(success, details.text);
    emit decodingFinishedWithDetails(details);
    return details.text;
}

QString QRCodeReader::performDecoding(const QImage &image)
{
    if (!m_decoder) {
       m_lastError = "解码器未初始化";
       return QString();
   }

   // 现在 decodeImage 函数能返回包含格式信息的字符串了
   // 这是假设你已经按照第二步修改了 QZXing 库的 decodeImage 方法
   QString result = m_decoder->decodeImage(image);
   // 注意：这个 result 现在的格式是 "类型:内容"，比如 "QR_CODE:Hello, World!"

   if (result.isEmpty() && !image.isNull()) {
       // 如果识别失败，尝试转换图像格式后再试一次
       QImage converted = image.convertToFormat(QImage::Format_RGB32);
       result = m_decoder->decodeImage(converted);
   }

   return result;
}

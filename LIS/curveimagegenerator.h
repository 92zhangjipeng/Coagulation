#ifndef CURVEIMAGEGENERATOR_H
#define CURVEIMAGEGENERATOR_H

#include <QString>
#include <QJsonObject>
#include <QObject>
#include <QTimer>
#include <QMutex>

class CurveImageGenerator : public QObject
{
    Q_OBJECT
public:

    // Meyers 单例：返回静态局部对象的指针
    static CurveImageGenerator* instance();

    // 初始化方法（在程序启动时调用一次，传入真实的数据库路径和保存目录）
    void init(const QString &dbPath, const QString &saveDir = "./CurveImages");


    // 设置/获取保存目录
    void setSaveDirectory(const QString &dir);
    QString saveDirectory() const;
    void setRetentionDays(int days);// 设置图片保留天数
    int retentionDays() const;

    // ---------- 测试数据（JSON）保存相关配置 ----------
    void setDataSaveEnabled(bool enabled);    // 是否同时保存 JSON 数据文件，默认 true
    bool isDataSaveEnabled() const;
    void setDataSaveDirectory(const QString &dir);  // 数据文件保存目录，默认与图片目录相同
    QString dataSaveDirectory() const;
    void setDataRetentionDays(int days);      // 数据文件保留天数，默认与图片保留天数相同
    int dataRetentionDays() const;



    // 同步生成图片线程安全，可在任意线程调用
    bool generateCurveImageSync(const QString &sampleId);


public slots:
    // 异步生成图片（内部直接调用同步版本，无需切换线程）
    void generateCurveImageAsync(const QString &sampleId);

signals:
    void imageGenerated(const QString &sampleId, bool success, const QString &localPath);
    void dataSaved(const QString &sampleId, bool success, const QString &jsonPath);   // 数据保存完成信号

private slots:
    void onCleanupTimer();   // 定时器回调

private:
    explicit CurveImageGenerator(const QString &dbPath,
                                 const QString &saveDir = "./CurveImages",
                                 QObject *parent = nullptr);
    ~CurveImageGenerator();

    // 数据库查询
    QJsonObject querySampleCurves(const QString &sampleId);   // 查询曲线数据
    QJsonObject querySampleDetail(const QString &sampleId);   // 查询样本完整信息（用于 JSON）

    // 渲染并保存图片
    bool renderAndSaveImage(const QString &sampleId, const QJsonObject &curves);
    // 保存样本数据为 JSON 文件
    bool saveSampleDataToJson(const QString &sampleId);
    // 清理指定目录下特定扩展名、超过保留天数的文件
    void cleanupOldFiles(const QString &dir, const QStringList &filters, int retentionDays);

    QString dataSaveDirectory_unlocked() const;   // 不加锁版本
    int dataRetentionDays_unlocked() const;

    QString m_dbPath;
    QString m_saveDir;          // 图片保存目录
    int m_retentionDays;        // 图片保留天数

    bool m_dataSaveEnabled;     // 是否启用数据保存
    QString m_dataSaveDir;      // 数据保存目录（空则与图片目录相同）
    int m_dataRetentionDays;    // 数据保留天数（-1 表示与图片相同）

    QTimer* m_cleanupTimer;
    mutable  QMutex  m_mutex;            // 保护文件操作
};

#endif // CURVEIMAGEGENERATOR_H

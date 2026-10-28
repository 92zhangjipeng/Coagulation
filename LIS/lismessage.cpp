#include "lismessage.h"

#include <QDateTime>
#include <QJsonObject>
#include <QList>
#include <QtGlobal>

static QString hl7Escape(QString text)
{
    text.replace("\\", "\\E\\");
    text.replace("|", "\\F\\");
    text.replace("^", "\\S\\");
    text.replace("&", "\\T\\");
    text.replace("~", "\\R\\");
    text.replace("\r", " ");
    text.replace("\n", " ");
    return text;
}

static QString astmClean(QString text)
{
    text.replace("|", " ");
    text.replace("^", " ");
    text.replace("&", " ");
    text.replace("\\", " ");
    text.replace("\r", " ");
    text.replace("\n", " ");
    return text.trimmed();
}

static QString sexCode(const QString &gender)
{
    if (gender.contains("M") || gender == QString::fromUtf8("\xe7\x94\xb7")) return "M";
    if (gender.contains("F") || gender == QString::fromUtf8("\xe5\xa5\xb3")) return "F";
    return "U";
}

static QString hl7Time(const QString &raw)
{
    QString digits;
    for (int i = 0; i < raw.size(); ++i) {
        if (raw.at(i).isDigit()) digits.append(raw.at(i));
    }
    if (digits.size() >= 14) return digits.left(14);
    if (digits.size() == 8) return digits + "000000";
    return QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
}

static bool parseAgg(const QString &stored, double out[4])
{
    QStringList parts = stored.split(',', QString::SkipEmptyParts);
    if (parts.size() < 4) return false;
    for (int i = 0; i < 4; ++i) {
        QString one = parts.at(i).trimmed();
        if (one.endsWith('%')) one.chop(1);
        bool ok = false;
        out[i] = one.toDouble(&ok);
        if (!ok) return false;
    }
    return true;
}

static QString num(double value)
{
    return QString::number(value, 'f', 2);
}

struct ObxItem
{
    QString code;
    QString value;
    QString unit;
};

static void appendReagent(QList<ObxItem> &items, const QString &code, const QJsonObject &detail)
{
    const QJsonObject results = detail.value("results").toObject();
    const QString stored = results.value(code).toString().trimmed();
    if (stored.isEmpty()) return;

    double agg[4];
    if (parseAgg(stored, agg)) {
        const QString tags[4] = {"60", "180", "END", "MAX"};
        for (int i = 0; i < 4; ++i) {
            ObxItem item;
            item.code = code + "_" + tags[i];
            item.value = num(agg[i]);
            item.unit = "%";
            items.append(item);
        }
    } else {
        ObxItem item;
        item.code = code;
        item.value = stored;
        item.unit = "";
        items.append(item);
    }

    const QJsonObject analysis = detail.value("analysis_parameters").toObject().value(code).toObject();
    const QString keys[5] = {"AUC", "MaxSlope", "TMA", "Lag", "Adhesion"};
    for (int i = 0; i < 5; ++i) {
        const double value = analysis.value(keys[i]).toDouble();
        if (qAbs(value) < 0.0001) continue;
        ObxItem item;
        item.code = code + "_" + keys[i];
        item.value = QString::number(value, 'f', 4);
        item.unit = "";
        items.append(item);
    }
}

LisOutboundText LisMessage::build(const LisConfigData &cfg,
                                  const QJsonObject &detail,
                                  const QString &localIp,
                                  bool includeCurveUrl)
{
    LisOutboundText out;
    const QString sampleId = detail.value("sample_id").toString().trimmed();
    if (sampleId.isEmpty()) {
        out.error = "sample not found";
        return out;
    }

    QList<ObxItem> items;
    appendReagent(items, "AA", detail);
    appendReagent(items, "ADP", detail);
    appendReagent(items, "EPI", detail);
    appendReagent(items, "COL", detail);
    appendReagent(items, "RIS", detail);
    if (items.isEmpty()) {
        out.error = "no result";
        return out;
    }
    if (detail.contains("age")) {
        ObxItem age;
        age.code = "AGE";
        age.value = QString::number(detail.value("age").toInt());
        age.unit = "yr";
        items.append(age);
    }

    const QString now = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    const QString tested = hl7Time(detail.value("inspection_time").toString().isEmpty()
                                   ? detail.value("add_time").toString()
                                   : detail.value("inspection_time").toString());
    const QString barcode = detail.value("barcode").toString().trimmed();
    const QString name = detail.value("patient_name").toString().trimmed();
    const QString sex = sexCode(detail.value("gender").toString());
    const QString msgId = sampleId + now;

    QString note = QString("dept=%1;referring=%2;examining=%3;review=%4;diagnosis=%5")
            .arg(detail.value("department").toString(),
                 detail.value("referring_doctor").toString(),
                 detail.value("examining_doctor").toString(),
                 detail.value("review_doctor").toString(),
                 detail.value("diagnosis").toString());
    if (includeCurveUrl && !localIp.isEmpty()) {
        note += QString(";curve=http://%1:%2/v1/sample/%3/curves.png")
                .arg(localIp)
                .arg(cfg.pullPort)
                .arg(sampleId);
    }

    out.hl7 = QString("MSH|^~\\&|%1|%2|%3|%4|%5||ORU^R01|%6|P|2.5|||||||%7\r"
                      "PID|1||%8^^^^BAR||%9|||%10\r"
                      "OBR|1||%11|PLT^Platelet Aggregation|||%12\r")
            .arg(hl7Escape(cfg.sendingApp))
            .arg(hl7Escape(cfg.sendingFacility))
            .arg(hl7Escape(cfg.recvApp))
            .arg(hl7Escape(cfg.recvFacility))
            .arg(now)
            .arg(hl7Escape(msgId))
            .arg(cfg.charset == "GBK" ? QString("GBK") : QString("UTF-8"))
            .arg(hl7Escape(barcode.isEmpty() ? sampleId : barcode))
            .arg(hl7Escape(name))
            .arg(sex)
            .arg(hl7Escape(sampleId))
            .arg(tested);
    for (int i = 0; i < items.size(); ++i) {
        const ObxItem &item = items.at(i);
        bool numeric = false;
        item.value.toDouble(&numeric);
        const QString type = numeric ? QString("NM") : QString("TX");
        out.hl7 += QString("OBX|%1|%2|%3^%3||%4|%5|||||F\r")
                .arg(QString::number(i + 1))
                .arg(type)
                .arg(hl7Escape(item.code))
                .arg(hl7Escape(item.value))
                .arg(hl7Escape(item.unit));
    }
    out.hl7 += QString("NTE|1||%1\r").arg(hl7Escape(note));

    out.astmRecords << QString("H|\\^&|||%1|||||||P|LIS2-A2|%2")
                           .arg(astmClean(cfg.sendingApp), now);
    out.astmRecords << QString("P|1||%1||%2|||%3")
                           .arg(astmClean(barcode.isEmpty() ? sampleId : barcode),
                                astmClean(name),
                                sex);
    out.astmRecords << QString("O|1|%1||^^^PLT|R||%2")
                           .arg(astmClean(sampleId), tested);
    for (int i = 0; i < items.size(); ++i) {
        const ObxItem &item = items.at(i);
        out.astmRecords << QString("R|%1|^^^%2|%3|%4||||F")
                               .arg(QString::number(i + 1))
                               .arg(astmClean(item.code))
                               .arg(astmClean(item.value))
                               .arg(astmClean(item.unit));
    }
    out.astmRecords << QString("C|1|L|%1|G").arg(astmClean(note));
    out.astmRecords << "L|1|N";
    return out;
}

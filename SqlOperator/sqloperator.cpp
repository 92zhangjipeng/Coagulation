#pragma execution_character_set("utf-8")
#include "sqloperator.h"
#include <QSqlError>
#include <QSqlDriver>
#include <operclass/fullyautomatedplatelets.h>

SqlOperator::SqlOperator()
{
    QUIUtils::isDirExist(QUIUtils::getappload("suoweiData"));
}

bool SqlOperator::open(const QString &dbName, const QString &connectName)
{
    if (QSqlDatabase::connectionNames().contains(connectName)) {
        m_db = QSqlDatabase::database(connectName);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", connectName);
    }

    m_connectName = connectName;

    if (m_db.isOpen()) {
        m_db.close();
    }

    m_db.setDatabaseName(dbName);
    if (m_db.open()) {
        m_query = QSqlQuery(m_db);
        return true;
    }

    m_lastError = m_db.lastError().text();
    return false;
}

void SqlOperator::close()
{
    m_db.close();
}

QString SqlOperator::getConnectName() const
{
    return m_connectName;
}

bool SqlOperator::isExistTable(const QString &tableName)
{
    return m_db.tables().contains(tableName);
}

bool SqlOperator::createTable(const QString &tableName, const QHash<QString, QString> &fields, const QList<QString> &pks)
{
    if (isExistTable(tableName)) {
        m_lastError = QString("Table [%1] is already exist").arg(tableName);
        return false;
    }

    QString sql = QString("create table %1 (").arg(tableName);
    for (auto iter = fields.begin(); iter != fields.end(); ++iter) {
        sql.append(QString("%1 %2, ").arg(iter.key(), iter.value()));
    }

    sql.append(QString("primary key ("));
    for (const auto &item : pks) {
        sql.append(QString("%1, ").arg(item));
    }

    sql.remove(sql.length() - 2, 2);
    sql.append("))");

    if (m_query.exec(sql)) {
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::selectData(const QString &tableName, const QList<QString> &fields, QList<QVariantList> &data)
{
    if (!checkTableInfo(tableName, fields)) {
        return false;
    }

    QString sql = QString("select ");
    for (const auto &item : fields) {
        sql.append(QString("%1, ").arg(item));
    }
    sql.remove(sql.length() - 2, 2);
    sql.append(QString(" from %1").arg(tableName));

    if (m_query.exec(sql)) {
        while (m_query.next()) {
            QVariantList temp;
            for (const auto &item : fields) {
                temp.append(m_query.value(item));
            }
            data.append(temp);
        }
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::selectData(const QString &tableName, const QList<QString> &fields,
                             const QHash<QString, QVariant> &whereConditions, QList<QVariantList> &data)
{
    QList<QString> tempFields;
    tempFields.append(fields);
    tempFields.append(whereConditions.keys());
    if (!checkTableInfo(tableName, tempFields)) {
        return false;
    }

    QString sql = QString("select ");
    for (const auto &item : fields) {
        sql.append(QString("%1, ").arg(item));
    }
    sql.remove(sql.length() - 2, 2);
    sql.append(QString(" from %1").arg(tableName));

    sql.append(QString(" where "));
    for (auto iter = whereConditions.begin(); iter != whereConditions.end(); ++iter) {
        sql.append(QString("%1 = ? and ").arg(iter.key()));
    }
    sql.remove(sql.length() - 5, 5);

    m_query.prepare(sql);
    for (auto iter = whereConditions.begin(); iter != whereConditions.end(); ++iter) {
        m_query.addBindValue(iter.value());
    }

    if (m_query.exec()) {
        while (m_query.next()) {
            QVariantList temp;
            for (const auto &item : fields) {
                temp.append(m_query.value(item));
            }
            data.append(temp);
        }
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::selectDataBySql(const QString &sql, const QList<QString> &fields, QList<QVariantList> &data)
{
    if (m_query.exec(sql)) {
        while (m_query.next()) {
            QVariantList temp;
            for (const auto &item : fields) {
                temp.append(m_query.value(item));
            }
            data.append(temp);
        }
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::insertRowData(const QString &tableName, const QList<QString> &fields, const QVariantList &data)
{
    if (!checkTableInfo(tableName, fields)) {
        return false;
    }

    QString sql = QString("insert into %1(").arg(tableName);
    for (const auto &item : fields) {
        sql.append(QString("%1, ").arg(item));
    }
    sql.remove(sql.length() - 2, 2);
    sql.append(QString(") values"));

    QString tempValue = QString("(");
    for (int i = 0; i < fields.count(); ++i) {
        tempValue.append(QString("?, "));
    }
    tempValue.remove(tempValue.length() - 2, 2);
    tempValue.append(QString(")"));

    sql.append(tempValue);

    m_query.prepare(sql);

    for (int i = 0; i < fields.count(); ++i) {
        m_query.addBindValue(data.at(i));
    }


    if (m_query.exec()) {
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::insertRowsData(const QString &tableName, const QList<QString> &fields, const QList<QVariantList> &data)
{
    if (!checkTableInfo(tableName, fields)) {
        return false;
    }

    QString sql = QString("insert into %1(").arg(tableName);
    for (const auto &item : fields) {
        sql.append(QString("%1, ").arg(item));
    }
    sql.remove(sql.length() - 2, 2);
    sql.append(QString(") values"));

    QString tempValue = QString("(");
    for (int i = 0; i < fields.count(); ++i) {
        tempValue.append(QString("?, "));
    }
    tempValue.remove(tempValue.length() - 2, 2);
    tempValue.append(QString(")"));

    for (int i = 0; i < data.count(); ++i) {
        sql.append(QString("%1, ").arg(tempValue));
    }
    sql.remove(sql.length() - 2, 2);

    m_query.prepare(sql);
    for (const auto &item : data) {
        for (int i = 0; i < fields.count(); ++i) {
            m_query.addBindValue(item.at(i));
        }
    }

    if (m_query.exec()) {
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::updateData(const QString &tableName, const QHash<QString, QVariant> &data,
                             const QHash<QString, QVariant> &whereConditions)
{
    QList<QString> fields;
    fields << data.keys() << whereConditions.keys();
    if (!checkTableInfo(tableName, fields)) {
        return false;
    }

    QString sql = QString("update %1 set ").arg(tableName);
    for (auto iter = data.begin(); iter != data.end(); ++iter) {
        sql.append(QString("%1 = ?, ").arg(iter.key()));
    }
    sql.remove(sql.length() - 2, 2);

    sql.append(" where ");
    for (auto iter = whereConditions.begin(); iter != whereConditions.end(); ++iter) {
        sql.append(QString("%1 = ? and ").arg(iter.key()));
    }
    sql.remove(sql.length() - 5, 5);

    m_query.prepare(sql);
    for (auto iter = data.begin(); iter != data.end(); ++iter) {
        m_query.addBindValue(iter.value());
    }
    for (auto iter = whereConditions.begin(); iter != whereConditions.end(); ++iter) {
        m_query.addBindValue(iter.value());
    }

    if (m_query.exec()) {
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::deleteData(const QString &tableName, const QHash<QString, QVariant> &whereConditions)
{
    QList<QString> fields = whereConditions.keys();
    if (!checkTableInfo(tableName, fields)) {
        return false;
    }

    QString sql = QString("delete from %1 where ").arg(tableName);
    for (auto iter = whereConditions.begin(); iter != whereConditions.end(); ++iter) {
        sql.append(QString("%1 = ? and ").arg(iter.key()));
    }
    sql.remove(sql.length() - 5, 5);

    m_query.prepare(sql);
    for (auto iter = whereConditions.begin(); iter != whereConditions.end(); ++iter) {
        m_query.addBindValue(iter.value());
    }

    if (m_query.exec()) {
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::exec(const QString &sql)
{
    if (m_query.exec(sql)) {
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

bool SqlOperator::hasTransactions()
{
    if (m_query.driver()->hasFeature(QSqlDriver::Transactions)) {
        return true;
    }

    m_lastError = QString("This database don't support tasnsactions");
    return false;
}

bool SqlOperator::transaction()
{
    return m_db.transaction();
}

bool SqlOperator::commit()
{
    return m_db.commit();
}

bool SqlOperator::rollback()
{
    return m_db.rollback();
}

int SqlOperator::size()
{
    int count = -1;
    m_query.first();
    if (m_query.next()) {
        if (m_query.driver()->hasFeature(QSqlDriver::QuerySize)) {
            count = m_query.size();
        } else {
            m_query.last();
            // m_query.at()是返回当前记录的编号(从0开始)，所以最后一条记录的编号 +1 就为记录数
            count = m_query.at() + 1;
        }
    }

    m_query.first();
    return count;
}

bool SqlOperator::getTableFieldsInfo(const QString &tableName, QList<QString> &fieldsName)
{
    QString sql = QString("PRAGMA table_info('%1')").arg(tableName);
    if (m_query.exec(sql)) {
        while (m_query.next()) {
            fieldsName.append(m_query.value(1).toString());
        }
        return true;
    }

    m_lastError = m_query.lastError().text();
    return false;
}

QString SqlOperator::lastQuerySql()
{
    return m_query.lastQuery();
}

QString SqlOperator::lastError() const
{
    return m_lastError;
}

bool SqlOperator::checkTableInfo(const QString &tableName, const QList<QString> &fields)
{
    if (!isExistTable(tableName)) {
        m_lastError = QString("Table [%1] is not exist").arg(tableName);
        return false;
    }

    QList<QString> fieldsName;
    QStringList noFieldsName;
    if (getTableFieldsInfo(tableName, fieldsName)) {
        for (const auto &item : fields) {
            if (!fieldsName.contains(item)) {
                noFieldsName << item;
            }
        }

        if (noFieldsName.count() > 0) {
            m_lastError = QString("Table [%1] have no fields [%2]").arg(tableName).arg(noFieldsName.join(','));
            return false;
        }
    } else {
        return false;
    }

    return true;
}


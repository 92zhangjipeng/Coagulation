#include "lissettingspage.h"

#include "lisbridge.h"
#include "lisconfig.h"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFormLayout>
#include <QHeaderView>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableWidget>
#include <QTcpSocket>
#include <QTimer>

static QString zh(const char *utf8)
{
    return QString::fromUtf8(utf8);
}

static QString statusText(const QString &status)
{
    if (status == "pending") return zh("\xe5\xbe\x85\xe5\x8f\x91\xe9\x80\x81");
    if (status == "sending") return zh("\xe5\x8f\x91\xe9\x80\x81\xe4\xb8\xad");
    if (status == "sent") return zh("\xe5\xb7\xb2\xe9\x80\x81\xe8\xbe\xbe");
    if (status == "failed") return zh("\xe5\xa4\xb1\xe8\xb4\xa5");
    return status;
}

static QWidget *need(QWidget *host, const char *name)
{
    QWidget *widget = host->findChild<QWidget *>(QLatin1String(name));
    return widget;
}

void LisSettingsPage::attach(QWidget *host)
{
    if (!host || host->findChild<LisSettingsPage *>()) {
        return;
    }
    new LisSettingsPage(host);
}

LisSettingsPage::LisSettingsPage(QWidget *host)
    : QObject(host)
    , m_host(host)
    , m_enabled(qobject_cast<QCheckBox *>(need(host, "checkBox_lisEnabled")))
    , m_pull(qobject_cast<QCheckBox *>(need(host, "checkBox_lisPull")))
    , m_pullPort(qobject_cast<QSpinBox *>(need(host, "spinBox_lisPullPort")))
    , m_url(qobject_cast<QLabel *>(need(host, "label_lisPullUrl")))
    , m_push(qobject_cast<QCheckBox *>(need(host, "checkBox_lisPush")))
    , m_onComplete(qobject_cast<QCheckBox *>(need(host, "checkBox_lisOnComplete")))
    , m_onReview(qobject_cast<QCheckBox *>(need(host, "checkBox_lisOnReview")))
    , m_protocol(qobject_cast<QComboBox *>(need(host, "comboBox_lisProtocol")))
    , m_hostEdit(qobject_cast<QLineEdit *>(need(host, "lineEdit_lisHost")))
    , m_pushPort(qobject_cast<QSpinBox *>(need(host, "spinBox_lisPushPort")))
    , m_charset(qobject_cast<QComboBox *>(need(host, "comboBox_lisCharset")))
    , m_sendApp(qobject_cast<QLineEdit *>(need(host, "lineEdit_lisSendApp")))
    , m_sendFacility(qobject_cast<QLineEdit *>(need(host, "lineEdit_lisSendFacility")))
    , m_recvApp(qobject_cast<QLineEdit *>(need(host, "lineEdit_lisRecvApp")))
    , m_recvFacility(qobject_cast<QLineEdit *>(need(host, "lineEdit_lisRecvFacility")))
    , m_timeout(qobject_cast<QSpinBox *>(need(host, "spinBox_lisTimeout")))
    , m_retry(qobject_cast<QSpinBox *>(need(host, "spinBox_lisRetry")))
    , m_status(qobject_cast<QLabel *>(need(host, "label_lisStatus")))
    , m_table(qobject_cast<QTableWidget *>(need(host, "tableWidget_lisOutbox")))
    , m_testButton(qobject_cast<QPushButton *>(need(host, "pushButton_lisTest")))
    , m_resendButton(qobject_cast<QPushButton *>(need(host, "pushButton_lisResend")))
    , m_pullForm(host->findChild<QFormLayout *>(QLatin1String("formLayout_lisPull")))
    , m_pushForm(host->findChild<QFormLayout *>(QLatin1String("formLayout_lisPush")))
    , m_refresh(new QTimer(this))
{
    if (!m_enabled || !m_pull || !m_push || !m_table || !m_hostEdit) {
        return;
    }

    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels(QStringList()
                                       << zh("\xe6\xa0\xb7\xe6\x9c\xac\xe5\x8f\xb7")
                                       << zh("\xe7\x8a\xb6\xe6\x80\x81")
                                       << zh("\xe6\xac\xa1\xe6\x95\xb0")
                                       << zh("\xe8\xaf\xb4\xe6\x98\x8e")
                                       << zh("\xe6\x97\xb6\xe9\x97\xb4"));
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);

    QPushButton *saveButton = qobject_cast<QPushButton *>(need(host, "pushButton_lisSave"));
    QPushButton *refreshButton = qobject_cast<QPushButton *>(need(host, "pushButton_lisRefresh"));
    if (saveButton) {
        connect(saveButton, &QPushButton::clicked, this, [this]() { saveForm(); });
    }
    if (m_testButton) {
        connect(m_testButton, &QPushButton::clicked, this, [this]() { testHost(); });
    }
    if (refreshButton) {
        connect(refreshButton, &QPushButton::clicked, this, [this]() { reloadTable(); updateUrl(); });
    }
    if (m_resendButton) {
        connect(m_resendButton, &QPushButton::clicked, this, [this]() {
            const int row = m_table->currentRow();
            if (row < 0 || !m_table->item(row, 0)) return;
            LisBridge::instance()->resend(m_table->item(row, 0)->text());
            reloadTable();
        });
    }
    connect(m_protocol, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [this](int) {
        const int port = m_pushPort->value();
        if (port == 2575 || port == 5000) {
            m_pushPort->setValue(m_protocol->currentText() == "ASTM" ? 5000 : 2575);
        }
    });
    connect(m_pull, &QCheckBox::toggled, this, [this](bool) { updateUrl(); updateModeRows(); });
    connect(m_push, &QCheckBox::toggled, this, [this](bool) { updateModeRows(); });
    connect(m_pullPort, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [this](int) { updateUrl(); });
    connect(m_refresh, &QTimer::timeout, this, [this]() { reloadTable(); updateUrl(); });
    m_refresh->setInterval(3000);
    host->installEventFilter(this);
    loadForm();
}

bool LisSettingsPage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_host && m_enabled) {
        if (event->type() == QEvent::Show) {
            loadForm();
            m_refresh->start();
        } else if (event->type() == QEvent::Hide) {
            m_refresh->stop();
        }
    }
    return QObject::eventFilter(watched, event);
}

void LisSettingsPage::showRow(QFormLayout *form, QWidget *field, bool visible)
{
    if (!field) return;
    field->setVisible(visible);
    if (!form) return;
    if (QWidget *label = form->labelForField(field)) {
        label->setVisible(visible);
    }
}

void LisSettingsPage::updateModeRows()
{
    const bool pull = m_pull->isChecked();
    const bool push = m_push->isChecked();
    showRow(m_pullForm, m_pullPort, pull);
    showRow(m_pullForm, m_url, pull);
    if (m_onComplete) m_onComplete->setVisible(push);
    if (m_onReview) m_onReview->setVisible(push);
    showRow(m_pushForm, m_protocol, push);
    showRow(m_pushForm, m_hostEdit, push);
    showRow(m_pushForm, m_pushPort, push);
    showRow(m_pushForm, m_charset, push);
    showRow(m_pushForm, m_sendApp, push);
    showRow(m_pushForm, m_sendFacility, push);
    showRow(m_pushForm, m_recvApp, push);
    showRow(m_pushForm, m_recvFacility, push);
    showRow(m_pushForm, m_timeout, push);
    showRow(m_pushForm, m_retry, push);
    if (m_testButton) m_testButton->setVisible(push);
    if (m_resendButton) m_resendButton->setVisible(push);
    if (m_table) m_table->setVisible(push);
}

void LisSettingsPage::loadForm()
{
    if (!m_enabled) return;
    const LisConfigData data = LisConfig::load();
    const QSignalBlocker blockPull(m_pull);
    const QSignalBlocker blockPush(m_push);
    const QSignalBlocker blockProtocol(m_protocol);
    m_enabled->setChecked(data.enabled);
    m_pull->setChecked(data.pullEnabled);
    m_pullPort->setValue(data.pullPort);
    m_push->setChecked(data.pushEnabled);
    m_onComplete->setChecked(data.sendOnTestComplete);
    m_onReview->setChecked(data.sendOnReview);
    m_protocol->setCurrentIndex(data.protocol == "ASTM" ? 1 : 0);
    m_hostEdit->setText(data.host);
    m_pushPort->setValue(data.pushPort);
    m_charset->setCurrentText(data.charset);
    m_sendApp->setText(data.sendingApp);
    m_sendFacility->setText(data.sendingFacility);
    m_recvApp->setText(data.recvApp);
    m_recvFacility->setText(data.recvFacility);
    m_timeout->setValue(data.timeoutSec);
    m_retry->setValue(data.retryMax);
    updateUrl();
    updateModeRows();
    reloadTable();
}

void LisSettingsPage::saveForm()
{
    LisConfigData data;
    data.enabled = m_enabled->isChecked();
    data.pullEnabled = m_pull->isChecked();
    data.pullPort = m_pullPort->value();
    data.pushEnabled = m_push->isChecked();
    data.sendOnTestComplete = m_onComplete->isChecked();
    data.sendOnReview = m_onReview->isChecked();
    data.protocol = m_protocol->currentText();
    data.host = m_hostEdit->text().trimmed();
    data.pushPort = m_pushPort->value();
    data.charset = m_charset->currentText();
    data.sendingApp = m_sendApp->text().trimmed();
    data.sendingFacility = m_sendFacility->text().trimmed();
    data.recvApp = m_recvApp->text().trimmed();
    data.recvFacility = m_recvFacility->text().trimmed();
    data.timeoutSec = m_timeout->value();
    data.retryMax = m_retry->value();

    if (data.pushEnabled && !data.host.isEmpty()) {
        QHostAddress address;
        if (!address.setAddress(data.host)) {
            QMessageBox::warning(m_host, "LIS",
                                 zh("LIS \xe5\x9c\xb0\xe5\x9d\x80\xe8\xa6\x81\xe5\xa1\xab IPv4\xef\xbc\x8c\xe4\xbe\x8b\xe5\xa6\x82 192.168.1.10"));
            return;
        }
    }
    if (data.enabled && data.pushEnabled && !data.sendOnTestComplete && !data.sendOnReview) {
        data.sendOnTestComplete = true;
        m_onComplete->setChecked(true);
    }
    LisConfig::save(data);
    LisBridge::instance()->applyConfig();
    updateUrl();
    m_status->setText(zh("\xe5\xb7\xb2\xe4\xbf\x9d\xe5\xad\x98\xe3\x80\x82\xe6\x8d\xa2\xe5\x8c\xbb\xe9\x99\xa2\xe5\x8f\xaa\xe6\x94\xb9\xe5\x9c\xb0\xe5\x9d\x80\xe3\x80\x81\xe7\xab\xaf\xe5\x8f\xa3\xe5\x92\x8c\xe5\x8d\x8f\xe8\xae\xae\xe3\x80\x82"));
}

void LisSettingsPage::updateUrl()
{
    const QString ip = LisConfig::localIpv4();
    const QString pull = LisBridge::instance()->pullStatus();
    if (!m_enabled->isChecked() || !m_pull->isChecked()) {
        m_url->setText(zh("\xe6\x9c\xaa\xe5\xbc\x80\xe5\x90\xaf"));
    } else if (ip.isEmpty()) {
        m_url->setText(zh("\xe6\x9c\xaa\xe6\x89\xbe\xe5\x88\xb0\xe6\x9c\xac\xe6\x9c\xba\xe5\x9c\xb0\xe5\x9d\x80"));
    } else {
        m_url->setText(QString("http://%1:%2/v1/health").arg(ip).arg(m_pullPort->value()));
    }
    if (pull == "ok") {
        m_status->setText(zh("\xe5\x8f\x96\xe6\x95\xb0\xe6\x9c\x8d\xe5\x8a\xa1\xe5\xb7\xb2\xe5\xbc\x80\xe5\x90\xaf"));
    } else if (pull == "off") {
        m_status->setText(zh("\xe5\x8f\x96\xe6\x95\xb0\xe6\x9c\x8d\xe5\x8a\xa1\xe6\x9c\xaa\xe5\xbc\x80\xe5\x90\xaf"));
    } else if (!pull.isEmpty()) {
        m_status->setText(zh("\xe5\x8f\x96\xe6\x95\xb0\xe6\x9c\x8d\xe5\x8a\xa1\xe5\x90\xaf\xe5\x8a\xa8\xe5\xa4\xb1\xe8\xb4\xa5\xef\xbc\x9a") + pull);
    }
}

void LisSettingsPage::testHost()
{
    const QString host = m_hostEdit->text().trimmed();
    QHostAddress address;
    if (!address.setAddress(host)) {
        QMessageBox::warning(m_host, "LIS",
                             zh("\xe5\x85\x88\xe5\xa1\xab\xe5\x86\x99 LIS \xe7\x9a\x84 IPv4 \xe5\x9c\xb0\xe5\x9d\x80"));
        return;
    }
    QTcpSocket socket;
    socket.connectToHost(host, static_cast<quint16>(m_pushPort->value()));
    const bool ok = socket.waitForConnected(3000);
    socket.abort();
    QMessageBox::information(m_host, "LIS", ok
                             ? zh("\xe5\xb7\xb2\xe8\xbf\x9e\xe4\xb8\x8a LIS \xe7\xab\xaf\xe5\x8f\xa3")
                             : zh("\xe8\xbf\x9e\xe4\xb8\x8d\xe4\xb8\x8a\xef\xbc\x8c\xe8\xaf\xb7\xe6\xa3\x80\xe6\x9f\xa5\xe5\x9c\xb0\xe5\x9d\x80\xe3\x80\x81\xe7\xab\xaf\xe5\x8f\xa3\xe5\x92\x8c\xe7\xbd\x91\xe7\xbb\x9c"));
}

void LisSettingsPage::reloadTable()
{
    if (!m_table) return;
    const QString dbPath = LisBridge::instance()->dbPath();
    m_table->setRowCount(0);
    if (dbPath.isEmpty()) return;
    const QString conn = "lis_settings_view";
    {
        QSqlDatabase existing = QSqlDatabase::database(conn, false);
        if (existing.isValid()) {
            existing.close();
            QSqlDatabase::removeDatabase(conn);
        }
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", conn);
    db.setDatabaseName(dbPath);
    if (!db.open()) return;
    int row = 0;
    {
        QSqlQuery query(db);
        if (!query.exec("SELECT sample_id, status, tries, last_error, updated_at FROM LisOutbox "
                        "ORDER BY updated_at DESC LIMIT 30")) {
            query.finish();
            db.close();
            QSqlDatabase::removeDatabase(conn);
            return;
        }
        while (query.next()) {
            m_table->insertRow(row);
            const QString columns[5] = {
                query.value(0).toString(),
                statusText(query.value(1).toString()),
                query.value(2).toString(),
                query.value(3).toString(),
                query.value(4).toString()
            };
            for (int col = 0; col < 5; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(columns[col]);
                item->setTextAlignment(Qt::AlignCenter);
                m_table->setItem(row, col, item);
            }
            ++row;
        }
        query.finish();
    }
    db.close();
    QSqlDatabase::removeDatabase(conn);
}

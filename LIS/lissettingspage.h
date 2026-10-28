#ifndef LISSETTINGSPAGE_H
#define LISSETTINGSPAGE_H

#include <QObject>

class QCheckBox;
class QComboBox;
class QEvent;
class QFormLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QTimer;
class QWidget;

class LisSettingsPage : public QObject
{
public:
    static void attach(QWidget *host);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    explicit LisSettingsPage(QWidget *host);
    void loadForm();
    void saveForm();
    void reloadTable();
    void updateUrl();
    void updateModeRows();
    void testHost();
    void showRow(QFormLayout *form, QWidget *field, bool visible);

    QWidget *m_host;
    QCheckBox *m_enabled;
    QCheckBox *m_pull;
    QSpinBox *m_pullPort;
    QLabel *m_url;
    QCheckBox *m_push;
    QCheckBox *m_onComplete;
    QCheckBox *m_onReview;
    QComboBox *m_protocol;
    QLineEdit *m_hostEdit;
    QSpinBox *m_pushPort;
    QComboBox *m_charset;
    QLineEdit *m_sendApp;
    QLineEdit *m_sendFacility;
    QLineEdit *m_recvApp;
    QLineEdit *m_recvFacility;
    QSpinBox *m_timeout;
    QSpinBox *m_retry;
    QLabel *m_status;
    QTableWidget *m_table;
    QPushButton *m_testButton;
    QPushButton *m_resendButton;
    QFormLayout *m_pullForm;
    QFormLayout *m_pushForm;
    QTimer *m_refresh;
};

#endif

#pragma once
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

class CustomMessageBox : public QDialog
{
	Q_OBJECT
public:
	explicit CustomMessageBox(const QString &title, const QString &text,
		QMessageBox::Icon icon,
		QMessageBox::StandardButtons buttons = QMessageBox::Ok,
		QWidget *parent = nullptr);

	static void warning(QWidget *parent, const QString &title, const QString &text) {
		CustomMessageBox msgBox(title, text, QMessageBox::Warning, QMessageBox::Ok, parent);
		msgBox.setWindowModality(Qt::WindowModal);
		msgBox.show();
		msgBox.raise();
		msgBox.activateWindow();
		msgBox.exec();
	}

	static void warningNonModal(QWidget *parent, const QString &title, const QString &text) {
		CustomMessageBox *msgBox = new CustomMessageBox(title, text, QMessageBox::Warning, QMessageBox::Ok, parent);
		msgBox->setAttribute(Qt::WA_DeleteOnClose);
		msgBox->setWindowModality(Qt::NonModal);
		msgBox->setModal(false);
		msgBox->show();
		msgBox->raise();
		msgBox->activateWindow();
	}

	static void information(QWidget *parent, const QString &title, const QString &text) {
		CustomMessageBox msgBox(title, text, QMessageBox::Information, QMessageBox::Ok, parent);
		msgBox.setWindowModality(Qt::WindowModal);
		msgBox.show();
		msgBox.raise();
		msgBox.activateWindow();
		msgBox.exec();
	}

	static void critical(QWidget *parent, const QString &title, const QString &text) {
		CustomMessageBox msgBox(title, text, QMessageBox::Critical, QMessageBox::Ok, parent);
		msgBox.setWindowModality(Qt::WindowModal);
		msgBox.show();
		msgBox.raise();
		msgBox.activateWindow();
		msgBox.exec();
	}

	static QMessageBox::StandardButton question(QWidget *parent, const QString &title, const QString &text,
		QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No);

private:
	QMessageBox::StandardButton m_result;
};
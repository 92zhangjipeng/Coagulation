// CustomMessageBox.h
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
                              QMessageBox::Icon icon, QWidget *parent = nullptr);

    static void warning(QWidget *parent, const QString &title, const QString &text) {
        CustomMessageBox msgBox(title, text, QMessageBox::Warning, parent);
        msgBox.exec();
    }

   // 添加静态便捷函数
   static void information(QWidget *parent, const QString &title, const QString &text);
   static void critical(QWidget *parent, const QString &title, const QString &text);
   static void question(QWidget *parent, const QString &title, const QString &text);
};

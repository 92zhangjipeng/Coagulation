#ifndef CUSTOM_TEXT_COLORS_H
#define CUSTOM_TEXT_COLORS_H

#include <QObject>

class Custom_Text_Colors : public QObject
{
    Q_OBJECT
public:
    explicit Custom_Text_Colors(QObject *parent = nullptr);

signals:

public slots:
    void SetText_Colors(int Colors_index, QString enter_str, QString &out_str, QString change_text);
};

#endif // CUSTOM_TEXT_COLORS_H

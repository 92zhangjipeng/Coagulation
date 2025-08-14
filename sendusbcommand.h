#ifndef SENDUSBCOMMAND_H
#define SENDUSBCOMMAND_H

#include <QObject>
#include <QReadWriteLock>
#include <testing.h>


class Sendusbcommand : public QObject
{
    Q_OBJECT
public:
    explicit Sendusbcommand(QObject *parent = nullptr);
	~Sendusbcommand();

public slots:


	
signals:

   
private:

	
};

#endif // SENDUSBCOMMAND_H

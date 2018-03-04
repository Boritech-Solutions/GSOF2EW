#ifndef EWCONN_H
#define EWCONN_H
#include <QObject>
#include "client.h"

class EWconn : public QObject
{
    Q_OBJECT
public:
    explicit EWconn(QObject *parent = nullptr);

signals:

public slots:
    void print2sc(GPS_State state);

private:
    GPS_State state;

};

#endif // EWCONN_H

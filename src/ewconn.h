#ifndef EWCONN_H
#define EWCONN_H
#include <QObject>
#include <QHostAddress>
extern "C"{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "earthworm.h"
    #include "kom.h"
    #include <transport.h>       /* Earthworm shared-memory transport routines */
    #include <trace_buf.h>       /* Earthworm trace buffer/packet definitions */
    #define MAX_BYTES_STATUS MAX_BYTES_PER_EQ
}

#include "client.h"

class EWconn : public QObject
{
    Q_OBJECT
public:
    explicit EWconn(QObject *parent = nullptr, QString configfile = QString(""));
    QString getHost();
    qint32  getPort();
signals:

public slots:
    void processState(GPS_State state);

private:
    QString config;
    GPS_State mystate;
    bool connected;
    bool velocity;
    QHostAddress add;
    QString mod_name;
    unsigned char mod_id;
    QString ring_name;
    long ring_id;
    qint32 heartbeat;
    qint32 logf;
    QString staID;
    QString netID;
    qint32 debug;
    qint32 port;
    qint32 sampler;
    double SubX,SubY,SubZ;
    bool   Xcor,Ycor,Zcor;

    SHM_INFO   region;                /* The shared memory region   */
    MSG_LOGO   logo;

    unsigned char TypeTraceBuf2;
    unsigned char InstId;        /* local installation id      */

    int  get_config(char *configfile);
    void appendlog(QString status);
    int  disconnectFromEw();
    void createPacket();
    int  connectToEw();

};

#endif // EWCONN_H

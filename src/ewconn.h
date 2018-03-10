#ifndef EWCONN_H
#define EWCONN_H
#include <QObject>
#include <QHostAddress>
#include <QTimer>
extern "C"{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <earthworm.h>       /* Earthworm main definitions and routines */
    #include <kom.h>             /* Earthworm configuration file reader routine */
    #include <transport.h>       /* Earthworm shared-memory transport routines */
    #include <trace_buf.h>       /* Earthworm trace buffer/packet definitions */
}
#define MAX_BYTES_STATUS MAX_BYTES_PER_EQ
#define MAX_MSG_SIZE      256

#include "client.h"

class EWconn : public QObject
{
    Q_OBJECT
public:
    explicit EWconn(QObject *parent = nullptr, QString configfile = QString(""), qint64 mypid = 0);
    QString getHost();      // Get host IP
    qint32  getPort();      // Get port number
    bool    getVelF();      // Get velocity flag
    bool    isConn ();      // All ok?
signals:

public slots:
    void processState(GPS_State state);
    void sendHB();

private:
    QString config;         // Config file
    qint64 pid;             // Pid number
    GPS_State mystate;      // GPS State
    bool connected;         // Connection check
    bool velocity;          // Velocity flag check
    QHostAddress add;       // IP Address
    QString mod_name;       // Module Name
    unsigned char mod_id;   // Module ID
    QString ring_name;      // Ring Name
    long ring_id;           // Ring ID
    qint32 heartbeat;       // Heartbeat Rate
    qint32 logf;            // Logfile
    QString staID;          // Station ID
    QString netID;          // Network ID
    qint32 debug;           // Debug Level
    qint32 port;            // Port Number
    qint32 sampler;         // Sample Rate
    double SubX,SubY,SubZ;  // Correction or 0-level
    bool   Xcor,Ycor,Zcor;  // Correction flag

    SHM_INFO   region;      /* The shared memory region   */
    MSG_LOGO   hblogo;      /* Logo */

    unsigned char TypeTraceBuf2;    /* Type tracebuff message id */
    unsigned char TypeHeartBeat;    /* Type heartbeat message id */
    unsigned char TypeError;        /* Type error     message id */

    unsigned char InstId;           /* Local installation id     */

    int  get_config(char *configfile);                                      // Get parameters from config file
    void appendlog(QString status);                                         // Append to log file
    int  disconnectFromEw();                                                // Disconnect from EW
    void createTracePacket();                                               // Create EW TracePacket
    void createHBPacket(unsigned char type, short code, char *message);     // Create HB Packet
    int  connectToEw();                                                     // Connect to EW
    QTimer* BeatHeart;                                                      // Heartbeat Timer

};

#endif // EWCONN_H

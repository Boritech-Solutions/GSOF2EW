#ifndef CLIENT_H
#define CLIENT_H
// Uses QT and code from the ArduPilot project: https://github.com/ArduPilot/ardupilot
#include <QTcpSocket>
#include <QDataStream>
#include <QNetworkSession>
#include <QDateTime>
#include <QGeoCoordinate>
#include <QVector3D>
#include <QObject>
#include <QDebug>

// GPS Specific double precision conversions
// The precision here does matter when using the wsg* functions for converting
// between LLH and ECEF coordinates.
#define DEG_TO_RAD_DOUBLE 0.0174532925199432954743716805978692718781530857086181640625
#define RAD_TO_DEG_DOUBLE 57.29577951308232286464772187173366546630859375

enum GPS_Status {
    NO_GPS = 0,
    NO_FIX = 1,
    GPS_OK_FIX_2D = 2,
    GPS_OK_FIX_3D = 3,
    GPS_OK_FIX_3D_DGPS = 4,
    GPS_OK_FIX_3D_RTK = 5
};

struct ECEFPos {
    double X;
    double Y;
    double Z;
};

struct GPS_State {
    uint8_t instance; // the instance number of this GPS
    // all the following fields must all be filled by the backend driver

    GPS_Status status;
    uint32_t time_week_ms;
    uint16_t time_week;
    QGeoCoordinate location;
    ECEFPos ECEF;
    float ground_speed;
    float ground_course;
    uint16_t hdop;
    uint16_t vdop;
    uint8_t num_sats;
    QVector3D velocity;
    float speed_accuracy;
    float horizontal_accuracy;
    float vertical_accuracy;
    bool have_vertical_velocity:1;
    bool have_speed_accuracy:1;
    bool have_horizontal_accuracy:1;
    bool have_vertical_accuracy:1;
    uint32_t last_gps_time_ms;
    QDateTime last_gps_time;
};

class Client : public QObject
{
    Q_OBJECT
public:
    Client(QHostAddress maddress = QHostAddress("127.0.0.1"), qint16 mport = 10000, bool velreq = false);
    bool isconn();
    void connectToGPS();

private slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void processBytes();

signals:
    void messageReceived(GPS_State current);

private:
    QTcpSocket *tcpSocket = nullptr;
    //QDataStream in;
    uint64_t time_epoch_convert(uint16_t gps_week, uint32_t gps_ms);
    double SwapDouble(uint8_t*,uint32_t);
    float SwapFloat(uint8_t*,uint32_t);
    uint32_t SwapUint32(uint8_t*,uint32_t);
    uint16_t SwapUint16(uint8_t*,uint32_t);
    bool parse(uint8_t temp);
    bool process_message();
    bool velocity;
    GPS_State state;
    qint64 port;
    QHostAddress address;

    static const uint8_t GSOF_STX = 0x02;
    static const uint8_t GSOF_ETX = 0x03;
    struct gsof_msg_parser_t{
        enum
        {
            STARTTX = 0,
            STATUS,
            PACKETTYPE,
            LENGTH,
            DATA,
            CHECKSUM,
            ENDTX
        } gsof_state;

        uint8_t starttx;
        uint8_t status;
        uint8_t packettype;
        uint8_t length;
        uint8_t data[256];
        uint8_t checksum;
        uint8_t endtx;
        uint16_t read;

        uint8_t checksumcalc;
    } gsof_msg ;
};

#endif // CLIENT_H

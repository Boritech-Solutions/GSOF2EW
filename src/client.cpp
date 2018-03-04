#include "client.h"

Client::Client(QHostAddress address, qint16 port)
    : tcpSocket(new QTcpSocket(this))
{
    //in.setDevice(tcpSocket);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Client::displayError);
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(processBytes()));

    tcpSocket->connectToHost(address, port);
    state.instance = 1;
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "The connection was refused by the peer."
               << "Make sure the server is running,"
               << "and check that the host name and port settings are correct.";
        break;
    default:
        qDebug() << "The following error occurred:" << tcpSocket->errorString();
        break;
    }
}

void Client::processBytes(){
    QByteArray data;
    data = tcpSocket->readAll();
    for (int i = 0; i < data.size(); i++){
        parse(data.at(i));
    }
}

double Client::SwapDouble(uint8_t* src, uint32_t pos)
{
    union {
        double d;
        char bytes[sizeof(double)];
    } doubleu;
    doubleu.bytes[0] = src[pos + 7];
    doubleu.bytes[1] = src[pos + 6];
    doubleu.bytes[2] = src[pos + 5];
    doubleu.bytes[3] = src[pos + 4];
    doubleu.bytes[4] = src[pos + 3];
    doubleu.bytes[5] = src[pos + 2];
    doubleu.bytes[6] = src[pos + 1];
    doubleu.bytes[7] = src[pos + 0];

    return doubleu.d;
}

float Client::SwapFloat(uint8_t* src, uint32_t pos)
{
    union {
        float f;
        char bytes[sizeof(float)];
    } floatu;
    floatu.bytes[0] = src[pos + 3];
    floatu.bytes[1] = src[pos + 2];
    floatu.bytes[2] = src[pos + 1];
    floatu.bytes[3] = src[pos + 0];

    return floatu.f;
}

uint32_t Client::SwapUint32(uint8_t* src, uint32_t pos)
{
    union {
        uint32_t u;
        char bytes[sizeof(uint32_t)];
    } uint32u;
    uint32u.bytes[0] = src[pos + 3];
    uint32u.bytes[1] = src[pos + 2];
    uint32u.bytes[2] = src[pos + 1];
    uint32u.bytes[3] = src[pos + 0];

    return uint32u.u;
}

uint16_t Client::SwapUint16(uint8_t* src, uint32_t pos)
{
    union {
        uint16_t u;
        char bytes[sizeof(uint16_t)];
    } uint16u;
    uint16u.bytes[0] = src[pos + 1];
    uint16u.bytes[1] = src[pos + 0];

    return uint16u.u;
}

bool Client::parse(uint8_t temp)
{
    switch (gsof_msg.gsof_state)
    {
    default:
    case gsof_msg_parser_t::STARTTX:
        if (temp == GSOF_STX)
        {
            gsof_msg.starttx = temp;
            gsof_msg.gsof_state = gsof_msg_parser_t::STATUS;
            gsof_msg.read = 0;
            gsof_msg.checksumcalc = 0;
        }
        break;
    case gsof_msg_parser_t::STATUS:
        gsof_msg.status = temp;
        gsof_msg.gsof_state = gsof_msg_parser_t::PACKETTYPE;
        gsof_msg.checksumcalc += temp;
        break;
    case gsof_msg_parser_t::PACKETTYPE:
        gsof_msg.packettype = temp;
        gsof_msg.gsof_state = gsof_msg_parser_t::LENGTH;
        gsof_msg.checksumcalc += temp;
        break;
    case gsof_msg_parser_t::LENGTH:
        gsof_msg.length = temp;
        gsof_msg.gsof_state = gsof_msg_parser_t::DATA;
        gsof_msg.checksumcalc += temp;
        break;
    case gsof_msg_parser_t::DATA:
        gsof_msg.data[gsof_msg.read] = temp;
        gsof_msg.read++;
        gsof_msg.checksumcalc += temp;
        if (gsof_msg.read >= gsof_msg.length)
        {
            gsof_msg.gsof_state = gsof_msg_parser_t::CHECKSUM;
        }
        break;
    case gsof_msg_parser_t::CHECKSUM:
        gsof_msg.checksum = temp;
        gsof_msg.gsof_state = gsof_msg_parser_t::ENDTX;
        if (gsof_msg.checksum == gsof_msg.checksumcalc)
        {
            return process_message();
        }
        break;
    case gsof_msg_parser_t::ENDTX:
        gsof_msg.endtx = temp;
        gsof_msg.gsof_state = gsof_msg_parser_t::STARTTX;
        break;
    }

    return false;
}

bool Client::process_message()
{
    //http://www.trimble.com/OEM_ReceiverHelp/V4.81/en/default.html#welcome.html

    if (gsof_msg.packettype == 0x40) { // GSOF
#if gsof_DEBUGGING
        uint8_t trans_number = gsof_msg.data[0];
        uint8_t pageidx = gsof_msg.data[1];
        uint8_t maxpageidx = gsof_msg.data[2];
        qDebug() <<"GSOF page:" << pageidx << "of" << maxpageidx <<"(trans_number="<< trans_number <<")";
#endif

        int valid = 0;

        // want 1 2 8 9 12

        for (uint32_t a = 3; a < gsof_msg.length; a++)
        {
            uint8_t output_type = gsof_msg.data[a];
            a++;
            uint8_t output_length = gsof_msg.data[a];
            a++;
            //qDebug() << "GSOF type:" << output_type << "len:" << output_length;

            if (output_type == 1) // pos time
            {
                state.time_week_ms = SwapUint32(gsof_msg.data, a);
                state.time_week = SwapUint16(gsof_msg.data, a + 4);
                state.num_sats = gsof_msg.data[a + 6];
                uint8_t posf1 = gsof_msg.data[a + 7];
                uint8_t posf2 = gsof_msg.data[a + 8];
                //qDebug() << "POSTIME:" << posf1 << posf2);
                if ((posf1 & 1) == 1)
                {
                    state.status = GPS_OK_FIX_3D;
                    if ((posf2 & 1) == 1)
                    {
                        state.status = GPS_OK_FIX_3D_DGPS;
                        if ((posf2 & 4) == 4)
                        {
                            state.status = GPS_OK_FIX_3D_RTK;
                        }
                    }
                }
                else
                {
                    state.status = NO_FIX;
                }
                valid++;
            }

            else if (output_type == 2) // position
            {
                state.location.setLatitude((int32_t)(RAD_TO_DEG_DOUBLE * (SwapDouble(gsof_msg.data, a)) * 1e7));
                state.location.setLongitude((int32_t)(RAD_TO_DEG_DOUBLE * (SwapDouble(gsof_msg.data, a + 8)) * 1e7));
                state.location.setAltitude((int32_t)(SwapDouble(gsof_msg.data, a + 16) * 1e2));

                state.last_gps_time_ms = state.time_week_ms;

                valid++;
            }

            else if (output_type == 8) // velocity
            {
                uint8_t vflag = gsof_msg.data[a];
                if ((vflag & 1) == 1)
                {
                    state.ground_speed = SwapFloat(gsof_msg.data, a + 1);
                    state.ground_course = (SwapFloat(gsof_msg.data, a + 5) * RAD_TO_DEG_DOUBLE);
                    float gps_heading = state.ground_course * DEG_TO_RAD_DOUBLE;
                    state.velocity.setX(state.ground_speed*cosf(gps_heading));
                    state.velocity.setY(state.ground_speed*sinf(gps_heading));
                    state.velocity.setZ(-SwapFloat(gsof_msg.data, a + 9));
                    state.have_vertical_velocity = true;
                }
                valid++;
            }

            else if (output_type == 9) //dop
            {
                state.hdop = (uint16_t)(SwapFloat(gsof_msg.data, a + 4) * 100);
                valid++;
            }

            else if (output_type == 12) // position sigma
            {
                state.horizontal_accuracy = (SwapFloat(gsof_msg.data, a + 4) + SwapFloat(gsof_msg.data, a + 8)) / 2;
                state.vertical_accuracy = SwapFloat(gsof_msg.data, a + 16);
                state.have_horizontal_accuracy = true;
                state.have_vertical_accuracy = true;
                valid++;
            }
            a += output_length-1u;
        }

        if (valid == 5) {
            emit messageReceived(state);
            return true;
        } else {
            state.status = NO_FIX;
        }
    }

    return false;
}

uint64_t Client::time_epoch_convert(uint16_t gps_week, uint32_t gps_ms)
{
    const uint64_t ms_per_week = 7000ULL*86400ULL;
    const uint64_t unix_offset = 17000ULL*86400ULL + 52*10*7000ULL*86400ULL - 15000ULL;
    uint64_t fix_time_ms = unix_offset + gps_week*ms_per_week + gps_ms;
    return fix_time_ms;
}

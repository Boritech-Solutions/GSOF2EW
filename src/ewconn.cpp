#include "ewconn.h"

EWconn::EWconn(QObject *parent) : QObject(parent)
{

}

void EWconn::print2sc(GPS_State state)
{
    qDebug() << state.location.latitude() << state.location.longitude() << state.location.altitude();
    qDebug() << state.time_week << state.time_week_ms;
}

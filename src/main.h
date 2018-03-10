#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <QCommandLineParser>
#include <QHostAddress>
#include <QObject>
#include <QDebug>
#include "client.h"
#include "ewconn.h"

Client *gps;

EWconn *EWconnection;
bool debug;
bool velocity;

#endif // MAIN_H

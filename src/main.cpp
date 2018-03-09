#include "main.h"
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("GSOF2EW");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Connects and converts gsof messages from trimble into EW Tracebuf");
    parser.addPositionalArgument("config_file", QCoreApplication::translate("main", ": Name of the EW Config file"));

    // Process the actual command line arguments given by the user
    parser.process(app);

    //remember to const this
    QStringList args = parser.positionalArguments();
    args.append("gsof2ew.d");

    if (args.size() < 1){
        fputs(qPrintable("Please pass the correct arguments"), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    }

    EWconnection = new EWconn(nullptr,args.first());
    usleep(3000);

    QHostAddress address(EWconnection->getHost());
    qint16 port = EWconnection->getPort();

    qDebug() << "Attaching to" << address.toString() << "at" << port;
    gps = new Client(address, port);

    QObject::connect(gps,SIGNAL(messageReceived(GPS_State)),EWconnection,SLOT(processState(GPS_State)));

    cout << "Hello World!" << endl;

    return app.exec();
}

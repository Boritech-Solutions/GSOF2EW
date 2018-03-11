#include "main.h"
using namespace std;

int main(int argc, char *argv[])
{
    // Initialize the application
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("GSOF2EW");
    QCoreApplication::setApplicationVersion("1.0");
    const qint64 pid = app.applicationPid();

    // Set up CLI Parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Connects and converts gsof messages from trimble into EW Tracebuf\n");
    parser.addPositionalArgument("config_file", QCoreApplication::translate("main", ": Name of the EW Config file"));

    // Process the actual command line arguments given by the user
    parser.process(app);

    // Get argument
    const QStringList args = parser.positionalArguments();

    // Past the correct arguments
    if (args.size() < 1){
        qDebug () << "Please pass the correct arguments\n";
        qDebug () << parser.helpText();
        return 1;
    }

    // Initialize the EW connection
    EWconnection = new EWconn(nullptr, args.first(), pid);
    sleep_ew(3000);
    //usleep(3000);

    if(EWconnection->isConn()){
        // Setup address and port
        QHostAddress address(EWconnection->getHost());
        qint16 port = EWconnection->getPort();
        bool velocity = EWconnection->getVelF();

        // Check for debug
        qDebug() << "Attaching to" << address.toString() << "at" << port;
        gps = new Client(address, port, velocity);
        if (gps->isconn())
            QObject::connect(gps,SIGNAL(messageReceived(GPS_State)),EWconnection,SLOT(processState(GPS_State)));

    }

    cout << "Hello World!" << endl;

    return app.exec();
}

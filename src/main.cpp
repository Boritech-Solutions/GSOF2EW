#include "main.h"
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("GSOF2EW");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Connects and converts gsof messages from trimble into EW Tracebuf");
    parser.addPositionalArgument("Address", QCoreApplication::translate("main", ": IP address of the target GPS"));
    parser.addPositionalArgument("Port", QCoreApplication::translate("main", ": Port of the target GPS"));

    // Process the actual command line arguments given by the user
    parser.process(app);

    //remember to const this
    QStringList args = parser.positionalArguments();
    args.append("127.0.0.1");
    args.append("10000");

    if (args.size() < 1){
        fputs(qPrintable("Please pass the correct arguments"), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    }
    QHostAddress address(args.at(0));
    qint16 port(args.at(1).toInt());

    qDebug() << "Attaching to" << address.toString() << "at" << port;
    gps = new Client(address, port);


    while(true){
        ;
    }

    cout << "Hello World!" << endl;
    return 0;
}

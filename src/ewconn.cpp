#include "ewconn.h"

EWconn::EWconn(QObject *parent,QString configfile) : QObject(parent), config(configfile)
{
    connected = false;
    velocity  = false;
    connectToEw();
}

int EWconn::get_config(char *configfile)
{
    int      nfiles;
    char     init[9];

    /* Open the main configuration file
     * ********************************/
    nfiles = k_open( configfile );
    if ( nfiles == 0 )
    {
        qDebug() << "Error opening configuration file";
        return -1;
    }

    /* Process all nested configuration files
     * **************************************/
    while ( nfiles > 0 )          /* While there are config files open */
    {
        while ( k_rd() )           /* Read next line from active file  */
        {
            int  success;
            char *com;
            char *str;

            com = k_str();          /* Get the first token from line */

            if ( !com ) continue;             /* Ignore blank lines */
            if ( com[0] == '#' ) continue;    /* Ignore comments */

            /* Open another configuration file
             * *******************************/
            if ( com[0] == '@' )
            {
                success = nfiles + 1;
                nfiles  = k_open( &com[1] );
                if ( nfiles != success )
                {
                    qDebug() << "Error opening command file" << QString(com[1]);
                    return -1;
                }
                continue;
            }
            /* Read configuration parameters
             * *****************************/

            else if ( k_its( "TcpAddr" ) )
            {
                if ( (str = k_str()) != 0 )
                {
                    add = QHostAddress(str);
                    init[0] = 1;
                }
                /* Null string; error picked up later */
            }

            else if ( k_its( "TcpPort" ) )
            {
                if((port=k_long()) < 1024L || port > 100000L)
                {
                    qDebug() <<"TcpPort value ("<<port<<") out of range\n";
                    return -1;
                }
                init[1] = 1;
            }

            else if ( k_its( "ModuleId" ) )
            {
                unsigned char gcfg_module_idnum;
                if ( (str = k_str()) != 0 )
                {
                    /* copy module name; make sure NULL terminated */
                    mod_name = QString(str);
                    if ( GetModId(str, &gcfg_module_idnum) == -1 )
                    {
                        qDebug() << "Invalid ModuleId Please Register ModuleId <%s> in earthworm.d!";
                        return -1;
                    }
                    mod_id = gcfg_module_idnum;
                }
                init[2] = 1;
            }

            else if ( k_its( "RingName" ) )
            {
                long gcfg_ring_key;
                if ( (str = k_str()) != NULL )
                {
                    /* copy ring name; make sure NULL terminated */
                    ring_name = QString(str);
                    if ( (gcfg_ring_key = GetKey(str)) == -1 )
                    {
                        qDebug() << "Invalid RingName" ;
                        return -1;
                    }
                    ring_id = gcfg_ring_key;
                }
                init[3] = 1;
            }

            else if ( k_its( "HeartbeatInt" ) )
            {
                heartbeat = k_int();
                init[4] = 1;
            }

            else if ( k_its( "SampRate" ) )
            {
                sampler = k_int();
                init[5] = 1;
            }

            else if ( k_its( "LogFile" ) )
            {
                logf = k_int();
                init[6] = 1;
            }

            else if ( k_its( "StationID" ) ){
                /* copy stationID name; make sure NULL terminated */
                staID = QString(k_str());
                init[7] = 1;
            }

            /* Optional Commands */
            else if ( k_its( "Network" ) )
            {
                netID = QString(k_str());
                init[8] = 1;
            }

            else if (k_its( "Debug" ) )
            {
                debug = k_int();
                init[9] = 1;
            }
            else if (k_its( "InjectVel" ) )
            {
                velocity = true;
            }

            else
            {
                /* An unknown parameter was encountered */
                qDebug() << "unknown parameter in GPS Config File";
                return -1;
            }

            /* See if there were any errors processing the command
       ***************************************************/
            if ( k_err() )
            {
                qDebug() << "Bad <%s> command in GPS Config File";
                return -1;
            }
        }
        nfiles = k_close();
    }

    /* After all files are closed, check flags for missed required commands
           ***********************************************************/
    int nmiss = 0;
    for ( int i = 0; i < 8; i++ )
        if ( !init[i] )
            nmiss++;

    if ( nmiss > 0 )
    {
        qDebug() << "ERROR, no ";
        if ( !init[0] ) qDebug() <<"<TcpAddr>" ;
        if ( !init[1] ) qDebug() <<"<TcpPort>" ;
        if ( !init[2] ) qDebug() <<"<ModuleId>";
        if ( !init[3] ) qDebug() <<"<RingName>";
        if ( !init[4] ) qDebug() <<"<HeartbeatInt>";
        if ( !init[5] ) qDebug() <<"<SampRate>";
        if ( !init[6] ) qDebug() <<"<LogFile> ";
        if ( !init[7] ) qDebug() <<"<StaID> ";
        if ( !init[8] ) qDebug() <<"<Network> ";
        if ( !init[9] ) qDebug() <<"<Debug> ";
        qDebug() <<"configuration detected";
        return -1;
    }
    return 0;
}

void EWconn::appendlog(QString status)
{
    qDebug() << status;
}

void EWconn::print2sc(GPS_State state)
{
    mystate = state;
    qDebug() << mystate.location.latitude() << mystate.location.longitude() << mystate.location.altitude();
    qDebug() << mystate.ECEF.X << mystate.ECEF.Y << mystate.ECEF.Z;
    qDebug() << mystate.last_gps_time;
    qDebug() << mystate.time_week << mystate.time_week_ms;
    createPacket();
}

void EWconn::createPacket()
{
    for (int i=0; i < 3; i++){
        static int cd;
        char* chan;
        char* chanv;

        if (i == 0){
            chan  = "GPX";
            chanv = "GVX";
        }
        if (i == 1){
            chan  = "GPY";
            chanv = "GVY";
        }
        if (i == 2){
            chan  = "GPZ";
            chanv = "GVZ";
        }

        TracePacket ew_trace_pkt;
        MSG_LOGO logo;
        logo.type = TypeTraceBuf2;
        logo.mod = mod_id;
        logo.instid = InstId;
        memset(&ew_trace_pkt,0,sizeof(ew_trace_pkt));
        strncpy(ew_trace_pkt.trh2.sta,staID.toLocal8Bit().data(), TRACE2_STA_LEN-1);
        ew_trace_pkt.trh2.version[0]=TRACE2_VERSION0;
        ew_trace_pkt.trh2.version[1]=TRACE2_VERSION1;
        strcpy(ew_trace_pkt.trh2.datatype,"i4");   /* enter data type (Intel ints) */
        ew_trace_pkt.trh2.samprate = (double) sampler; /* enter GPS sample rate */
        ew_trace_pkt.trh2.nsamp = sampler;   /* enter GPS number of samples rate */

        if (sizeof(TRACE2_HEADER) + ew_trace_pkt.trh2.nsamp * sizeof(int32_t) > MAX_TRACEBUF_SIZ)
        {
            /* exit thread function */
            qDebug() << "There will be an error";
            return;
        }

        strncpy(ew_trace_pkt.trh2.chan, chan, TRACE2_CHAN_LEN-1);
        ew_trace_pkt.trh2.chan[TRACE2_CHAN_LEN-1] = '\0';

        strncpy(ew_trace_pkt.trh2.net,netID.toLocal8Bit().data(), TRACE2_NET_LEN-1);
        ew_trace_pkt.trh2.loc[TRACE2_LOC_LEN-1] = '\0';

        strncpy(ew_trace_pkt.trh2.loc,"--", TRACE2_LOC_LEN-1);
        ew_trace_pkt.trh2.loc[TRACE2_LOC_LEN-1] = '\0';

        double starttime = (double) mystate.last_gps_time.toSecsSinceEpoch();

        /* calculate and enter start-timestamp for packet */
        ew_trace_pkt.trh2.starttime = starttime;

        /* endtime is the time of last sample in this packet, not the time *
         * of the first sample in the next packet */
        ew_trace_pkt.trh2.endtime = ew_trace_pkt.trh2.starttime + (double)(ew_trace_pkt.trh2.nsamp - 1) / ew_trace_pkt.trh2.samprate;

        if (i == 0){
            /* copy payload of 32-bit ints into trace buffer (after header) */
            memcpy(&ew_trace_pkt.msg[sizeof(TRACE2_HEADER)],&mystate.ECEF.X , ew_trace_pkt.trh2.nsamp*sizeof(int32_t));
        }
        if (i == 1){
            /* copy payload of 32-bit ints into trace buffer (after header) */
            memcpy(&ew_trace_pkt.msg[sizeof(TRACE2_HEADER)],&mystate.ECEF.Y, ew_trace_pkt.trh2.nsamp*sizeof(int32_t));
        }
        if (i == 2){
            /* copy payload of 32-bit ints into trace buffer (after header) */
            memcpy(&ew_trace_pkt.msg[sizeof(TRACE2_HEADER)],&mystate.ECEF.Z, ew_trace_pkt.trh2.nsamp*sizeof(int32_t));
        }

        sleep_ew(10);/* Take a short nap so we don't flood the transport ring */


        /* send data trace message to Earthworm */
        if ( (cd = tport_putmsg(&region, &logo,
                                (int32_t)sizeof(TRACE2_HEADER) + (int32_t)ew_trace_pkt.trh2.nsamp * sizeof(int32_t),
                                (char *)&ew_trace_pkt)) != PUT_OK)
        {
            qDebug() << "There has been an error";
            return;
        }
    }
}


/************************************************************
 *               Connect to Earthworm                       *
 * Should be used to connect to earthworm                   *
 * *********************************************************/
int EWconn::connectToEw() {
    // Set Stuff
    char *runPath;

    appendlog(QString("using default config file: ") + QString(config));

    /* Change working directory to environment variable EW_PARAMS value
        *********************************************************************/
    runPath = getenv( "EW_PARAMS" );

    if ( runPath == NULL ) {
        appendlog("status: Environment variable EW_PARAMS not defined.");
        appendlog(" Exiting.\n");
        return -1;
    }

    if ( *runPath == '\0' ) {
        appendlog("status: Environment variable EW_PARAMS ");
        appendlog("defined, but has no value. Exiting.\n");
        return -1;
    }

    if ( chdir_ew( runPath ) == -1 ) {
        appendlog(QString("status: Params directory not found: ") + QString(runPath) + "\n");
        appendlog(QString("status: Reset environment variable EW_PARAMS."));
        appendlog(QString("Exiting.\n"));
        return -1;
    }

    /* Look up ids in earthworm.d tables
           ***********************************/
    if ( GetLocalInst( &InstId ) != 0 ) {
        appendlog("status: error getting local installation id; exiting!\n");
        return -1;
    }

    if ( GetType( "TYPE_TRACEBUF2", &TypeTraceBuf2 ) != 0 ) {
        appendlog("status: Invalid message type <TYPE_TRACEBUF2> exiting!\n");
        return -1;
    }

    if( get_config(config.toLocal8Bit().data()) == -1)
        return -1;

    /* Attach to shared memory ring
        *****************************/
    tport_attach( &region, ring_id );

    connected = true;
    return 0;
}

/************************************************************
 *               Disconnect From Earthworm                  *
 * Should be used to disconnect to earthworm                *
 * *********************************************************/
int EWconn::disconnectFromEw(){
    if (connected){
        tport_detach( &region );
        appendlog("Successful Disconnection");
        return 0;
    } else {
        appendlog("Not Connected");
        return 0;
    }
}


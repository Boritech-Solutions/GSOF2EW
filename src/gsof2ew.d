#
# gsof2ew configuration file
#

TcpAddr         127.0.0.1       # IP address of GPS
TcpPort         10000           # TCP port number of GPS

ModuleId        MOD_K2EW        # module id for this import
RingName        WAVE_RING       # transport ring to use for input/output

LogFile         1               # If 0, don't output to logfile; if 1, do
                                # if 2, log to module log but not stderr/stdout

HeartbeatInt    30              # Heartbeat interval in seconds

Network         PR              # Network name for EW msg headers

StationID       PRSN		        # GPS Name

Debug 0                         # debug level: 0 or commented out for no debug
                                # logging; 1 - 4 for increasing verbosity
                                # default: no debug output
                                
#InjectVel                      # inject GPS Velocity

SampRate       1                # GPS Samplerate in Hz

#SubX           0               # Subtract from X value 
#SubY           0               # Subtract from Y value
#SubZ           0               # Subtract from Z value

#ChannelNames	  GPX,GPY,GPZ	    # GPS Channels
#
#LocationNames   --,--,--	      # If present, assign location codes to
#				                        # the TRACEBUF packets emitted by gps2ew.

#ifndef S7TYPES_H
#define S7TYPES_H

//#define BCCWIN 		// if you work on windows

//#define HANDLE int
#define tmotype int
#define daveMaxRawLen 2048
#define daveProtoISOTCP	122	/* ISO over TCP */
#define daveProtoISOTCP243 123	/* ISO over TCP with CP243 */
/*
 *    ProfiBus speed constants:
*/
#define daveSpeed9k     0
#define daveSpeed19k    1
#define daveSpeed187k   2
#define daveSpeed500k   3
#define daveSpeed1500k  4
#define daveSpeed45k    5
#define daveSpeed93k    6

#define daveDB 0x84	/* data blocks */

/*    Some definitions for debugging:   */
#define daveDebugRawRead  	0x01	/* Show the single bytes received */
#define daveDebugSpecialChars  	0x02	/* Show when special chars are read */
#define daveDebugRawWrite	0x04	/* Show the single bytes written */
#define daveDebugListReachables 0x08	/* Show the steps when determine devices in MPI net */
#define daveDebugInitAdapter 	0x10	/* Show the steps when Initilizing the MPI adapter */
#define daveDebugConnect 	0x20	/* Show the steps when connecting a PLC */
#define daveDebugPacket 	0x40
#define daveDebugByte 		0x80
#define daveDebugCompare 	0x100
#define daveDebugExchange 	0x200
#define daveDebugPDU 		0x400	/* debug PDU handling */
#define daveDebugUpload		0x800	/* debug PDU loading program blocks from PLC */
#define daveDebugMPI 		0x1000
#define daveDebugPrintErrors	0x2000	/* Print error messages */
#define daveDebugPassive 	0x4000

#define daveDebugErrorReporting	0x8000
#define daveDebugOpen		0x10000  /* print messages in openSocket and setPort */

#define daveDebugAll 0x1ffff

#define daveProtoISOTCPR 124	/* ISO over TCP with Routing */

typedef struct dost {
    int rfd;
    int wfd;
//    int connectionType;
} _daveOSserialType;

typedef struct _daveConnection  daveConnection;
typedef struct _daveInterface  daveInterface;

typedef struct _daveS5AreaInfo  {
    int area;
    int DBnumber;
    int address;
    int len;
    struct _daveS5AreaInfo * next;
} daveS5AreaInfo;

typedef struct _daveS5cache {
    int PAE;	// start of inputs
    int PAA;	// start of outputs
    int flags;	// start of flag (marker) memory
    int timers;	// start of timer memory
    int counters;// start of counter memory
    int systemData;// start of system data
    daveS5AreaInfo * first;
} daveS5cache;

typedef struct {
    unsigned char * header;	/* pointer to start of PDU (PDU header) */
    unsigned char * param;		/* pointer to start of parameters inside PDU */
    unsigned char * data;		/* pointer to start of data inside PDU */
    unsigned char * udata;		/* pointer to start of data inside PDU */
    int hlen;		/* header length */
    int plen;		/* parameter length */
    int dlen;		/* data length */
    int udlen;		/* user or result data length */
} PDU;

struct _daveConnection {
    int AnswLen;	/* length of last message */
    unsigned char * resultPointer;	/* used to retrieve single values from the result byte array */
    int maxPDUlength;
    int MPIAdr;		/* The PLC's address */
    daveInterface * iface; /* pointer to used interface */
    int needAckNumber;	/* message number we need ackknowledge for */
    int PDUnumber; 	/* current PDU number */
    int ibhSrcConn;
    int ibhDstConn;
    unsigned char msgIn[daveMaxRawLen];
    unsigned char msgOut[daveMaxRawLen];
    unsigned char * _resultPointer;
    int PDUstartO;	/* position of PDU in outgoing messages. This is different for different transport methodes. */
    int PDUstartI;	/* position of PDU in incoming messages. This is different for different transport methodes. */
    int rack;		/* rack number for ISO over TCP */
    int slot;		/* slot number for ISO over TCP */
    int connectionNumber;
    int connectionNumber2;
    unsigned char 	messageNumber;  /* current MPI message number */
    unsigned char	packetNumber;	/* packetNumber in transport layer */
    void * hook;	/* used in CPU/CP simulation: pointer to the rest we have to send if message doesn't fit in a single packet */
    daveS5cache * cache; /* used in AS511: We cache addresses of memory areas and datablocks here */
};

typedef int (* _initAdapterFunc)(_daveInterface*);
typedef int (* _connectPLCFunc) (daveConnection *);
typedef int (* _disconnectPLCFunc) (daveConnection *);
typedef int (* _disconnectAdapterFunc) (daveInterface * );
typedef int (* _exchangeFunc) (_daveConnection *, PDU *);
typedef int (* _sendMessageFunc) (_daveConnection *, PDU *);
typedef int (* _getResponseFunc) (daveConnection *);
typedef int (* _listReachablePartnersFunc) (daveInterface * di, char * buf); // changed to unsigned char because it is a copy of an unsigned char buffer

typedef int (*  _writeFunc) (daveInterface *, char *, int); // changed to char because char is what system read/write expects
typedef int (*  _readFunc) (daveInterface *, char *, int);

struct _daveInterface {
    tmotype timeout;	/* Timeout in microseconds used in transort. */
    _daveOSserialType fd; /* some handle for the serial interface */
    int localMPI;	/* the adapter's MPI address */

    int users;		/* a counter used when multiple PLCs are accessed via */
                        /* the same serial interface and adapter. */
    char * name;	/* just a name that can be used in programs dealing with multiple */
                        /* daveInterfaces */
    int protocol;	/* The kind of transport protocol used on this interface. */
    int speed;		/* The MPI or Profibus speed */
    int ackPos;		/* position of some packet number that has to be repeated in ackknowledges */
    int nextConnection;
    _initAdapterFunc initAdapter;		/* pointers to the protocol */
    _connectPLCFunc connectPLC;			/* specific implementations */
    _disconnectPLCFunc disconnectPLC;		/* of these functions */
    _disconnectAdapterFunc disconnectAdapter;
    _exchangeFunc exchange;
    _sendMessageFunc sendMessage;
    _getResponseFunc getResponse;
    _listReachablePartnersFunc listReachablePartners;
    char realName[20];
    _readFunc ifread;
    _writeFunc ifwrite;
    int seqNumber;
};

#endif // S7TYPES_H

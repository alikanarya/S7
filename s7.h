#ifndef S7_H
#define S7_H

#define MESSAGE0 "K�t�phane y�klendi"
#define MESSAGE1 "K�t�phane y�klenemedi!!! PLC kontrol kullan�lamayacakt�r."
#define MESSAGE2 "PLC ba�lant�s� OK....."
#define MESSAGE3 "PLC'YE   B A � L A N I L A M I Y O R !!!"
#define MESSAGE4 "PLC B A � L A N T I S I   K E S � L D � !!!"
#define MESSAGE5 "Teknik Servis ile irtibata ge�iniz."
#define MESSAGE6 "PLC ba�lant�s� kuruldu"

#include <QLibrary>

#include "s7types.h"

#define ERROR_CODE -12345

class s7{
    public:

        QLibrary *library;          // libnodave.dll library
        bool libraryLoaded;         // true: library loaded successfully
        bool socketOpened;          // true: tcp socket opened
        bool adapterInited;         // true: net. adapter initilized
        bool plcConnected;          // true: plc connection is made
        bool plcInteract;           // true: plc can be accessed to read/write opearions.

        const char* peer;           // plc ip address
        int plcType;
        int plcProtocol;

        // libnodave vars
        _daveOSserialType fds;      // about socket
        daveInterface *intf;        // about interface
        daveConnection *conn;       // about connection

        QByteArray *buffer;         // buffer for red data  ??
        QByteArray *bufferPtr;      // buffer pointer       ??
        int readResult;             // read op. result

        QString message;            // status message of class



        s7();                               // constructor
        void changeType(int type);
        bool connect(const char* _peer);    // connect to plc
        bool disconnect();                  // disconnect from plc

        int readBits(int db, int byte, int length);     // read bits from plc
        int setBit(int db, int byte, int bit);          // set bit of plc
        int clrBit(int db, int byte, int bit);          // clear bit of plc

    private:

        /* library function pointers________________________________________________________________ */
        typedef int (*openSocketPtr)(const int,const char*);
        openSocketPtr openSocket;

        typedef daveInterface* (*daveNewInterfacePtr) (_daveOSserialType, char*, int, int, int);
        daveNewInterfacePtr daveNewInterface;

        typedef int (*daveInitAdapterPtr) (daveInterface*);
        daveInitAdapterPtr daveInitAdapter;

        typedef int (*daveConnectPLCPtr) (daveConnection*);
        daveConnectPLCPtr daveConnectPLC;

        typedef daveConnection* (*daveNewConnectionPtr) (daveInterface*, int, int, int);
        daveNewConnectionPtr daveNewConnection;

        typedef int (*closePortPtr) (int port);
        closePortPtr closePort;

        typedef void (*daveFreePtr) (void*);
        daveFreePtr daveFree;

        typedef int (*daveDisconnectAdapterPtr) (daveInterface*);
        daveDisconnectAdapterPtr daveDisconnectAdapter;

        typedef int (*daveDisconnectPLCPtr) (daveConnection*);
        daveDisconnectPLCPtr daveDisconnectPLC;

        typedef int (*daveReadBytesPtr) (daveConnection*, int, int, int, int, void*);
        daveReadBytesPtr daveReadBytes;

        typedef float (*daveGetFloatfromPtr) (unsigned char*);
        daveGetFloatfromPtr daveGetFloatfrom;

        typedef float (*daveGetFloatPtr) (daveConnection*);
        daveGetFloatPtr daveGetFloat;

        typedef int (*daveReadBitsPtr) (daveConnection*, int, int, int, int, void*);
        daveReadBitsPtr daveReadBits;

        typedef int (*daveSetBitPtr) (daveConnection*, int, int, int , int);
        daveSetBitPtr daveSetBit;

        typedef int (*daveClrBitPtr) (daveConnection*, int, int, int, int);
        daveClrBitPtr daveClrBit;
        /* _________________________________________________________________________________________ */

};

#endif // S7_H

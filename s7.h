#ifndef S7_H
#define S7_H

#define MESSAGE0 "Kütüphane yüklendi"
#define MESSAGE1 "Kütüphane yüklenemedi!!! PLC kontrol kullanılamayacaktır."
#define MESSAGE2 "PLC bağlantısı OK....."
#define MESSAGE3 "PLC'YE BAĞLANILAMIYOR!!!"
#define MESSAGE4 "PLC BAĞLANTISI KESİLDİ!!!"
#define MESSAGE5 "Teknik Servis ile irtibata geçiniz."
#define MESSAGE6 "PLC bağlantısı kuruldu"

#include <QLibrary>

#include "s7types.h"
//#include "nodave.h"

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
        int portNum;                // socket port number
        int plcType;
        int plcProtocol;

        // libnodave vars
        _daveOSserialType fds;      // about socket
        daveInterface *intf;        // about interface
        daveConnection *conn;       // about connection

        int readResult;             // read op. result
        QString message = ".";      // status message of class

        s7();                               // constructor
        void changeType(int type);
        bool connect(const char* _peer);    // connect to plc
        bool disconnect();                  // disconnect from plc

        int readBits(int db, int byte, int length);     // read bits from plc
        int setBit(int db, int byte, int bit);          // set bit of plc
        int clrBit(int db, int byte, int bit);          // clear bit of plc
        int readBytes(int db, int start, int length, unsigned char* buffer);    // receive bytes from plc
        int getS16(unsigned char* buffer);  // convert 2-bytes (big-endian) to SIGNED INTEGER
        float getFloat(unsigned char* buffer);  // convert 4-bytes (big-endian) to FLOAT
        unsigned char* putFloat(unsigned char* buffer, float var);  // convert float to 4-BYTES (big-endian)
        int writeBytes(int db, int start, int length, unsigned char* buffer);
        int writeBits(int db, int start, int length, void* buffer);

        daveInterface* daveNewInterfaceXYZ(_daveOSserialType nfd, char * nname, int localMPI, int protocol, int speed){
            daveInterface * di=(daveInterface *) calloc(1, sizeof(daveInterface));
        /*
            LOG7("daveNewInterface(fd.rfd:%d fd.wfd:%d name:%s local MPI:%d protocol:%d PB speed:%d)\n",
            nfd.rfd,nfd.wfd,nname, localMPI, protocol, speed);
            FLUSH;
        */
            if (di) {
        //	di->name=nname;
            strncpy(di->realName,nname,20);
            di->name=di->realName;
            di->fd=nfd;
            di->localMPI=localMPI;
            di->protocol=protocol;
            di->timeout=1000000; /* 1 second */
            di->nextConnection=0x14;
            di->speed=speed;

            di->getResponse=_daveGetResponseISO_TCP;
            di->ifread=stdread;
            di->ifwrite=stdwrite;
            di->initAdapter=_daveReturnOkDummy;
            di->connectPLC=_daveReturnOkDummy2;
            di->disconnectPLC=_daveReturnOkDummy2;
            di->disconnectAdapter=_daveReturnOkDummy;
            di->listReachablePartners=_daveListReachablePartnersDummy;
            switch (protocol) {

                case daveProtoISOTCP:
                case daveProtoISOTCP243:
                case daveProtoISOTCPR:	// routing over MPI network
                di->getResponse=_daveGetResponseISO_TCP;
                di->connectPLC=_daveConnectPLCTCP;
                di->exchange=_daveExchangeTCP;
                break;

            }
        //#ifdef BCCWIN
            //setTimeOut(di, di->timeout);
        //#endif
            }
            return di;
        }

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

        typedef int (*daveGetS16fromPtr) (unsigned char*);
        daveGetS16fromPtr daveGetS16from;

        typedef int (*daveWriteBytesPtr) (daveConnection*, int, int, int , int, void*);
        daveWriteBytesPtr daveWriteBytes;

        typedef int (*daveWriteBitsPtr) (daveConnection*, int, int, int , int, void*);
        daveWriteBitsPtr daveWriteBits;

        typedef unsigned char* (*davePutFloatPtr) (unsigned char*, float);
        davePutFloatPtr davePutFloat;

        typedef void (*daveSetDebugPtr) (int);
        daveSetDebugPtr daveSetDebug;

        typedef int (*_daveGetResponseISO_TCPPtr) (daveConnection*);
        _daveGetResponseISO_TCPPtr _daveGetResponseISO_TCP;

        typedef int (*stdreadPtr) (daveInterface*, char*, int);
        stdreadPtr stdread;

        typedef int (*stdwritePtr) (daveInterface*, char*, int);
        stdwritePtr stdwrite;

        typedef int (*_daveReturnOkDummyPtr) (daveInterface*);
        _daveReturnOkDummyPtr _daveReturnOkDummy;

        typedef int (*_daveReturnOkDummy2Ptr) (daveConnection*);
        _daveReturnOkDummy2Ptr _daveReturnOkDummy2;

        typedef int (*_daveListReachablePartnersDummyPtr) (daveInterface*, char*);
        _daveListReachablePartnersDummyPtr _daveListReachablePartnersDummy;

        typedef int (*_daveConnectPLCTCPPtr) (daveConnection*);
        _daveConnectPLCTCPPtr _daveConnectPLCTCP;

        typedef int (*_daveExchangeTCPPtr) (daveConnection*, PDU*);
        _daveExchangeTCPPtr _daveExchangeTCP;

        typedef void (*setTimeOutPtr) (daveInterface*, int);
        setTimeOutPtr setTimeOut;
        /* _________________________________________________________________________________________ */

};


#endif // S7_H

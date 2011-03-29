#include "s7.h"

#include <exception>
#include <QTextCodec>

using namespace std;

s7::s7(){
    changeType(0);  // S7-200

    libraryLoaded = false;
    socketOpened = false;
    adapterInited = false;
    plcConnected = false;
    plcInteract = false;
    library = new QLibrary("libnodave.dll");
    library->load();

    if (library->isLoaded()) libraryLoaded = true;

    message = "";

    // resolve functions from library
    if (libraryLoaded){
        openSocket = (openSocketPtr) library->resolve("openSocket");
        if (openSocket) message = "openSocket ok.....\n";
        else message = "openSocket fail.....\n";

        daveNewInterface = (daveNewInterfacePtr) library->resolve("daveNewInterface");
        if (daveNewInterface) message += "daveNewInterface ok.....\n";
        else message += "daveNewInterface fail.....\n";

        daveInitAdapter = (daveInitAdapterPtr) library->resolve("daveInitAdapter");
        if (daveNewInterface) message += "daveInitAdapter ok.....\n";
        else message += "daveInitAdapter fail.....\n";

        daveConnectPLC = (daveConnectPLCPtr) library->resolve("daveConnectPLC");
        if (daveConnectPLC) message += "daveConnectPLC ok.....\n";
        else message += "daveConnectPLC fail.....\n";

        daveNewConnection = (daveNewConnectionPtr) library->resolve("daveNewConnection");
        if (daveNewConnection) message += "daveNewConnection ok.....\n";
        else message += "daveNewConnection fail.....\n";

        closePort = (closePortPtr) library->resolve("closePort");
        if (closePort) message += "closePort ok.....\n";
        else message += "closePort fail.....\n";

        daveFree = (daveFreePtr) library->resolve("daveFree");
        if (daveFree) message += "daveFree ok.....\n";
        else message += "daveFree fail.....\n";

        daveDisconnectAdapter = (daveDisconnectAdapterPtr) library->resolve("daveDisconnectAdapter");
        if (daveDisconnectAdapter) message += "daveDisconnectAdapter ok.....\n";
        else message += "daveDisconnectAdapter fail.....\n";

        daveDisconnectPLC = (daveDisconnectPLCPtr) library->resolve("daveDisconnectPLC");
        if (daveDisconnectPLC) message += "daveDisconnectPLC ok.....\n";
        else message += "daveDisconnectPLC fail.....\n";

        daveReadBytes = (daveReadBytesPtr) library->resolve("daveReadBytes");
        if (daveReadBytes) message += "daveReadBytes ok.....\n";
        else message += "daveReadBytes fail.....\n";

        daveGetFloatfrom = (daveGetFloatfromPtr) library->resolve("daveGetFloatfrom");
        if (daveGetFloatfrom) message += "daveGetFloatfrom ok.....\n";
        else message += "daveGetFloatfrom fail.....\n";

        daveGetFloat = (daveGetFloatPtr) library->resolve("daveGetFloat");
        if (daveGetFloat) message += "daveGetFloat ok.....\n";
        else message += "daveGetFloat fail.....\n";

        daveReadBits = (daveReadBitsPtr) library->resolve("daveReadBits");
        if (daveReadBits) message += "daveReadBits ok.....\n";
        else message += "daveReadBits fail.....\n";

        daveSetBit = (daveSetBitPtr) library->resolve("daveSetBit");
        if (daveSetBit) message += "daveSetBit ok.....\n";
        else message += "daveSetBit fail.....\n";

        daveClrBit = (daveClrBitPtr) library->resolve("daveClrBit");
        if (daveClrBit) message += "daveClrBit ok.....\n";
        else message += "daveClrBit fail.....\n";

        daveGetS16from = (daveGetS16fromPtr) library->resolve("daveGetS16from");
        if (daveGetFloatfrom) message += "daveGetS16from ok.....\n";
        else message += "daveGetS16from fail.....\n";

        davePutFloat = (davePutFloatPtr) library->resolve("davePutFloat");
        if (davePutFloat) message += "davePutFloat ok.....\n";
        else message += "davePutFloat fail.....\n";

        daveWriteBytes = (daveWriteBytesPtr) library->resolve("daveWriteBytes");
        if (daveWriteBytes) message += "daveWriteBytes ok.....\n";
        else message += "daveWriteBytes fail.....\n";

        daveWriteBits = (daveWriteBitsPtr) library->resolve("daveWriteBits");
        if (daveWriteBits) message += "daveWriteBits ok.....\n";
        else message += "daveWriteBits fail.....\n";

        message += "------------------------------------------";

        message = MESSAGE0;
    } else
        message = MESSAGE1;

    portNum = 102;
}

void s7::changeType(int type){
    plcType = type;

    if (plcType == 0)                       // S7-200
        plcProtocol = daveProtoISOTCP243;
    else if (plcType == 1)                  // S7-300
        plcProtocol = daveProtoISOTCP;
    else {                                  // unknown
        plcType = 1;
        plcProtocol = daveProtoISOTCP;
    }
}

bool s7::connect(const char* _peer){
    message = "";
    peer = _peer;
    if (libraryLoaded){
        disconnect();   // if some connection before

        if (!socketOpened && !adapterInited && !plcConnected){
            fds.rfd = openSocket(portNum, peer);     // open socket
            fds.wfd = fds.rfd;

            message += " rfd: " + QString::number(fds.rfd) + ".....\n";

            char x = '_';
            char* _if1 = &x;

            if (fds.rfd > 0){   // socked openned
                socketOpened = true;
                intf = daveNewInterface(fds, _if1, 0, plcProtocol, daveSpeed187k);     // make a new interface
                intf->timeout = 100000;      // timeout 100.000 nanosec = 100 msec

                if (daveInitAdapter(intf) == 0){    // adapter initialized with the interface
                    adapterInited = true;
                    message += "init adapter ok.....\n";

                    conn = daveNewConnection(intf, 2, 0, 2);    // make a new connection
                    if (daveConnectPLC(conn) == 0){             // connect to plc
                        plcConnected = true;
                        message = MESSAGE2;
                    } else
                        message = MESSAGE3;
                }
            }
        }
    }

    plcInteract = libraryLoaded && socketOpened && adapterInited && plcConnected; // if all steps are ok
    return plcInteract;
}

int s7::readBits(int db, int byte, int length){
    int status = ERROR_CODE;
    message = "readbits: not-interact";

    if (plcInteract){
        message = "readbits: interact";
        try{
            readResult = daveReadBits(conn, daveDB, db, byte, length, NULL);
            status = readResult;
            message += "read bits status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

int s7::setBit(int db, int byte, int bit){
    int status = ERROR_CODE;
    message = "";

    if (plcInteract){
        try{
            readResult = daveSetBit(conn, daveDB, db, byte, bit);
            status = readResult;
            message += "set bit status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

int s7::clrBit(int db, int byte, int bit){
    int status = ERROR_CODE;
    message = "";

    if (plcInteract){
        try{
            readResult = daveClrBit(conn, daveDB, db, byte, bit);
            status = readResult;
            message += "clear bit status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

int s7::readBytes(int db, int start, int length, unsigned char* buffer){
    int status = ERROR_CODE;
    message = "readbytes: not-interact";

    if (plcInteract){
        message = "readbytes: interact";
        try{
            readResult = daveReadBytes(conn, daveDB, db, start, length, buffer);
            status = readResult;
            message += "readbytes status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

int s7::getS16(unsigned char* buffer){
    int status = ERROR_CODE;
    message = "getS16: not-interact";

    if (plcInteract){
        message = "getS16: interact";
        try{
            readResult = daveGetS16from(buffer);
            status = readResult;
            message += "getS16 status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

float s7::getFloat(unsigned char* buffer){
    float status = ERROR_CODE;
    message = "getFloat: not-interact";

    if (plcInteract){
        message = "getFloat: interact";
        try{
            status = daveGetFloatfrom(buffer);
            message += "getFloat status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

int s7::writeBytes(int db, int start, int length, unsigned char* buffer){
    int status = ERROR_CODE;
    message = "writeBytes: not-interact";

    if (plcInteract){
        message = "writeBytes: interact";
        try{
            readResult = daveWriteBytes(conn, daveDB, db, start, length, buffer);
            status = readResult;
            message += "writeBytes status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

int s7::writeBits(int db, int start, int length, void* buffer){
    int status = ERROR_CODE;
    message = "writeBits: not-interact";

    if (plcInteract){
        message = "writeBits: interact";
        try{
            readResult = daveWriteBits(conn, daveDB, db, start, length, buffer);
            status = readResult;
            message += "writeBits status: " + QString::number(status);

        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return status;
}

unsigned char* s7::putFloat(unsigned char* buffer, float var){
    unsigned char* bytes = new unsigned char;

    message = "putFloat: not-interact";

    if (plcInteract){
        message = "putFloat: interact";
        try{
            bytes = davePutFloat(buffer, var);
        } catch (exception& e){
            message += "READ ERROR....."+QString::fromUtf8(e.what());
        }
    }
    return bytes;
}

bool s7::disconnect(){
    message = "";

    if (plcConnected){
        daveDisconnectPLC(conn);
        //daveFree(conn);
        plcConnected = false;
    }

    if (adapterInited){
        daveDisconnectAdapter(intf);
        //daveFree(intf);
        adapterInited = false;
    }

    if (socketOpened){
        closePort(fds.rfd);
        socketOpened = false;
    }

    plcInteract = false;
    message = MESSAGE4;
    return plcInteract;
}

//readResult = daveReadBytes(conn,daveDB,1,5000,4,NULL);
//var = daveGetFloat(conn); double v = var; message += "V-MEM: " + QString::number(v) + ".....\n";

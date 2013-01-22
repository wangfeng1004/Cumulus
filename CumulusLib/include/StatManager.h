/* 
	Copyright 2010 OpenRTMFP
 
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License received along this program for more
	details (or else see http://www.gnu.org/licenses/).

	This file is a part of Cumulus.
*/

#pragma once

#include "Cumulus.h"
#include "Startable.h"
#include "SocketHandler.h"
#include <map>
#include <string>
#include "Poco/AtomicCounter.h"
#include "AtomicCounter64.h"
#include "RTMFPServer.h"

#define PACKETRECV_SIZE     2048

namespace Cumulus {

class StatData {
public:
    // error
    AtomicCounter64             _timeoutKeepalive;
    AtomicCounter64             _timeoutConnection;
    AtomicCounter64             _failOnClient;
    AtomicCounter64             _unknownSession;
    AtomicCounter64             _decryptError;
    //AtomicCounter64             _flowWriterFail;
    //AtomicCounter64             _flowUnhandleMsg;

    // normal
    AtomicCounter64             _recvPackets;
    AtomicCounter64             _sendPackets;
    AtomicCounter64             _recvAccDuration;
    AtomicCounter64             _sendAccDuration;
    AtomicCounter64             _recvPeakCost;
    AtomicCounter64             _sendPeakCost;
    AtomicCounter64             _handShake;
    AtomicCounter64             _keepAlive;

    void init(){
        _timeoutKeepalive = 0;
        _timeoutConnection = 0;
        _failOnClient = 0;
        _unknownSession = 0;
        _decryptError = 0;

        //_flowWriterFail = 0;
        //_flowUnhandleMsg = 0;

        _recvPackets = 0;
        _sendPackets = 0;
        _handShake = 0;
        _keepAlive = 0;
        _recvAccDuration = 0;
        _sendAccDuration = 0;
        _recvPeakCost = 0;
        _sendPeakCost = 0;
    }

    void clear(){
        init();
    }

	StatData& operator = (StatData &o){
        _timeoutKeepalive = o._timeoutKeepalive;
        _timeoutConnection = o._timeoutConnection;
        _failOnClient = o._failOnClient;
        _unknownSession = o._unknownSession;
        _decryptError = o._decryptError;
        _recvPackets = o._recvPackets;
        _sendPackets = o._sendPackets;
        _handShake = o._handShake;
        _keepAlive = o._keepAlive;
        _recvAccDuration = o._recvAccDuration;
        _sendAccDuration = o._sendAccDuration;
        _recvPeakCost.CompBigAndSwap(o._recvPeakCost);
        _sendPeakCost.CompBigAndSwap(o._sendPeakCost);
    };

	void operator += (StatData &o){
        _timeoutKeepalive += o._timeoutKeepalive;
        _timeoutConnection += o._timeoutConnection;
        _failOnClient += o._failOnClient;
        _unknownSession += o._unknownSession;
        _decryptError += o._decryptError;
        _recvPackets += o._recvPackets;
        _sendPackets += o._sendPackets;
        _handShake += o._handShake;
        _keepAlive += o._keepAlive;
        _recvAccDuration += o._recvAccDuration;
        _sendAccDuration += o._sendAccDuration;
        _recvPeakCost.CompBigAndSwap(o._recvPeakCost);
        _sendPeakCost.CompBigAndSwap(o._sendPeakCost);
    };

    std::string format_string();
};

class StatManager : public Startable, public SocketHandler {
public:
	StatManager(const std::string& name="StatManager");
	virtual ~StatManager();

	void                        launch();
	void                        status_string(std::string &s);
    void                        setRTMFPServer(RTMFPServer *server);


    static StatManager          global;
    StatData                    stat_data,last_stat_data,acc_stat_data;
private:
	void					    run();
    void                        clear();
	void					    checkCmd(Poco::Net::Socket& socket);
    void                        onReadable(Poco::Net::Socket& socket);
    void                        onError(const Poco::Net::Socket& socket,const std::string& error);

    std::string                 _startDatetimeStr;
	Poco::Mutex				    _mutex;
    Poco::Net::SocketAddress    _address;
    char                        _buf[PACKETRECV_SIZE];

    RTMFPServer                 *_server;
} ;

} // namespace Cumulus

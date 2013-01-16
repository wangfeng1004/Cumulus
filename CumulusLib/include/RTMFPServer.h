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
#include "Handshake.h"
#include "Gateway.h"
#include "Sessions.h"
#include "Startable.h"
#include "Handler.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"


namespace Cumulus {

class RTMFPServerParams {
public:
	RTMFPServerParams() : port(RTMFP_DEFAULT_PORT),udpBufferSize(0),threadPriority(Poco::Thread::PRIO_HIGH),pCirrus(NULL),middle(false),keepAlivePeer(10),keepAliveServer(15), shellPort(0) {	
	}
	Poco::UInt16				port;
	Poco::UInt32				udpBufferSize;
	bool						middle;
	Poco::Net::SocketAddress*	pCirrus;
	Poco::Thread::Priority		threadPriority;

	Poco::UInt16				keepAlivePeer;
	Poco::UInt16				keepAliveServer;
	Poco::UInt16 				shellPort;
};

class MainSockets : public SocketManager,private TaskHandler {
public:
	MainSockets():SocketManager((TaskHandler&)*this,"MainSockets") {}
	virtual ~MainSockets(){}
private:
	void requestHandle(){giveHandleEx();}
};


class RTMFPServer : private Gateway,protected Handler,private Startable,private SocketHandler {
	friend class RTMFPManager;
	friend class RTMFPReceiving;
public:
	RTMFPServer(Poco::UInt32 threads=0);
	virtual ~RTMFPServer();

	void start();
	void start(RTMFPServerParams& params);
	void stop();
	Poco::UInt16 port();
	bool running();

	const Poco::Net::DatagramSocket & shellSocket();

	void status_string(std::string & s); 

protected:
	virtual void    manage();

public:
	Poco::Int64 rcvpTm, rcvpCnt, psndTm, psndCnt;
	Poco::Int64 peakRcvp, peakPsnd;

private:
	RTMFPServer(const std::string& name,Poco::UInt32 cores);
	virtual void    onStart(){}
	virtual void    onStop(){}
	void			requestHandle();
	virtual void	handle(bool& terminate);

	void			receive(RTMFPReceiving * rtmfpReceiving);
	void			run();
	Poco::UInt8		p2pHandshake(const std::string& tag,PacketWriter& response,const Poco::Net::SocketAddress& address,const Poco::UInt8* peerIdWanted);
	Session&		createSession(const Peer& peer,Cookie& cookie);
	void			destroySession(Session& session);

	void			onReadable(Poco::Net::Socket& socket);
	void			onError(const Poco::Net::Socket& socket,const std::string& error);

	void handleShellCommand(RTMFPReceiving * received);

	Handshake					_handshake;

	Poco::UInt16				_port;
	Poco::Net::DatagramSocket	* _pSocket;

	Poco::UInt16 _shellPort;
	Poco::Net::DatagramSocket _shellSocket;
	std::string _startDatetimeStr;

	bool							_middle;
	Target*							_pCirrus;
	Sessions						_sessions;
//	MainSockets						_mainSockets;
	int  tm_5m;	
};

inline Poco::UInt16 RTMFPServer::port() {
	return _port;
}

inline void	RTMFPServer::requestHandle() {
	wakeUp();
}

inline bool RTMFPServer::running() {
	return Startable::running();
}

inline void RTMFPServer::stop() {
	Startable::stop();
}


} // namespace Cumulus

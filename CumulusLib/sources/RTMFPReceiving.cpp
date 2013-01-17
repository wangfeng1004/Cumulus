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

#include "RTMFPReceiving.h"
#include "RTMFPServer.h"
#include "Logs.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Cumulus {

RTMFPReceiving::RTMFPReceiving(RTMFPServer& server,Poco::Net::DatagramSocket& socket): Task(&server), _server(server),pPacket(NULL),id(0),socket(socket) {
	int size = socket.receiveFrom(_buff,sizeof(_buff),address);
	if(server.shellSocket() == socket) {
		if (size >= 0) _buff[size] = '\0';
		if (size >= 1 && (_buff[size - 1] == '\r' || _buff[size -1] == '\n')) 
			_buff[size-1] = '\0'; 
		return;
	}

	if(_server.isBanned(address.host())) {
		INFO("Data rejected because client %s is banned",address.host().toString().c_str());
		return;
	}
	if(size<RTMFP_MIN_PACKET_SIZE) {
		ERROR("Invalid packet");
		return;
	}
	pPacket = new PacketReader(_buff,size);
	id = RTMFP::Unpack(*pPacket);
}

RTMFPReceiving::~RTMFPReceiving() {
	if(pPacket)
		delete pPacket;
}

void RTMFPReceiving::run() {
	if(!pPacket) {
		ERROR("RTMFPReceiving on session %u with a null packet",id);
		return;
	}
	if(!RTMFP::Decode(decoder,*pPacket)) {
		ERROR("Decrypt error on session %u",id);
		return;
	}
	duplicate();
	waitHandleEx(false);
}

void RTMFPReceiving::handle() {
	_server.receive(this);

	Poco::Timestamp tv1;
	Poco::Timestamp::TimeDiff delta = tv1 - tv0;
	_server.rcvpCnt += 1;
	_server.rcvpTm += delta;
	Poco::Int64 tmp = _server.rcvpCnt;
	if (tmp > 0) {
		tmp = _server.rcvpTm / _server.rcvpCnt;
		if(_server.peakRcvp < tmp)
			_server.peakRcvp = tmp;
	}

	release();
}

const char * RTMFPReceiving::bufdata() {
	return (const char *)(&_buff[0]);
}

} // namespace Cumulus

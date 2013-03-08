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

#include "SocketManager.h"
#include "Logs.h"
#include "Poco/SharedPtr.h"
#include "Poco/RefCountedObject.h"
#include "Poco/NumberFormatter.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Cumulus {


class SocketManagedImpl : public SocketImpl {
public:
	SocketManagedImpl(SocketImpl * pImpl, SocketManaged& socketManaged):
		pTrueImpl(pImpl), SocketImpl( pImpl->sockfd() ), pSocketManaged(&socketManaged) 
	{
		pTrueImpl->duplicate();
	}
	virtual ~SocketManagedImpl(){
		reset(); // to avoid the "close" on destruction!
		pTrueImpl->release();
	}
	SocketManaged*		pSocketManaged;
private:
	SocketImpl *pTrueImpl;
};


class PublicSocket : public Socket {
public:
	PublicSocket(SocketImpl* pImpl):Socket(pImpl) {}
};


class SocketManaged : public RefCountedObject {
public:
	SocketManaged(Socket& sock,SocketHandler& hdl): socketSelectable(new SocketManagedImpl(sock.impl(),*this)),handler(hdl), socket(sock) {}
	SocketHandler&				handler;
	PublicSocket				socketSelectable;
	Socket &  socket;
};



SocketManager::SocketManager(TaskHandler& handler,const string& name) : Startable(name),Task(&handler) {

}


SocketManager::~SocketManager() {
	clear();
}

void SocketManager::clear() {
	{
		ScopedLock<Mutex> lock(_mutex);
		map<const Socket*,SocketManaged*>::iterator it;
		for(it=_sockets.begin();it!= _sockets.end();++it) {
			SocketManaged * managed = it->second;
			((SocketManagedImpl*)(managed->socketSelectable.impl()))->pSocketManaged = NULL;
			managed->release();
		}
		_sockets.clear();
	}
	stop();
}

void SocketManager::launch() {
	start();
}

TaskHandler * SocketManager::getTaskHandler() {
	return getTaskHandler();
}

void SocketManager::add(Socket& socket,SocketHandler& handler) {
	ScopedLock<Mutex> lock(_mutex);
	map<const Socket*,SocketManaged*>::iterator it = _sockets.lower_bound(&socket);
	if(it!=_sockets.end() && it->first==&socket)
		return;
	if(it!=_sockets.begin())
		--it;
	_sockets.insert(it,pair<const Socket*,SocketManaged*>(&socket,new SocketManaged(socket,handler)));
}

void SocketManager::remove(const Socket& socket) {
	ScopedLock<Mutex> lock(_mutex);
	map<const Socket*,SocketManaged*>::iterator it = _sockets.find(&socket);
	if(it == _sockets.end())
		return;
	SocketManaged * managed = it->second;  
	((SocketManagedImpl *)(managed->socketSelectable.impl()))->pSocketManaged = NULL;
	managed->release();
	_sockets.erase(it);
}

void SocketManager::handle() {
	ScopedLock<Mutex> lock(_mutex);
	Socket::SocketList::iterator it;
	SocketManaged*		pSocketManaged;
	for (it = _readables.begin(); it != _readables.end(); ++it) {
		//ScopedLock<Mutex> lock(_mutex);
		pSocketManaged = ((SocketManagedImpl*)it->impl())->pSocketManaged;
		if(pSocketManaged) {
			try {
				pSocketManaged->handler.onReadable(pSocketManaged->socket);
			} catch(Exception& ex) {
				pSocketManaged->handler.onError(pSocketManaged->socket, ex.displayText().c_str());
			}
			pSocketManaged->release();
		}
	}
	for (it = _writables.begin(); it != _writables.end(); ++it) {
		//ScopedLock<Mutex> lock(_mutex);
		pSocketManaged = ((SocketManagedImpl*)it->impl())->pSocketManaged;
		if(pSocketManaged) {
			try {
				pSocketManaged->handler.onWritable(pSocketManaged->socket);
			} catch(Exception& ex) {
				pSocketManaged->handler.onError(pSocketManaged->socket,ex.displayText().c_str());
			}
			pSocketManaged->release();
		}
	}
	for (it = _errors.begin(); it != _errors.end(); ++it) {
		//ScopedLock<Mutex> lock(_mutex);
		pSocketManaged = ((SocketManagedImpl*)it->impl())->pSocketManaged;
		if(pSocketManaged) {
			try {
				error(pSocketManaged->socket.impl()->socketError());
			} catch(Exception& ex) {
				pSocketManaged->handler.onError(pSocketManaged->socket,ex.displayText().c_str());
			}
			pSocketManaged->release();
		}
	}
}


void SocketManager::run() {

	Timespan			timeout(10000);
	
	while(running()) {

		bool empty=true;
		{
			ScopedLock<Mutex> lock(_mutex);
			_readables.resize(_sockets.size());
			_errors.resize(_sockets.size());
			_writables.clear();

			int i=0;
			map<const Socket*,SocketManaged*>::iterator it;
			for(it = _sockets.begin(); it != _sockets.end(); ++it) {
				empty=false;
				SocketManaged& socket = *it->second;
				_readables[i] = socket.socketSelectable;
				socket.duplicate();
				if(socket.handler.haveToWrite(socket.socket)) {
					_writables.push_back(socket.socketSelectable);
					socket.duplicate();
				}
				_errors[i] = socket.socketSelectable;
				socket.duplicate();
				++i;
			}
		}
		if(empty) {
			sleep(timeout.milliseconds());
			continue;
		}

		try {
			if (Socket::select(_readables, _writables, _errors, timeout)==0)
				continue;
		} catch(Exception& ex) {
			WARN("Socket error, %s",ex.displayText().c_str())
		}
		
		try {
			handle(); 
		}
		catch (Exception &ex) {
			WARN("handle error, %s", ex.displayText().c_str());
		}
	}
	
}

void SocketManager::status_string(std::string & s) {
	size_t n = 0;
	{
		ScopedLock<Mutex> lock(_mutex);
		n = _sockets.size();
	}
	s = "-------SocketManager-------\n"; 
	s += "\tmapsz: " + Poco::NumberFormatter::format((int)n);
	s += " run : " + Poco::NumberFormatter::format((int)running()); 
	s += "\n";
}

} // namespace Cumulus

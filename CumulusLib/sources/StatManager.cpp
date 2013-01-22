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

#include "StatManager.h"
#include "Logs.h"
#include "Poco/NumberFormatter.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Timespan.h"
#include "Poco/Net/DatagramSocket.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Cumulus {

string StatData::format_string(){
    string s = "";
    s += "\tRecvPackets:       " + NumberFormatter::format(_recvPackets, 8); 
    s += "\tSendPackets:       " + NumberFormatter::format(_sendPackets, 8); 
    s += "\tHandShake:         " + NumberFormatter::format(_handShake, 8); 
    s += "\tKeepAlive:         " + NumberFormatter::format(_keepAlive, 8); 
    s += "\n";
    s += "\tRecvCost:          " + NumberFormatter::format(_recvAccDuration / (_recvPackets + 1), 8); 
    s += "\tRecvPeakCost:      " + NumberFormatter::format(_recvPeakCost, 8); 
    s += "\tSendCost:          " + NumberFormatter::format(_sendAccDuration / (_sendPackets + 1), 8); 
    s += "\tSendPeakCost:      " + NumberFormatter::format(_sendPeakCost, 8); 
    s += "\n";
    s += "\tUnknownSession:    " + NumberFormatter::format(_unknownSession, 8); 
    s += "\tDecryptError:      " + NumberFormatter::format(_decryptError, 8); 
    s += "\n";
    s += "\tTimeoutKeepalive:  " + NumberFormatter::format(_timeoutKeepalive, 8); 
    s += "\tTimeoutConnection: " + NumberFormatter::format(_timeoutConnection, 8); 
    s += "\tFailOnClient:      " + NumberFormatter::format(_failOnClient, 8); 
    s += "\n";
    return s;
}

StatManager::StatManager(const string& name) : Startable(name) {
    stat_data.init();
    last_stat_data.init();
    acc_stat_data.init();
    _startDatetimeStr = DateTimeFormatter::format(LocalDateTime(), "%b %d %Y %H:%M:%S");
}

StatManager::~StatManager() {
	stop();
}

void StatManager::launch() {
	start();
}

void StatManager::run() {
	Timespan timeout(300000000);
	
	do{
        clear();
	}while(sleep(timeout.totalMilliseconds())!=STOP);
}

void StatManager::clear(){
    last_stat_data = stat_data;
    acc_stat_data += stat_data;
    stat_data.clear();
}

void StatManager::onError(const Poco::Net::Socket& socket,const std::string& error) {
    WARN("StatManager, %s",error.c_str());
}

void StatManager::checkCmd(Socket &socket){
    string tmp;
    string resp = "Cumulus server, build time: " __DATE__ " " __TIME__ ", start time: ";
    resp += _startDatetimeStr;
    resp += ", cur time: " + DateTimeFormatter::format(LocalDateTime(),"%b %d %Y %H:%M:%S");
    resp += "\n";

    if (strcmp(_buf, "status") == 0) {
        status_string(tmp);
        resp += tmp;
        //resp += "sessions_n: " + Poco::NumberFormatter::format(_sessions.count()) 
        //    + " sessions_n_peak: " + Poco::NumberFormatter::format(_sessions.peakCount) 
        //    + " task_handle_qsize: " + Poco::NumberFormatter::format(qsize())
        //    + "\n";
        //poolThreads.status_string(tmp);
        //resp += tmp; 
        //sockets.status_string(tmp);
        //resp += tmp;
//void RTMFPServer::status_string(std::string & s) {

    }
    else if (strcmp(_buf, "help") == 0) {
        resp += "Commands: help status\n";
    }
    else {
        resp += "Please type in `help` for details.\n";
    }

    ((DatagramSocket&)socket).sendTo(resp.c_str(), resp.length(), _address, 0); 
}

void StatManager::onReadable(Socket& socket) {
    int size = ((DatagramSocket&)socket).receiveFrom(_buf,sizeof(_buf),_address);
    if (size >= 0) _buf[size] = '\0';
    if (size >= 1 && (_buf[size - 1] == '\r' || _buf[size -1] == '\n')) 
        _buf[size-1] = '\0'; 

    checkCmd(socket);
    return;
}

void StatManager::status_string(string& s) {
    s = "";
    if(_server){
        s += "-------RTMFPServer-------\n"; 
        s += "\tpeak_qsize: " + Poco::NumberFormatter::format(_server->peak_qsize());
        s += " run: " + Poco::NumberFormatter::format(_server->running())
             + "\n"; 
        s += "\tsessions_n: " + Poco::NumberFormatter::format(_server->_sessions.count()) 
             + " sessions_n_peak: " + Poco::NumberFormatter::format(_server->_sessions.peakCount) 
             + "\n";

        s += "-------PoolThreads-------\n";
        for(size_t i = 0; i < _server->poolThreads._threads.size(); ++i) {
            s += "\tthr[" + Poco::NumberFormatter::format(i) 
                + "] qsize: " +  Poco::NumberFormatter::format(_server->poolThreads._threads[i]->queue()) 
                + " run: " + Poco::NumberFormatter::format(_server->poolThreads._threads[i]->running()) 
                + "\n";
        }

        s += "-------SocketManager-------\n"; 
        s += "\tmapsz: " + Poco::NumberFormatter::format(_server->sockets._sockets.size());
        s += " run : " + Poco::NumberFormatter::format(_server->sockets.running()); 
        s += "\n";
    }

    s += "-------StatManager-------\n";
    s += "-------current status:\n";
    s += stat_data.format_string();
    s += "-------last five minutes status:\n";
    s += last_stat_data.format_string();
    s += "-------accumulative status:\n";
    s += acc_stat_data.format_string();
    s += "\n";
}

void StatManager::setRTMFPServer(RTMFPServer *server) {
    _server = server;
};

StatManager StatManager::global;

} // namespace Cumulus

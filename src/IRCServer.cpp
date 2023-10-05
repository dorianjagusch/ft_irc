/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttikanoj <ttikanoj@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 23:21:45 by tuukka            #+#    #+#             */
/*   Updated: 2023/10/05 17:16:52 by ttikanoj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/IRCServer.hpp"
#include <fstream>

void bitsToStream(short num, std::ostream& os) {
    int numBits = sizeof(num) * 8; // Number of bits in a short

    for (int i = numBits - 1; i >= 0; i--) {
        // Use bitwise AND to check the i-th bit
        int bit = (num >> i) & 1;
        os << bit;
    }
}

IRCServer::IRCServer(uint16_t port) : p_port(port){
	// std::cout << "IRCServer constructor called" << std::endl;
	p_pfds.reserve(MAXCLIENTS);
	p_users.reserve(MAXCLIENTS);
	initServer();
	return ;
}

IRCServer::~IRCServer(void) {
	std::cout << "IRCServer destructor called" << std::endl;
	return ;
}

void IRCServer::initServer() {
	if (getListenerSocket())
		throw std::runtime_error("Failed to create listener socket");
	// if (pollingRoutine())
	// 	throw std::runtime_error("Polling failed");
	return ;
}

void IRCServer::initCommands() {
	static const std::string cmdNames[] = {
		"PASS",
		"NICK"
	};

	static const CommandFunction cmdFunctions[] = {
		cmd_pass,
		cmd_nick
	};

	for (size_t i = 0; i < N_COMMANDS; i++)
		p_commandMap[cmdNames[i]] = cmdFunctions[i];
}

std::string const & IRCServer::getName(){
	return p_serverName;
}

std::string const & IRCServer::getPassword() const{
	return (p_password);
}

Uvector		const &	IRCServer::getUsers() const{
	return (p_users);
}

std::vector<std::string> const &		IRCServer::getBlocked() const{
	return p_blockeUserNames;
}

void IRCServer::setBlocked(std::string nick){
	p_blockeUserNames.push_back(nick);
}

bool	IRCServer::isBlocked(std::string nick) const{

	std::vector<std::string>::const_iterator it;

	for (it = p_blockeUserNames.begin(); it != p_blockeUserNames.end(); it++){
		if (*it == nick)
			return true;
	}
	return false;
}

int IRCServer::receiveMsg(User* user, nfds_t i) {
	char buf[MAXDATASIZE];
	memset(buf, '\0', MAXDATASIZE);
	ssize_t numbytes;
	numbytes = recv(p_pfds[i].fd, buf, MAXDATASIZE - 1, 0);
	if (numbytes <= 0) {
		dropConnection(numbytes, i);
		return (-1);
	}
    std::ofstream outFile;
	outFile.open("log", std::fstream::app);
	outFile << buf;
	outFile << "=============================" << std::endl;
	if (numbytes == 1) {
		std::cout << "Recieved empty message. (Just a newline from nc?)" << std::endl;
		return (0);
	}
	user->getRecvBuffer().addToBuffer(buf);
	return (0);
}

int IRCServer::checkBuffer(User* user, nfds_t i) {
	if (user->getRecvBuffer().findCRLF() == -1) { //do we have a complete msg ???
		// std::cout << "did not detect CRLF" << std::endl;
		return (0);
	}
	std::string msg = user->getRecvBuffer().extractBuffer();
	// std::cout << "Server: received message: " << msg << std::endl;
	Message m(msg);
	m.printContent();
	replyToMsg(p_users.findUserBySocket(p_pfds[i].fd), &m);
	return 0;
}

void IRCServer::replyToMsg(User* user, Message *msg) { //ottaa vastaan message classin ja target user ?
	if (user->getSendBuffer().emptyCheck() == 0) {
		const char* msgc = msg->toString();
		user->getSendBuffer().addToBuffer(msgc);
		delete msgc;
	} if (user->getSendBuffer().emptyCheck() == 1) {
		std::string toSend = user->getSendBuffer().extractBuffer();
		const char* toSendC = toSend.c_str();
		ssize_t toSendLen = static_cast<ssize_t>(toSend.length());
		ssize_t n_sent = 0;
		if ( (n_sent = send(user->getSocket(),  &(toSendC[0]), static_cast<size_t>(toSendLen), 0) ) <= 0)
			std::cerr << "Send failed" << std::endl;
		if (n_sent > 0 && n_sent < toSendLen) {
			toSend.erase(0, static_cast<size_t>(n_sent));
			const char* toBuffer = toSend.c_str();
			user->getSendBuffer().addToBuffer(toBuffer, toSendLen - n_sent);
		}
	}
}

//check for POLLNVAL & POLLERR
int IRCServer::pollingRoutine() {
	int poll_count;
	int j = 0;
	nfds_t fd_count = static_cast<nfds_t>(p_pfds.size());
	while (1) {
		if ((poll_count = poll(&(p_pfds[0]), fd_count, 200)) == -1) //!!!!!!!!!!!! TIMEOUT
			return (-1);
		for (nfds_t i = 0; i < fd_count; i++) {
			if (p_pfds[i].revents & (POLLIN | POLLOUT | POLLNVAL | POLLERR)) {
				if (i == 0) { //Listener has a client in accept queue
					if (acceptClient())
						continue ;
					fd_count++;
				} else if (p_pfds[i].revents & POLLIN) { //A client has sent us a message, add to buffer!
					std::cout <<"Received msgs:" << j++ << std::endl;
 					if (receiveMsg(p_users.findUserBySocket(p_pfds[i].fd), i)) {
						fd_count--;
						continue ;
					}
				} else if (p_pfds[i].revents & POLLOUT) { //A client has sent us a message, add to buffer!
					std::cout << "Ready to receive!" << std::endl;
				}  else { //if (p_pfds[i].revents & (POLLNVAL | POLLERR)) { //A client has sent us a message, add to buffer!
					std::cout << "Polling invalid / error" << std::endl;
					dropConnection(-1, i);
				}
				std::cout << "our bits: ";
				bitsToStream(p_pfds[i].revents, std::cout);
				std::cout << std::endl;
			}
			if (i != 0) //check the buffer for this user, if there is something in there, extract until CRLF and process
				checkBuffer(p_users.findUserBySocket(p_pfds[i].fd), i);
			//catch a signal maybe?? with sigaction ????
		}
	}
	for (nfds_t i = 0; i < fd_count; i++) {
		close(p_pfds[i].fd);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djagusch <djagusch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 23:21:45 by tuukka            #+#    #+#             */
/*   Updated: 2023/11/03 07:35:17 by djagusch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/IRCServer.hpp"
#include "../../inc/Commands.hpp"
#include "../../inc/Utils.hpp"

std::string const & IRCServer::getName() const{

	return p_serverName;
}

std::string const & IRCServer::getPassword() const{

	return p_password;
}

Uvector const &	IRCServer::getUsers() const{
	
	return p_users;
}

Cvector & IRCServer::getChannels(){

	return p_channels;
}

std::vector<Operator> const & IRCServer::getOpers() const{

	return p_opers;
}

std::string const & IRCServer::getDate(void) const{
	return p_creationDate;
}

std::string const & IRCServer::getVersion(void) const{
	return p_version;
}

std::string const & IRCServer::getUmodes(void) const{
	return p_modes;
}

std::string const & IRCServer::getCmodes(void) const{
	return p_chanModes;
}


void signalHandler(int signum) {
	
	if (signum == SIGINT) {
		std::cout << std::endl << "Server quit." << std::endl;
		exit(0);
	}
}

IRCServer::IRCServer(uint16_t port, std::string password) : p_port(port), p_password(password){

	p_pfds.reserve(MAXCLIENTS + 1);
	p_users.reserve(MAXCLIENTS + 1);
	p_logger = new Logger("./config/log");
	p_logger->log("IRCserv started", __FILE__, __LINE__);
	try{
		initServer();
	} catch (std::exception &e){
		exit(1);
	}
	return ;
}

IRCServer::~IRCServer(void) {
	
	p_logger->log("IRCserv shutdown", __FILE__, __LINE__);
	delete p_logger;
	return ;
}

void IRCServer::initServer() {
	
	if (getListenerSocket()){
		p_logger->log("Could not create listener socket", __FILE__, __LINE__);
		throw std::runtime_error("Failed to create listener socket");
	}
	setGlobals() ;
	initCommands();
	initOperators();
	std::cout << "Running..."<< std::endl;
	return ;
}

void IRCServer::setGlobals() {
	std::time_t p_cur_time = std::time(0);
	std::tm* now = std::localtime(&p_cur_time);
	// Mon Sep 11 2023 at 00:21:58 UTC

	p_serverName = "PawsitiveIRC";
	p_creationDate = std::asctime(now); 
	p_version = "0.0.1";
	p_modes = "awiroOs";
	p_chanModes = "itkol";
}

void IRCServer::initCommands() {

	std::pair<std::string, commandFunction> cmdPairs[] = {
		std::make_pair("CAP", cmd_cap),
		std::make_pair("PASS", cmd_pass),
		std::make_pair("NICK", cmd_nick),
		std::make_pair("USER", cmd_user),
		std::make_pair("MODE", cmd_mode),
		std::make_pair("MOTD", cmd_motd),
		std::make_pair("OPER", cmd_oper),
		std::make_pair("AWAY", cmd_away),
		std::make_pair("QUIT", cmd_quit),
		std::make_pair("PING", cmd_ping),
		std::make_pair("PONG", cmd_pong),
		std::make_pair("PRIVMSG", cmd_privmsg),
		std::make_pair("NOTICE", cmd_notice),
		std::make_pair("KILL", cmd_kill),
		std::make_pair("JOIN", chan_cmd_join),
		std::make_pair("PART", chan_cmd_part),
		std::make_pair("TOPIC", chan_cmd_topic),
		std::make_pair("INVITE", chan_cmd_invite),
		std::make_pair("KICK", chan_cmd_kick)
	};
	p_commandMap.insert(cmdPairs, cmdPairs + sizeof(cmdPairs) / sizeof(cmdPairs[0]));
}

void IRCServer::initOperators(){

	std::ifstream operFile;

	operFile.open("config/operators.config", std::fstream::in);
	if (!operFile.good() || !operFile.is_open() || operFile.peek() < 0){
		std::cout << "Cannot set any operators" << std::endl;
		return ;
	}
	char line[256];
	std::vector<std::string> rawOper;
	while (operFile.getline(line, 256)){
		std::string string = line;
		rawOper = split(string, ' ');
		p_opers.push_back(Operator(rawOper[0], rawOper[1], rawOper[2]));
	}
}

void IRCServer::log(std::string string, std::string file, int line){
	
	p_logger->log(string, file, line);
}

int IRCServer::pollingRoutine() {
	
	int poll_count;
	p_fd_count = static_cast<nfds_t>(p_pfds.size());
	signal(SIGINT, signalHandler);
	while (1) {
		if ((poll_count = poll(&(p_pfds[0]), p_fd_count, 0)) == -1)
			return (-1);
		for (nfds_t i = 0; i < p_fd_count; i++) {
			if (p_pfds[i].revents & (POLLIN | POLLOUT | POLLNVAL | POLLERR)) {
				if (i == 0) {
					if (acceptClient()){
						p_logger->log("Failed to accept new client", __FILE__, __LINE__);
					}
				} else if (p_pfds[i].revents & POLLIN) {
					receiveMsg(p_users.findUserBySocket(p_pfds[i].fd), i);
				} else if (p_pfds[i].revents & POLLOUT) {
					checkSendBuffer(p_users.findUserBySocket(p_pfds[i].fd));
					checkRecvBuffer(p_users.findUserBySocket(p_pfds[i].fd), i);
				}
				else {
					dropConnection(-1, i);
					p_logger->log("Connection dropped", __FILE__, __LINE__);
				}
			}
		}
	}
	for (nfds_t i = 0; i < p_fd_count; i++) {
		close(p_pfds[i].fd);
	}
	return (0);
}

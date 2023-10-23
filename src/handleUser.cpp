/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleUser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djagusch <djagusch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 10:35:41 by djagusch          #+#    #+#             */
/*   Updated: 2023/10/23 17:10:20 by djagusch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/IRCServer.hpp"

// char const * IRCServer::UserNotFound::what() const throw(){
// 	return "Exception: User not found";
// }

bool IRCServer::getUserMode(User & user, e_uperm mode) const{
	try {
		if (!user.getNick().empty())
			return p_users.findUserByNick(user.getNick())->getMode() & mode;
		return p_users.findUserBySocket(user.getSocket())->getMode() & mode;
	} catch (std::exception & e){
		return false;
	}
}

void IRCServer::setUserMode(User & user, e_uperm mode){
	try {
		if (user.getNick() == "")
				p_users.findUserByNick(user.getNick())->setMode(mode);
			p_users.findUserBySocket(user.getSocket())->setMode(mode);
	} catch (std::exception & e){
		/* send error reply, no such user (techically we'd check first the local p_users, then all user list (combined from all servers)
		and only then give the error)*/
	}
}

void	IRCServer::unsetUserMode(User & user, e_uperm mode){
	try {
		if (user.getNick() == "")
				p_users.findUserByNick(user.getNick())->unsetMode(mode);
			p_users.findUserBySocket(user.getSocket())->unsetMode(mode);
	} catch (std::exception & e){
		/* send error reply, no such user (techically we'd check first the local p_users, then all user list (combined from all servers)
		and only then give the error)*/
	}
}

std::string	IRCServer::getModeStr(User const & user){
	static const e_uperm permissions[] = {away, wallops, invisible, restricted, oper, Oper, server_notice};
	static const std::string characters = "awiroOs";
	std::string result = "+";
	int mode = user.getMode();

	for(size_t i = 0; i < characters.size(); i++) {
		if (mode & permissions[i])
			result += characters[i];
	}
	return result;
}

std::string	IRCServer::setBatchMode(User & user, std::string const & modes, size_t *index){

	std::string opsdone = "";
	static const std::string characters = "iwrs";

	if (modes[*index] == '+')
	{
		for (; *index < modes.size(); (*index)++ ){
			switch (modes[*index]){
				case ('i'):
					if (user.setMode(invisible))
						opsdone += characters[0];
					break;
				case ('w'):
					if (user.setMode(wallops))
						opsdone += characters[1];
					break;
				case ('r'):
					if (user.setMode(restricted))
						opsdone += characters[2];
					break;
				case ('s'):
					if (user.setMode(server_notice))
						opsdone += characters[3];
					break;
				case ('-'):
					return opsdone;
				default:
					continue;
			}
		}
	}
	return opsdone;
}

std::string		IRCServer::unsetBatchMode(User & user, std::string const & modes, size_t *index){

	std::string opsdone = "";
	static const std::string characters = "iwoOs";

	if (modes[*index] == '-')
	{
		for ( ; *index < modes.size(); (*index)++ ){
			switch (modes[*index]){
				case ('i'):
					if (user.unsetMode(invisible))
						opsdone += characters[0];
					break;
				case ('w'):
					if (user.unsetMode(wallops))
						opsdone += characters[1];
					break;
				case ('o'):
					if (user.unsetMode(oper))
						opsdone += characters[2];
					break;
				case ('O'):
					if (user.unsetMode(Oper))
						opsdone += characters[3];
					break;
				case ('s'):
					if (user.unsetMode(server_notice))
						opsdone += characters[3];
					break;
				case ('+'):
					return opsdone;
				default:
					continue;
			}
		}
	}
	return opsdone;
}

Operator & IRCServer::getOperByNick(std::string nick){
	
	size_t i = 0;
	for ( ; i < p_opers.size(); i++){
		if (nick == p_opers[i].getNick())
			break;
	}
	// if (i == p_opers.size())
	// 	throw UserNotFound();
	return p_opers[i];
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttikanoj <ttikanoj@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 16:33:05 by djagusch          #+#    #+#             */
/*   Updated: 2023/11/02 13:09:14 by ttikanoj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/Commands.hpp"

#define NICKAPPENDICES "_123456789"

static bool isNickvalid(std::string& nick);

int cmd_nick(IRCServer& server, User& user, Message& message){
	if (user.getPassFlag() == false) {
		user.send(ERR_PASSWDMISMATCH(server.getName()));
		return 1;
	}
	std::string new_nick = message.getParams().front();
	if (new_nick.empty()){
		user.send(ERR_NONICKNAMEGIVEN(server.getName()));
		return 1;
	}
	if (!isNickvalid(new_nick)){
		user.send(ERR_ERRONEUSNICKNAME(server.getName(),
			new_nick));
		return 1;
	}
	User * found_user = server.getUsers().findUserByNick(new_nick);
	if (found_user != NULL){
		user.send(ERR_NICKNAMEINUSE(server.getName(), new_nick));
		return 1;
	}
	server.log("Changed nickname " + user.getNick() + "->" + new_nick, __FILE__, __LINE__);
	user.setNick(new_nick);
	user.setRegistrationFlag(1, user, server);
	return 0;
}

static bool isNickvalid(std::string& nick){

	if (nick[0] == '$' || nick[0] == ':' || nick[0] == '#' || nick[0] == '+'){
		return false;
	}
	if (nick.find_first_of(",*?! @.", 0) != std::string::npos){
		return false;
	}
	return true;
}

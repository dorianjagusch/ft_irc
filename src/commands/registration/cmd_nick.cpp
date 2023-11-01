/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djagusch <djagusch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 16:33:05 by djagusch          #+#    #+#             */
/*   Updated: 2023/11/01 13:14:52 by djagusch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/Commands.hpp"

#define NICKAPPENDICES "_123456789"

// std::string changeNick(IRCServer const &server, std::string const & nick){

// 	for (size_t i = 0; i < MAXCLIENTS; i++)
// 	{
// 		std::string new_nick = nick.substr(0, UNAMELEN - 1).append(1, NICKAPPENDICES[i]);
// 		if (!server.getUsers().findUserByNick(new_nick))
// 			return new_nick;
// 	}
// 	return "";
// }

bool	isNickvalid(std::string& nick){

	if (nick[0] == '$' || nick[0] == ':' || nick[0] == '#' || nick[0] == '+'){
		return false;
	}
	if (nick.find_first_of(",*?! @.", 0) != std::string::npos){
		return false;
	}
	return true;
}

int cmd_nick(IRCServer& server, User& user, Message& message){
	if (user.getPassFlag() == false) {
		user.send(ERR_PASSWDMISMATCH(server.getName()));
		return 1;
	}
	std::string new_nick = message.getParams()[0];
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
	if (server.isBlocked(new_nick)){
		user.send(ERR_UNAVAILRESOURCE(server.getName(),
			new_nick, "nick"));
		return 1;
	}
	user.setNick(new_nick);
	user.setRegistrationFlag(1, user, server);
	return 0;
}

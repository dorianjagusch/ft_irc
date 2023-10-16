/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executeCommand.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djagusch <djagusch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 15:52:00 by djagusch          #+#    #+#             */
/*   Updated: 2023/10/16 07:22:12 by djagusch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Commands.hpp"

int IRCServer::executeCommand(User& user, Message& message) {

	std::map<std::string, CommandFunction>::iterator it = p_commandMap.find(message.getCommand());
	if (it != p_commandMap.end()) {
		CommandFunction func = it->second;
		return func(*this, user, message);
	} else {
		std::cout << "Command not found...!" << std::endl; //remove
		user.getSendBuffer().addToBuffer(ERR_UNKNOWNCOMMAND(getName(), message.getCommand()).c_str());
		return 1;
	}
}

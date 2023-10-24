/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chan_cmd_join.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttikanoj <ttikanoj@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 09:40:42 by djagusch          #+#    #+#             */
/*   Updated: 2023/10/24 12:07:16 by ttikanoj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/Commands.hpp"
/*
	ERRORS
		ERR_NEEDMOREPARAMS		//if missing params
		ERR_BANNEDFROMCHAN		//trying to join a chan you are banned from
		ERR_INVITEONLYCHAN		//trying to join a chan that is set to invite only without inv
		ERR_BADCHANNELKEY		//wrong key (pw)
		ERR_CHANNELISFULL		//channel is full...
		ERR_BADCHANMASK			//not in use!!!!!?
		ERR_NOSUCHCHANNEL		//given channel name is invalid
		ERR_TOOMANYCHANNELS		//when the user has joined max amount of channels and tries to join another
		ERR_TOOMANYTARGETS		//Returned to a client which is attempting to send a PRIVMSG/NOTICE using the user@host destination format and for a user@host which has several occurrences
		ERR_UNAVAILRESOURCE		// Returned by a server to a user trying to join a channel currently blocked by the channel delay mechanism.

	REPLIES
		RPL_TOPIC				//sent upon successful JOIN
*/
int chan_cmd_join(IRCServer& server, User& user, Message& message){
	if (!(user.getMode() & IRCServer::registered)) {
		user.send(ERR_NOTREGISTERED(server.getName(), message.getCommand()));
		return 1;
	}
	if (message.getParams().front() == "") { //irssi expects this reply for some reason w registering...
		user.send(":127.0.0.1 451 * JOIN :You must finish connecting with another nickname first.\r\n");
		return 1;
	}
	// //ERR_NEEDMOREPARAMS
	// if (message.getParams().front() == "") {
	// 	user.send(ERR_NEEDMOREPARAMS(server.getName(), "JOIN"));
	// 	return 1;
	// }

	
	//if user has already joined max limit of channels: ERR_TOOMANYCHANNELS
	std::stringstream ss(message.getParams().front());
	std::string chan;
	while (std::getline(ss, chan, ',')) {
		Channel* toJoin = server.getChannels().findChannel(chan);
		//if yes join
		if (toJoin != NULL) { //JOINING AN EXISTING CHANNEL
			//if chan full: ERR_CHANNELISFULL
			//if key needed and key not provided? ERR_NEEDMOREPARAMS or ERR_BADCHANNELKEY ?
			//if key needed and bad key: ERR_BADCHANNELKEY
			//if invite only
				//check channel invitelist for user
					//if no match tell them off
					//if yes match erase name from the list
			toJoin->getMembers()->push_back(&user);
			toJoin->broadcastToChannel(":" + user.getNick() + "!add_user_host_here " + "JOIN :" + toJoin->getName() + "\r\n");
			if (toJoin->getTopic() != "")
				user.send(RPL_TOPIC(server.getName(), user.getNick(), toJoin->getName(), toJoin->getTopic()));
			else
				user.send(RPL_NOTOPIC(server.getName(), toJoin->getName()));
			//send 353 (???)
		} else { //CREATING NEW CHANNEL
			//check if the channel name is valid! If not, send err_nosuchchannel(???)
			//check that the channel name is unique
			toJoin = server.getChannels().createChannel(chan);
			toJoin->getMembers()->push_back(&user);
			//add chop to user?
			toJoin->broadcastToChannel(":" + user.getNick() + "!add_user_host_here " + "JOIN :" + toJoin->getName() + "\r\n");
		}
	}
	return 0;
}

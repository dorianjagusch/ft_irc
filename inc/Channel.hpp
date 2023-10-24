/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttikanoj <ttikanoj@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 23:24:23 by tuukka            #+#    #+#             */
/*   Updated: 2023/10/24 12:05:57 by ttikanoj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include "Uvector.hpp"

class User;

class Channel
{
	public:
		Channel(const std::string name);
		Channel(Channel const& src);
		~Channel();
		Channel &		operator=(Channel const& rhs);
		std::string 	getName();
		std::string 	getTopic();
		Uvector*		getMembers();
		Uvector*		getInvitelist();
		void			setTopic(std::string newTopic);
		void			broadcastToChannel(std::string message);

	private:
		Channel();
		std::string	p_name;
		std::string p_topic;
		Uvector		p_members;
		Uvector		p_invitelist;
		//wrapper for the members to have permissions etc :)
		Uvector		p_chopers;
		//channel mode
};

#endif

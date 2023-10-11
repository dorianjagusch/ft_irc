/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttikanoj <ttikanoj@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 23:29:10 by tuukka            #+#    #+#             */
/*   Updated: 2023/10/11 11:33:52 by ttikanoj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string name) : p_name(name)
{
	std::cout << "Channel constructor called" << std::endl;
	return ;
}

Channel::Channel(Channel const& src)
{
	*this = src;
}

Channel& Channel::operator=(Channel const& rhs)
{
	if(this != &rhs) {
		p_name = rhs.p_name;
	}
	return *this;
}

Channel::~Channel(void)
{
	std::cout << "Channel destructor called" << std::endl;
	return ;
}

std::string Channel::getName()
{
	return this->p_name;
}

Uvector* Channel::getMembers()
{
	return &(this->p_members);
}

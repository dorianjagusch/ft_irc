/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uvector.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djagusch <djagusch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/30 11:16:04 by djagusch          #+#    #+#             */
/*   Updated: 2023/10/05 09:58:02 by djagusch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Uvector.hpp"

char const * Uvector::UserNotFound::what() const throw(){
		return "User not found";
}

Uvector::Uvector()
{}

Uvector::Uvector(Uvector const& src)
{
	*this = src;
}

Uvector::~Uvector()
{}

Uvector& Uvector::operator=(Uvector const & rhs)
{
	if (this != &rhs)
		copy(rhs.begin(), rhs.end(), this->begin());
	return *this;
}


std::string Uvector::toIRCLower(std::string const & str) const {
	
	std::string result;
	
	for (size_t i = 0; i < str.size(); i++)
		result += (str[i] >= 'A' && str[i] <= '^' ? str[i] + 32 : str[i]);
	return (result);
}

User*		Uvector::findUserBySocket(int const socket_fd) const
{
	for (std::vector<User*>::const_iterator it = this->begin(); it != this->end(); it++){
		if ((*it)->getSocket() == socket_fd)
			return (*it);
	}
	return NULL;
}

User*		Uvector::findUserByNick(std::string const & nick) const
{
	for (std::vector<User*>::const_iterator it = this->begin(); it != this->end(); it++){
		if (toIRCLower((*it)->getNick()) == toIRCLower(nick))
			return (*it);
	}
	throw(UserNotFound());
}

User*		Uvector::findUserByIP(std::string const & ip_address) const
{
	for (std::vector<User*>::const_iterator it = this->begin(); it != this->end(); it++){
		if ((*it)->getIP() == ip_address)
			return (*it);
	}
	return (NULL);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Operator.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djagusch <djagusch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/19 10:21:34 by djagusch          #+#    #+#             */
/*   Updated: 2023/10/23 16:31:03 by djagusch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPERATOR_HPP
# define OPERATOR_HPP

# include <string>
# include <exception>
# include <vector>
# include "User.hpp"
# include <netinet/in.h>
# include <arpa/inet.h> 

class Operator
{
	public:

		Operator();
		Operator(std::string const nick, std::string const ip, std::string const password);
		Operator(Operator const& src);
		~Operator();

		std::string const & getNick() const;
		std::string const & getIP() const;
		std::string const & getPW() const;
		void				setUser(User *user);

		typedef std::string const & (Operator::*OperFunct)() const;

	private:
		std::string const p_nick;
		std::string const p_ip;
		std::string const p_password;
		User*			p_user;
};

#endif

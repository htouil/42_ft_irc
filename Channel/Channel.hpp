/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:46:37 by htouil            #+#    #+#             */
/*   Updated: 2024/12/23 00:12:14 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include "../Client/Client.hpp"

class Channel
{
	private:
		std::string				Name;
		std::string				Key;
		// std::string				Mod;
		std::string				Topic;
		size_t					Limit;
		bool					Inv_only;
		bool					Can_topic;
		std::vector<Client>		Members;
		std::vector<Client>		Banned;
	
	public:
							Channel(std::string Name, std::string Key);
		bool				operator==(std::string name);
		std::string			GetName();
		void				SetName(std::string newName);
		std::string			GetKey();
		void				SetKey(std::string newKey);
		std::string			GetTopic();
		void				SetTopic(std::string newTopic);
		size_t				GetLimit();
		void				SetLimit(size_t newLimit);
		bool				GetifInvonly();
		void				Setifinvonly(bool ifinv);
		bool				GetCantopic();
		void				SetCantopic(bool iftopic);
		std::vector<Client>	&GetMemberlist();
		std::vector<Client>	GetBannedlist();
};

#endif

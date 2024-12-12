/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:46:37 by htouil            #+#    #+#             */
/*   Updated: 2024/12/11 21:57:50 by htouil           ###   ########.fr       */
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
		bool					inv_only;
		std::vector<Client>		Members;
		std::vector<Client>		Banned;
	
	public:
							Channel(std::string Name, std::string Key);
		std::string			GetName();
		void				SetName(std::string newName);
		std::string			GetKey();
		void				SetKey(std::string newKey);
		std::string			GetTopic();
		void				SetTopic(std::string newTopic);
		size_t				GetLimit();
		void				SetLimit(size_t newLimit);
		bool				Getifinvonly();
		std::vector<Client>	GetMemberlist();
		std::vector<Client>	GetBannedlist();
};

#endif

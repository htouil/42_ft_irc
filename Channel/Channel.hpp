/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amirabendhia <amirabendhia@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:46:37 by htouil            #+#    #+#             */
/*   Updated: 2024/12/31 03:01:09 by amirabendhi      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include <utility>
# include <algorithm>
# include "../Client/Client.hpp"

class Channel
{
	private:
		std::string										Name;
		std::string										Key;
		std::string										Topic;
		size_t											Limit;
		bool											Inv_only;
		bool											Can_topic;
		std::vector<std::pair<Client, std::string> >	Members;
		std::vector<std::string>						ChanMods;
		std::vector<Client>                           invited_users;

		// C++98 compatible functor for finding invited users
		struct InvitedUserFinder {
			const std::string& nickname;
			InvitedUserFinder(const std::string& nick) : nickname(nick) {}
			bool operator()(const Client& client) const {
				return client.GetNickname() == nickname;
			}
		};

	public:
							Channel(std::string Name, std::string Key);
			std::string		GetName();
			void			SetName(std::string newName);
			std::string		GetKey();
			void			SetKey(std::string newKey);
			std::string		GetTopic();
			void			SetTopic(std::string newTopic);
		size_t				GetLimit();
		void				SetLimit(size_t newLimit);
		bool				GetifInvonly();
		void				Setifinvonly(bool ifinv);
		bool				GetCantopic();
		void				SetCantopic(bool iftopic);
		std::vector<std::pair<Client, std::string> >	&GetMemberlist();

		void AddInvited(const Client& client) {
			invited_users.push_back(client);
		}

		bool IsInvited(const Client& client) const {
			return std::find_if(invited_users.begin(), 
							   invited_users.end(),
							   InvitedUserFinder(client.GetNickname())) 
				   != invited_users.end();
		}
};

#endif

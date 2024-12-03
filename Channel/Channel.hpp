/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:46:37 by htouil            #+#    #+#             */
/*   Updated: 2024/12/03 23:00:14 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>

class Channel
{
	private:
		std::string				Name;
		std::string				Key;
		std::string				Topic;
		std::vector<int>		Members;
	
	public:
							Channel(std::string Name, std::string Key);
		std::string			GetName();
		void				SetName(std::string newName);
		std::string			GetTopic();
		void				SetTopic(std::string newTopic);
		std::vector<int>	GetMemberlist();
};

#endif

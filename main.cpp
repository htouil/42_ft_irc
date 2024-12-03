/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 22:22:39 by htouil            #+#    #+#             */
/*   Updated: 2024/12/03 01:17:34 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server/Server.hpp"

int	Parse_Arguments(char **av)
{
	int		n;
	char	*end;

	n = static_cast<int>(strtol(av[1], &end, 10));
	if (*end != '\0')
	{
		std::cerr << "The input port is not an integer." << std::endl;
		return (1);
	}
	if (n < 1024 || n > 49151)
	{
		std::cerr << "The input port should be between 1024 ans 49151." << std::endl;
		return (1);
	}
	return (0);
}

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Wrong set of arguments." << std::endl << "Try './ircserv <port> <Spassword>'" << std::endl;
		return (1);
	}
	if (Parse_Arguments(av) == 1)
		return (1);

	Server	server(atoi(av[1]), av[2]);
	try
	{
		signal(SIGINT, Server::Signal_Handler);
		signal(SIGQUIT, Server::Signal_Handler);
		server.Server_Initialization(av);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		server.Disconnect_Everything();
	}
	return (0);
}

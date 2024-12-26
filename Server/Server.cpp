/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htouil <htouil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 23:45:57 by htouil            #+#    #+#             */
/*   Updated: 2024/12/26 22:46:49 by htouil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	Server::Signal = false;

Server::Server(int port, std::string Spassword) : Name("Discord Mdere7"), Port(port), Spassword(Spassword), SockFd(-1)
{
}

void	Server::SetPort(int newPort)
{
	this->Port = newPort;
}

void	Server::Signal_Handler(int signum)
{
	(void)signum;
	Server::Signal = true;
}

void	Server::Disconnect_Everything()
{
	size_t	i;

	for (i = 0; i < this->Clients.size(); i++)
	{
		std::cout << display_current_time() << " Client " << this->Clients[i].GetFd() << " (" << this->Clients[i].GetNickname() << ") Disconnected." << std::endl;
		close(this->Clients[i].GetFd());
	}
	if (this->SockFd != -1)
	{
		std::cout << display_current_time() + "Server Disconnected." << std::endl;
		close(this->SockFd);
	}
}

void	Server::Remove_Client(int rfd)
{
	size_t	i;

	for (i = 0; i < this->Fds.size(); i++)
	{
		if (this->Fds[i].fd == rfd)
		{
			this->Fds.erase(this->Fds.begin() + i);
			break ;
		}
	}
	for (i = 0; i < this->Clients.size(); i++)
	{
		if (this->Clients[i].GetFd() == rfd)
		{
			close(this->Clients[i].GetFd());
			this->Clients.erase(this->Clients.begin() + i);
			break ;
		}
	}
}

// template<typename T>
// size_t	Server::find_client(T to_find, std::string type)
// {
// 	size_t	i;

// 	for (i = 0; i < this->Clients.size(); i++)
// 	{
// 		if (type == "fd")
// 		{
// 			if (this->Clients[i].GetFd() == (to_find))
// 				return (i);
// 		}
// 		if (type == "nick")
// 		{
// 		if (this->Clients[i].GetNickname() == (to_find))
// 			return (i);
// 		}
// 	}
// 	return (-1);
// }

std::pair<std::string, std::vector<std::string>	>	extract_args(std::string cmd)
{
	std::istringstream									ss(cmd);
	std::string											cmdName;
	std::string											arg;
	std::pair<std::string, std::vector<std::string>	>	args;

	
	ss >> cmdName;
	args.first = cmdName;
	while (ss >> std::ws)
	{
		// std::cout << "hna: \'" << arg << "\'" << std::endl;
		if (ss.peek() == ':')
		{
			// ss.get();
			if (std::getline(ss, arg))
				args.second.push_back(arg);
			break ;
		}
		if (std::getline(ss, arg, ' '))
			args.second.push_back(arg);
	}
	return (args);
}

void	Server::receive_request(int clifd)
{
	char												tmp[6000];
	size_t												bytes;
	std::vector<std::string>							cmds;
	// size_t												i;
	std::pair<std::string, std::vector<std::string>	>	args;
	int													pos;
	Client												client;

	memset(tmp, 0, sizeof(tmp));
	bytes = recv(clifd, tmp, sizeof(tmp) - 1, 0);
	// for (i = 0; i < std::strlen(tmp); i++)
	// 	std::cout << (int)tmp[i] << " , ";
	// std::cout << std::endl << "------" << std::endl;
	// std::cout << "this is buffer: " << buffer << std::endl;
	// for (i = 0; i < buffer.size(); i++)
	// 	std::cout << (int)buffer[i] << " , ";
	// std::cout << std::endl << "------" << std::endl;
	if (bytes == 0)
	{
		std::cerr << display_current_time() << " Client: " << clifd << " (" << this->Clients[this->find_fd(clifd, this->Clients)].GetNickname() << ") Hung up." << std::endl;
		this->Remove_Client(clifd);
		close(clifd);
	}
	else if (bytes > 0)
	{
		std::string	buffer(tmp);
		
		// for (i = 0; i < buffer.size(); i++)
		// std::cout << "buffer: " << (int) buffer[buffer.size() - 2] << std::endl;
		if (buffer.empty())
			return ;
		pos = this->find_fd(clifd, this->Clients);
		this->Clients[pos].SetInBuffer(buffer);
		// std::cout << "Buffer: \'" << this->Clients[pos].GetInBuffer() << "\'" << std::endl;
		// if (buffer[buffer.size() - 2] != '\r' || buffer[buffer.size() - 1] != '\n')
		if ((this->Clients[pos].GetInBuffer()).find_first_of("\r\n") != std::string::npos)
		{
			// if ((this->Clients[pos].GetInBuffer()).find_first_of("\r") != std::string::npos)
			// {
				buffer = remove_crln(buffer);
				if (pos != -1) // && this->Clients[pos].GetifReg() == false
				{
					// std::cout << "size: " << (this->Clients[pos].GetInBuffer()).size() << std::endl;
					// for (size_t j = 0; j < (this->Clients[pos].GetInBuffer()).size(); j++)
					// 	std::cout << (this->Clients[pos].GetInBuffer())[j] << " , ";
					cmds = split_input(this->Clients[pos].GetInBuffer(), "\r\n");
					for (size_t j = 0; j < cmds.size(); j++)
					{
						// std::cout << "Cmd " << j + 1 << ": \'" << cmds[j] << "\'"<< std::endl;
						// std::cout << "----------------------" << std::endl;
						// std::cout << "Command: \'" << args.first << "\'" << std::endl;
						// for (size_t j = 0; j < args.second.size(); j++)
						// 	std::cout << "Arg " << j + 1 << ": \'" << args.second[j] << "\'"<< std::endl;
						// std::cout << "----------------------" << std::endl;
						// std::vector<std::time_t>	&CperT = this->Clients[pos].GetCperT();
						// if (!cmds[j].empty())
						// {
						// 	CperT.push_back(std::time(NULL));
						// 	if (CperT.size() >= 3 && CperT[2] - CperT[0] > 5)
						// 	{
								
						// 	}
						// }
						// if ((this->Clients[pos].GetInBuffer()).find_first_of("\r") == std::string::npos)
						// 	return (send_server_msg(this->Clients[pos], ERR_UNKNOWNERROR(this->Clients[pos].GetNickname(), "\'" + (this->Clients[pos].GetInBuffer()).substr(0, (this->Clients[pos].GetInBuffer()).size() - 1) + "\'")));
						args = extract_args(cmds[j]); 
						commands(args, this->Clients[pos]);
						memset(tmp, 0, sizeof(tmp));
						this->Clients[pos].ClearBuffer();
					}
				}
			// }
		}
		else
			return ;
	}
}

void	Server::Accept_New_Client()
{
	struct sockaddr_in	newcliaddr;
	struct pollfd		newpoll;
	int					clifd;
	socklen_t			clilen;
	Client				newclient;
	
	
	clilen = sizeof(newcliaddr);
	clifd = accept(this->SockFd, (struct sockaddr *)&newcliaddr, &clilen);
	if (clifd == -1)
	{
		std::cerr << "Failed to accept a new client." << std::endl;
		return ;
	}
	if (fcntl(clifd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Failed to set the client socket to non-blocking." << std::endl;
		return ;
	}
	// if (this->find_IP(inet_ntoa(newcliaddr.sin_addr), this->Clients) > -1)
	// {
	// 	if (send(clifd, "Too many connections from this IP address.\r\n", 44, 0) == -1)
	// 		std::cerr << "Failed to send to client " << clifd << std::endl;
	// 	close(clifd);
	// }
	else
	{
		std::string	welcome;

		welcome.append("\n");
		welcome.append(" /$$      /$$ /$$$$$$$$ /$$        /$$$$$$   /$$$$$$  /$$      /$$ /$$$$$$$$\n");
        welcome.append("| $$  /$ | $$| $$_____/| $$       /$$__  $$ /$$__  $$| $$$    /$$$| $$_____/ \n");
        welcome.append("| $$ /$$$| $$| $$      | $$      | $$  \\__/| $$  \\ $$| $$$$  /$$$$| $$      \n");
        welcome.append("| $$/$$ $$ $$| $$$$$   | $$      | $$      | $$  | $$| $$ $$/$$ $$| $$$$$   \n");
        welcome.append("| $$$$_  $$$$| $$__/   | $$      | $$      | $$  | $$| $$  $$$| $$| $$__/   \n");
        welcome.append("| $$$/ \\  $$$| $$      | $$      | $$    $$| $$  | $$| $$\\  $ | $$| $$      \n");
        welcome.append("| $$/   \\  $$| $$$$$$$$| $$$$$$$$|  $$$$$$/|  $$$$$$/| $$ \\/  | $$| $$$$$$$$\n");
        welcome.append("|__/     \\__/|________/|________/ \\______/  \\______/ |__/     |__/|________/     \n");
        welcome.append("                                                                          \n");
        welcome.append("                             /$$$$$$$$ /$$$$$$                              \n");
        welcome.append("                            |__  $$__//$$__  $$                             \n");
        welcome.append("                               | $$  | $$  \\ $$                             \n");
        welcome.append("                               | $$  | $$  | $$                             \n");
        welcome.append("                               | $$  | $$  | $$                             \n");
        welcome.append("                               | $$  | $$  | $$                             \n");
        welcome.append("                               | $$  |  $$$$$$/                             \n");
        welcome.append("                               |__/   \\______/                              \n");
        welcome.append("                                                                          \n");
        welcome.append("     /$$$$$$$  /$$$$$$  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$  /$$$$$$$  \n");
        welcome.append("    | $$__  $$|_  $$_/ /$$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$__  $$   \n");
        welcome.append("    | $$  \\ $$  | $$  | $$  \\__/| $$  \\__/| $$  \\ $$| $$  \\ $$| $$  \\ $$   \n");
        welcome.append("    | $$  | $$  | $$  |  $$$$$$ | $$      | $$  | $$| $$$$$$$/| $$  | $$   \n");
        welcome.append("    | $$  | $$  | $$   \\____  $$| $$      | $$  | $$| $$__  $$| $$  | $$   \n");
        welcome.append("    | $$  | $$  | $$   /$$  \\ $$| $$    $$| $$  | $$| $$  \\ $$| $$  | $$   \n");
        welcome.append("    | $$$$$$$/ /$$$$$$|  $$$$$$/|  $$$$$$/|  $$$$$$/| $$  | $$| $$$$$$$/   \n");
        welcome.append("    |_______/ |______/ \\______/  \\______/  \\______/ |__/  |__/|_______/       \n");
        welcome.append("                                                                          \n");
        welcome.append("       /$$      /$$ /$$$$$$$  /$$$$$$$$ /$$$$$$$  /$$$$$$$$ /$$$$$$$$   \n");
        welcome.append("      | $$$    /$$$| $$__  $$| $$_____/| $$__  $$| $$_____/|_____ $$/     \n");
        welcome.append("      | $$$$  /$$$$| $$  \\ $$| $$      | $$  \\ $$| $$           /$$/      \n");
        welcome.append("      | $$ $$/$$ $$| $$  | $$| $$$$$   | $$$$$$$/| $$$$$       /$$/       \n");
        welcome.append("      | $$  $$$| $$| $$  | $$| $$__/   | $$__  $$| $$__/      /$$/        \n");
        welcome.append("      | $$\\  $ | $$| $$  | $$| $$      | $$  \\ $$| $$        /$$/         \n");
        welcome.append("      | $$ \\/  | $$| $$$$$$$/| $$$$$$$$| $$  | $$| $$$$$$$$ /$$/          \n");
        welcome.append("      |__/     |__/|_______/ |________/|__/  |__/|________/|__/              \n\r\n");
		if (send(clifd, welcome.c_str(), welcome.size(), 0) == -1)
			std::cerr << "Failed to send to client " << clifd << std::endl;
		newclient.SetFd(clifd);
		newclient.SetIPaddr(inet_ntoa(newcliaddr.sin_addr));
		this->Clients.push_back(newclient);
		newpoll.fd = clifd;
		newpoll.events = POLLIN;
		newpoll.revents = 0;
		this->Fds.push_back(newpoll);
		std::cout << display_current_time() << " New Client Connected." << std::endl;
		sleep (1);
	}
}

void	Server::Server_Initialization(char **av)
{
	size_t	i;

	this->Port = atoi(av[1]);
	Server_Socket_Creation();
	std::cout << display_current_time() << " Server connected." << std::endl;
	while (1)
	{
		if (Server::Signal == false)
		{
			if (Server::Signal == false && poll(&this->Fds[0], this->Fds.size(), -1) == -1)
				throw (std::runtime_error("Failed to check for event."));
			for (i = 0; i < this->Fds.size(); i++)
			{
				if (this->Fds[i].revents & POLLIN)
				{
					if (i == 0)
						Accept_New_Client();
					else
						receive_request(this->Fds[i].fd);
				}
			}
		}
		else
		{
			this->Disconnect_Everything();
			exit(1);
		}	
	}
}

void	Server::Server_Socket_Creation()
{
	struct sockaddr_in	newsockaddr;
	struct pollfd		newpoll;
	int					optval;

	this->SockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->SockFd == -1)
		throw (std::runtime_error("Failed to create a socket for the server."));
	optval = 1;
	if (setsockopt(this->SockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw (std::runtime_error("Failed to set the server socket options."));
	if (fcntl(this->SockFd, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("Failed to set the server socket to non-blocking."));
	newsockaddr.sin_family = AF_INET;
	newsockaddr.sin_port = htons(this->Port);
	newsockaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->SockFd, (struct sockaddr *)&newsockaddr, sizeof(newsockaddr)) == -1)
		throw (std::runtime_error("Failed to bind the server socket to the port."));
	if (listen(this->SockFd, SOMAXCONN) == -1)
		throw (std::runtime_error("Failed to switch the server socket to a passive socket."));
	newpoll.fd = this->SockFd;
	newpoll.events = POLLIN;
	newpoll.revents = 0;
	this->Fds.push_back(newpoll);
}

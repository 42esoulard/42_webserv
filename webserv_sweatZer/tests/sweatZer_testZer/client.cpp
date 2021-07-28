/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/02 14:05:12 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/28 11:57:27 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Client side C/C++ program to demonstrate Socket programming
// from Medium tutorial : https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <vector>

#define _MAXLINE 655360
#define PORT 8080
#define NB_REQUESTS 9

void		init_vec(std::vector<std::vector<std::string> > &requests)
{
	requests[0].push_back("GET with length < body");
	requests[0].push_back("content of index.html");
	requests[0].push_back("GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nContent-Length:2\r\n\r\nhello=test\r\n\r\n");

	requests[1].push_back("GET with length > body");
	requests[1].push_back("Error 400");
	requests[1].push_back("GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nContent-Length:2000\r\n\r\nhello=test\r\n\r\n");

	requests[2].push_back("GET without length");
	requests[2].push_back("Error 411");
	requests[2].push_back("GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\nhello=test\r\n\r\n");

	requests[3].push_back("GET noperm");
	requests[3].push_back("Error 403");
	requests[3].push_back("GET /noperm HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\n");

	requests[4].push_back("GET bad version");
	requests[4].push_back("Error 505");
	requests[4].push_back("GET / HTTP/1.2\r\nHost: 127.0.0.1:8080\r\n\r\n");

	requests[5].push_back("GET inexistent file");
	requests[5].push_back("Error 404");
	requests[5].push_back("GET /inexistent HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\n");

	requests[6].push_back("POST 1000");
	requests[6].push_back("post1000 in www/sweatzer/uploads, size 1000");
	requests[6].push_back("POST /post1000 HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nContent-Length: 1000\r\n\r\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam dictum porta laoreet. Pellentesque porta tempus eros tristique vehicula. Mauris eget ex turpis. Mauris ultrices auctor ultricies. Donec congue enim nec ex consectetur finibus. Sed ac rhoncus est. Etiam lectus leo, varius at magna venenatis, sollicitudin porttitor urna. Donec porttitor id orci et pellentesque. Phasellus ex purus, dapibus eu velit et, faucibus venenatis sapien. Ut eget turpis quis neque dignissim pellentesque. Suspendisse sollicitudin est ut arcu facilisis feugiat sed non ex. \nProin nulla eros, bibendum et erat et, pellentesque tincidunt quam. Nullam eleifend nulla sollicitudin urna volutpat, eu tincidunt dui suscipit. Curabitur a fringilla leo. Ut sit amet sapien scelerisque, efficitur libero posuere, molestie ipsum. Etiam ut tortor tristique, pretium lacus ac, convallis quam. Pellentesque molestie sapien id libero sagittis volutpat. Maecenas interdum ligula augue, lobortis ullamcorper ni consectetur eros.\r\n\r\n");

	requests[7].push_back("DELETE post1000 forbidden");
	requests[7].push_back("Error 403");
	requests[7].push_back("DELETE /uploads/post1000 HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\n");

	requests[8].push_back("DELETE post1000 after chmod");
	requests[8].push_back("204 & file deleted");
	requests[8].push_back("DELETE /uploads/post1000 HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\n");

}

int send_request(std::string &name, std::string &expected, std::string &request)
{
    int sock = 0;
    long valread;
    struct sockaddr_in serv_addr;
    char buffer[_MAXLINE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        strerror(errno);
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        strerror(errno);
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        strerror(errno);
        return -1;
    }
    send(sock , request.c_str(), request.size()  , 0 );
    std::cout << "------------------------------------------\n" << "\033[1m" << name << "\033[0m" << std::endl;
	std::cout << "\033[32mExpected: " << expected << "\033[0m" << std::endl;
	std::cout << "------------------------------------------" << std::endl;
    valread = read( sock , buffer, _MAXLINE);
	std::string	response(buffer);
	size_t	i = response.find("\r\n\r\n");
	if (response.size() - i > 500)
		std::cout << response.substr(0, i) << "\r\n\r\n" << "[body too long]" << std::endl;
	else
    	std::cout << response << std::endl;
    return 0;
}

int		main(int argc, char **argv)
{
	if (argc > 1)
	{
		int		fd;
		char	buf[_MAXLINE];

		memset(buf, 0, _MAXLINE);
		if ((fd = open(argv[1], O_RDONLY)) < 0)
		{
			std::cerr << "WRONG FILE" << std::endl;
			return (1);
		}
		if (read(fd, buf, _MAXLINE) < 0)
		{
			std::cerr << "READ ERROR" << std::endl;
			return (1);
		}
		std::string					req(buf);
		std::vector<std::string>	request;
		size_t	i = req.find("\n");
		if (i == std::string::npos)
		{
			std::cerr << "WRONG FORMAT" << std::endl;
			return (1);
		}
		request.push_back(req.substr(0, i));
		size_t	j = req.find("\n", ++i);
		if (j == std::string::npos)
		{
			std::cerr << "WRONG FORMAT" << std::endl;
			return (1);
		}
		request.push_back(req.substr(i, j - i));
		request.push_back(req.substr(++j));
		size_t		found = 0;
		while ((found = request[2].find("#\n")) != std::string::npos)
		{
			request[2].replace(found, 2, "\r\n");
			found += 2;
		}
		if (send_request(request[0], request[1], request[2]))
		{
			std::cerr << "TEST ERROR" << std::endl;
			return (1);
		}
		return (0);
	}

	std::vector<std::vector<std::string> >	requests(NB_REQUESTS);

	init_vec(requests);
	for (size_t i = 0 ; i < requests.size() ; ++i)
	{
		if (send_request(requests[i][0], requests[i][1], requests[i][2]))
		{
			std::cerr << "TEST ERROR" << std::endl;
			return (1);
		}
		if (i == 6)
		{
			std::cout << "\033[34mPlease check /uploads/post1000, then press enter\033[0m" << std::endl;
			getchar();
		}
		if (i == 7)
			chmod("../../www/sweatzer/uploads/post1000", 0666);
		if (i == 8)
		{
			std::cout << "\033[34mPlease check that /uploads/post1000 has been deleted, then press enter\033[0m" << std::endl;
			getchar();
		}
	}
	return (0);
}

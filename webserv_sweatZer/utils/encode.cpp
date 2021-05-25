/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encode.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 09:12:20 by rturcey           #+#    #+#             */
/*   Updated: 2021/05/25 10:47:43 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string			encode(std::string str)
{
	char		b[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string	result;
	size_t		i = -1;
	size_t		rest = str.length() % 3;
	int			stream;

	while (rest && ++i < rest)
		str += '\0';
	i = 0;
	while (i < str.length())
	{
		if (i > 0 && !((i / 3 * 4) % 76))
			result += "\r\n";
		stream = (str[i] << 16) + (str[i + 1] << 8) + str[i + 2];
		result += b[(stream >> 18) & 63];
		result += b[(stream >> 12) & 63];
		result += b[(stream >> 6) & 63];
		result += b[(stream) & 63];
		i += 3;
	}
	if (rest)
	{
		result = result.substr(0, result.length() - (3 - rest));
		while (rest++ < 3)
			result += '=';
	}
	return (result);
}

std::string			decode(std::string str)
{
	std::string	 b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string	result;
	size_t		i = 0;
	size_t		pos;
	int			stream;

	while ((pos = str.find("\r\n")) != std::string::npos)
		str.erase(pos, 2);
	while ((pos = str.find('=')) != std::string::npos)
		str[pos] = 'A';
	while (i < str.length())
	{
		stream = (b.find(str[i]) << 18) + (b.find(str[i + 1]) << 12) + (b.find(str[i + 2]) << 6) + b.find(str[i + 3]);
		result += (stream >> 16) & 255;
		result += (stream >> 8) & 255;
		result += stream & 255;
		i += 4;
	}
	while ((pos = str.find('\0')) != std::string::npos)
		str.erase(pos);
	return (result);
}

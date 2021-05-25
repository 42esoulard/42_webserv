/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encoder.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 10:49:09 by rturcey           #+#    #+#             */
/*   Updated: 2021/05/25 10:52:35 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

int		main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "How it works: ./encoder \"your message\"" << std::endl;
		return (1);
	}
	std::string	str(av[1]);
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
	std::cout << result << std::endl;
	return (0);
}

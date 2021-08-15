/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conv_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/15 11:15:29 by esoulard          #+#    #+#             */
/*   Updated: 2021/08/07 19:21:01 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "utils.hpp"


size_t		ft_stosize_t(std::string stri)
{
	size_t result;
	int i;
	const char *str = stri.c_str();

	i = 0;
	result = 0;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\v' || str[i] == '\f' || str[i] == '\r')
		i++;
	if (str[i] == '+')
		i++;
	
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - 48);
		i++;
	}
	return (result);
}

int		ft_atoi(const char *str)
{
	int result;
	int minus;
	int i;

	i = 0;
	result = 0;
	minus = 1;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\v' || str[i] == '\f' || str[i] == '\r')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			minus *= -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - 48);
		i++;
	}
	return (result * minus);
}

int		ft_stoi(std::string str) {
	return ft_atoi(str.c_str());
}

int		ft_atoi_hex(std::string &str)
{
	int 	result = 0;
	int 	i = 0;
	int		len = 0;
	size_t	f;
	std::string hexa = "0123456789abcdefgh";

	while (hexa.find(str[len]) != std::string::npos)
		len++;
	if (len == i)
		return (-1);
	len -= i;
	while ((f = hexa.find(str[i++])) != std::string::npos)
		result += ft_pow((int)f, 16, --len);
	return (result);
}

int		ft_pow(int nb, int f, int power)
{
	while (power--)
		nb *= f;
	return (nb);
}

int		ft_stoi_hex(std::string str)
{
	lower(str);
	return ft_atoi_hex(str);
}
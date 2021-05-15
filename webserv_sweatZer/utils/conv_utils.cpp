/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conv_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/15 11:15:29 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/15 11:18:06 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

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
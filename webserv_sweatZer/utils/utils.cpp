/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 16:04:01 by rturcey           #+#    #+#             */
/*   Updated: 2021/08/07 18:17:14 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

void ft_memdel(void **ptr) {
	if (ptr != NULL) {
		free(*ptr);
		*ptr = NULL;
	}
}

bool                         comp_float(std::pair<float, std::string> &p1, std::pair<float, std::string> &p2)
{
    if (p1.first > p2.first)
        return (1);
    return (0);
}

std::string                  cap_alpha(std::string &str, size_t &index)
{
    std::string     ret;
    while (is_alpha(str[index]))
    {
        ret.push_back(str[index]);
        index++;
    }
    return (ret);
}

std::string                  cap_alphanum(std::string &str, size_t &index)
{
    std::string     ret;
    while (is_alnum(str[index]))
    {
        ret.push_back(str[index]);
        index++;
    }
    return (ret);
}

bool				is_num(std::string &str)
{
	for (size_t i = 0 ; i < str.size() ; i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
	}
	return (1);
}

void				upper(std::string &str)
{
	for (size_t i = 0 ; i < str.size() ; i++)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 32;
	}
}

std::string			format_env(const std::string &strc)
{
	std::string	str(strc);
	upper(str);
	size_t	found = 0;
	while ((found = str.find('-')) != std::string::npos)
		str[found] = '_';
	return ("HTTP_" + str);
}

void				lower(std::string &str)
{
	for (size_t i = 0 ; i < str.size() ; i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
	}
}

bool			  is_alpha(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	return (0);
}

bool			  is_alnum(char c)
{
	if (is_alpha(c) || (c >= '0' && c <= '9'))
		return (1);
	return (0);
}


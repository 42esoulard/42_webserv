/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 12:08:27 by rturcey           #+#    #+#             */
/*   Updated: 2021/06/22 13:44:17 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include <vector>

std::vector<std::string>     split(std::string &str, char c, int max)
{
    std::vector<std::string>    vec;
    size_t                      i = 0;

    if (str.empty())
        return (vec);
    size_t          prev = 0;
    while (str[i] && str[i++] == c)
    {
        prev++;
        i++;
    }
    while (i < str.size())
    {
        while (str[i] && str[i] != c)
            i++;
        if (prev != i)
            vec.push_back(str.substr(prev, i - prev));
        if (--max == 0)
        {
            ++i;
            while (is_space(str[i]))
                i++;
            vec.push_back(str.substr(i));
            return (vec);
        }
        prev = i;
        while (str[i] && str[i] == c)
        {
            prev++;
            i++;
        }
    }
    return (vec);
}

std::vector<std::string>     split_crlf(std::string &str, size_t *body, int *err)
{
    std::vector<std::string>    vec;
	size_t						size = 0;
    size_t                      i = -1;

    if (str.empty())
        return (vec);
    size_t          prev = 0;
    while (i + 1 < str.size())
    {
        if ((i = str.find("\r\n", i + 1)) == std::string::npos)
			break ;
		size += i - prev;
		if (*body == (size_t)-1)
		{
			if (size > _MAXHEADERSIZE)
			{
				*err = 1;
				return (vec);
			}
		}
        vec.push_back(str.substr(prev, i - prev));
		if (vec.back().empty())
		{
            if (!str.substr(i + 2).empty())
            {
                *body = vec.size();
			    vec.back() = str.substr(i + 2);
            }
			break ;
		}
        prev = i + 2;
    }
    return (vec);
}

std::vector<std::string>     split_chunked(std::string &str)
{
    std::vector<std::string>    vec;
    size_t                      i = -1;

    if (str.empty())
        return (vec);
    size_t          prev = 0;
    while (i + 1 < str.size())
    {
        if ((i = str.find("\r\n", i + 1)) == std::string::npos)
			break ;
        if (!str.substr(prev, i - prev).empty())
            vec.push_back(str.substr(prev, i - prev));
        else
            vec.back() += "\r\n";
        prev = i + 2;
    }
    return (vec);
}

std::vector<std::string>     split(std::string &str, char c)
{
    std::vector<std::string>    vec;
    size_t                      i = 0;

    if (str.empty())
        return (vec);
    size_t          prev = 0;
    while (str[i] && str[i++] == c)
    {
        prev++;
        i++;
    }
    while (i < str.size())
    {
        while (str[i] && str[i] != c)
            i++;
        if (prev != i)
            vec.push_back(str.substr(prev, i - prev));
        prev = i;
        while (str[i] && str[i] == c)
        {
            prev++;
            i++;
        }
    }
    return (vec);
}

std::vector<std::string>     split_sp(std::string &str)
{
    std::vector<std::string>    vec;
    size_t                      i = 0;

    if (str.empty())
        return (vec);
    size_t          prev = 0;
    while (str[i] && is_space(str[i]))
    {
        prev++;
        i++;
    }
    while (i < str.size())
    {
        while (str[i] && !is_space(str[i]))
            i++;
        if (prev != i)
            vec.push_back(str.substr(prev, i - prev));
        prev = i;
        while (str[i] && is_space(str[i]))
        {
            prev++;
            i++;
        }
    }
    return (vec);
}

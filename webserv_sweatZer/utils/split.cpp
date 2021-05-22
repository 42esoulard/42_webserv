/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 12:08:27 by rturcey           #+#    #+#             */
/*   Updated: 2021/05/22 13:06:47 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include <vector>

bool                         comp_float(std::pair<float, std::string> &p1, std::pair<float, std::string> &p2)
{
	std::cout << "F=" << p1.first << " FBIS=" << p2.first << std::endl;
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

std::vector<std::string>     split_crlf(std::string &str, size_t *body)
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

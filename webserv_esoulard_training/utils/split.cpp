/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 12:08:27 by rturcey           #+#    #+#             */
/*   Updated: 2021/04/15 16:40:46 by rturcey          ###   ########.fr       */
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_map.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 15:21:42 by rturcey           #+#    #+#             */
/*   Updated: 2021/04/15 15:46:00 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

void        print_map(std::map<std::string, std::string> map)
{
    std::map<std::string, std::string>::iterator    it = map.begin();
    while (it != map.end())
    {
        std::cout << (*it).first << " = " << (*it).second << std::endl;
        ++it; 
    }
}
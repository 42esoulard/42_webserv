/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_map.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 15:21:42 by rturcey           #+#    #+#             */
/*   Updated: 2021/05/19 09:50:14 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

void        print_map(std::map<std::string, std::list<std::string> > map)
{
    std::map<std::string, std::list<std::string> >::iterator    it = map.begin();
    while (it != map.end())
    {
        for (std::list<std::string>::iterator iter = (*it).second.begin() ; iter != (*it).second.end() ; ++iter)
        {
            std::cout << (*it).first << " = " << (*iter) << std::endl;
        }
        ++it;
    }
}

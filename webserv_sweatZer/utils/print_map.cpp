/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_map.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 15:21:42 by rturcey           #+#    #+#             */
/*   Updated: 2021/07/22 16:32:30 by esoulard         ###   ########.fr       */
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
            if ((*it).first == "body")
                std::cerr << "body len = " << (*iter).size() << std::endl;
            else
                std::cerr << (*it).first << " = " << (*iter) << std::endl;
        }
        ++it;
    }
}

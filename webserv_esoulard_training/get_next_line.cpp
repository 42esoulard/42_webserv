/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 15:46:34 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/04 16:05:59 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*give_it_back(char *str, char **line)
{
	int		len;
	char	*result;

	len = 0;
	result = NULL;
	while (str[len] && str[len] != '\n')
		len++;
	*line = ft_strndup(str, len);
	if (str[len] && str[len + 1])
		result = strdup(&str[len + 1]);
	//free(str);
	return (result);
}

int		get_next_line(int fd, char **line)
{
	std::string	*s[OPEN_MAX];
    int         buffsize = 32;
	char		buffer[buffsize];
	int			i;

	if (!line || fd < 0 || (read(fd, 0, 0)) < 0)
		return (-1);
	if (s[fd] == NULL)
		*(s[fd]) = std::string("");
	i = 0;
	while (strchr(s[fd]->c_str(), '\n') || (i = read(fd, buffer, buffsize)) > 0)
	{
		buffer[i] = '\0';
		*(s[fd]) += std::string(buffer);
		if (strchr(s[fd]->c_str(), '\n') && ++i)
			break ;
	}
	if (i >= 0)
		s[fd] = give_it_back(s[fd], line);
	if (i > 0)
		return (1);
	return (i);
}
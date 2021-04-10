/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/10 17:28:29 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/10 18:25:35 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_HPP
# define GET_NEXT_LINE_HPP
# include <unistd.h>
# include <stdlib.h>
# include <limits.h>

#define BUFFER_SIZE 32

int			get_next_line(int fd, char **line);
char		*give_it_back(char *str, char **line);
char		*ft_strjoin(char *s1, char *s2);
char		*ft_strchr(const char *s, int c);
char		*ft_strdup(const char *s1);
char		*ft_strndup(const char *s1, int len);
int         pass_spaces(char *line, int index);

#endif
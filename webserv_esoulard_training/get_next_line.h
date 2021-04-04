/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 15:47:55 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/04 16:06:03 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include <cstring>
# include <string>

int			get_next_line(int fd, char **line);
char		*give_it_back(char *str, char **line);
char		*ft_strjoin(char *s1, char *s2);

char		*ft_strdup(const char *s1);
char		*ft_strndup(const char *s1, int len);

#endif
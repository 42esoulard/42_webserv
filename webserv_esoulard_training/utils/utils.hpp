/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/10 17:28:29 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/15 15:29:35 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP
# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include <vector>
# include <string>
# include <map>
# include <iostream>

#define BUFFER_SIZE 32

int		            	    get_next_line(int fd, char **line);
char	            	    *give_it_back(char *str, char **line);
char	            	    *ft_strjoin(char *s1, char *s2);
char	                 	*ft_strchr(const char *s, int c);
char	                	*ft_strdup(const char *s1);
char	                	*ft_strndup(const char *s1, int len);
int                         pass_spaces(char *line, int &index);
bool                        is_space(char a);
char	                	*ft_itoa(int n);
std::vector<std::string>     split_sp(std::string &str);
std::vector<std::string>     split(std::string &str, char c);
std::vector<std::string>     split(std::string &str, char c, int max);
bool			             is_alpha(char c);
void                         print_map(std::map<std::string, std::string> map);

#endif
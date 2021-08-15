/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/10 17:28:29 by esoulard          #+#    #+#             */
/*   Updated: 2021/08/07 19:21:34 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include <vector>
# include <list>
# include <string>
# include <map>
# include <iostream>
# include <algorithm>

#if defined(__linux__)
	#define OPEN_MAX 256
#endif

#define BUFFER_SIZE 32
#define _MAXHEADERSIZE 65536

int		            	    get_next_line(int fd, char **line);
char	            	    *give_it_back(char *str, char **line);
char	            	    *ft_strjoin(char *s1, const char *s2);
char	                 	*ft_strchr(const char *s, int c);
char	                	*ft_strdup(const char *s1);
char	                	*ft_strndup(const char *s1, int len);
void						ft_memdel(void **ptr);
int                         pass_spaces(char *line, int &index);
int                         pass_spaces(std::string &line, size_t &index);
bool                        is_space(char a);
char	                	*ft_itoa(int n);
std::vector<std::string>     split_sp(std::string &str);
std::vector<std::string>     split(std::string &str, char c);
std::vector<std::string>     split_crlf(std::string &str, size_t *body, int *err);
std::vector<std::string>     split_chunked(std::string &str);
std::vector<std::string>     split(std::string &str, char c, int max);
bool			             is_alpha(char c);
void                         print_map(std::map<std::string, std::list<std::string> > map);
std::string                  cap_alpha(std::string &str, size_t &index);
std::string                  cap_alphanum(std::string &str, size_t &index);
bool                         comp_float(std::pair<float, std::string> &p1, std::pair<float, std::string> &p2);
bool			  			is_alnum(char c);
int							ft_atoi(const char *str);
int							ft_stoi(std::string str);
size_t						ft_stosize_t(std::string stri);
void						lower(std::string &str);
void						upper(std::string &str);
std::string					format_env(const std::string &strc);
bool						is_num(std::string &str);
int							ft_pow(int nb, int f, int power);
int							ft_stoi_hex(std::string str);
int							ft_atoi_hex(std::string &str);
std::string					ft_itos(int n);
std::string					encode(std::string str);
std::string					decode(std::string str);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/10 17:44:34 by esoulard          #+#    #+#             */
/*   Updated: 2021/06/03 16:51:56 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

char	*ft_strjoin(char *s1, const char *s2) {
	
	char	*new_str;
	int		s1len;
	int		i;
	int		j;

	s1len = 0;
	i = -1;
	j = 0;
	new_str = NULL;
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	while (s1[s1len])
		s1len++;
	while (s2[j])
		j++;
	if (!(new_str = (char *)malloc(sizeof(char) * (s1len + j + 1))))
		return (NULL);
	while (s1[++i])
		new_str[i] = s1[i];
	j = 0;
	while (s2[j])
		new_str[i++] = s2[j++];
	new_str[i] = '\0';
	free(s1);
	return (new_str);
}

char	*ft_strchr(const char *s, int c) {

	char	*haystack;
	char	needle;
	int		i;

	haystack = (char *)s;
	needle = (char)c;
	i = -1;
	while (haystack[++i])
	{
		if (haystack[i] == needle)
			return ((char *)&s[i]);
	}
	if (needle == '\0' && haystack[i] == '\0')
		return ((char *)&s[i]);
	return (NULL);
}

char	*ft_strdup(const char *s1) {

	int		len;
	char	*cpy;
	int		i;

	len = 0;
	while (s1[len])
		len++;
	if (!(cpy = (char *)malloc(sizeof(char) * len + 1)))
		return (NULL);
	i = 0;
	while (s1[i])
	{
		cpy[i] = s1[i];
		i++;
	}
	cpy[i] = '\0';
	return (cpy);
}

char	*ft_strndup(const char *s1, int len) {

	char	*cpy;
	int		i;

	if (!(cpy = (char *)malloc(sizeof(char) * (len + 1))))
		return (NULL);
	i = 0;
	while (s1[i] && i < len)
	{
		cpy[i] = s1[i];
		i++;
	}
	cpy[i] = '\0';
	return (cpy);
}

bool is_space(char a) {
	if (a == ' ' || a == '\t'  || a == '\n'  || a == '\v'
		 || a == '\f'  || a ==  '\r')
		 return true;
	return false;
}

int pass_spaces(char *line, int &index) {
	
	if (!line)
		return index;
	while (line && is_space(line[index]))
		index++;
	return index;
}

int pass_spaces(std::string &line, size_t &index) {
	
	if (line.empty())
		return index;
	while (index < line.size() && is_space(line[index]))
		index++;
	return index;
}
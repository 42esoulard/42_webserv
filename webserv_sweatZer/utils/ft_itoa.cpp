/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rturcey <rturcey@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 15:32:47 by esoulard          #+#    #+#             */
/*   Updated: 2021/05/23 15:01:36 by rturcey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "utils.hpp"

static size_t     ft_strlen(char *str) {
    int size = -1;

    while (str && str[++size]);
    return size;
}

static char		*rev_tab(char *tab)
{
	size_t	i;
	size_t	j;
	size_t	len;
	char	stock;

	i = 0;
	len = ft_strlen(tab);
	j = len - 1;
	while (i < (ft_strlen(tab) / 2))
	{
		stock = tab[i];
		tab[i] = tab[j];
		tab[j] = stock;
		i++;
		j--;
	}
	tab[len] = '\0';
	return (tab);
}

static char		*init_tab(int quot, int n, int minus)
{
	char	*tab;
	int		i;
	long	j;

	j = n;
	if (n < 0)
		j *= -1;
	tab = NULL;
	i = 0;
	if (!(tab = (char *)malloc(sizeof(char) * quot + 2)))
		return (NULL);
	while (j > 9)
	{
		tab[i++] = (j % 10) + 48;
		j = j / 10;
	}
	tab[i] = j + 48;
	if (minus < 0)
		tab[++i] = '-';
	tab[i + 1] = '\0';
	return (rev_tab(tab));
}

char			*ft_itoa(int n)
{
	char	*result;
	long	i;
	int		quot;
	int		minus;

	i = n;
	quot = 0;
	minus = 1;
	if (n < 0)
	{
		minus *= -1;
		i *= -1;
		quot += 1;
	}
	while (i > 9)
	{
		quot++;
		i = i / 10;
	}
	result = init_tab(quot, n, minus);
	return (result);
}

std::string		ft_itos(int n)
{
	std::string	result;
	long		i = n;

	if (n < 0)
		i *= -1;
	while (i > 9)
	{
		result += i % 10 + '0';
		i /= 10;
	}
	result += i + '0';
	if (n < 0)
		result = '-';
	std::reverse(result.begin(), result.end());
	return (result);
}

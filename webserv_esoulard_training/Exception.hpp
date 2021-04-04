/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/04 14:14:30 by esoulard          #+#    #+#             */
/*   Updated: 2021/04/04 16:06:04 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#if defined(__linux__)
	#define _NOEXCEPT _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
#endif

#include <string>

class Exception : public std::exception {
			
	public:
		Exception(std::string const &str) : _err(str) {};
		virtual char const *what() const _NOEXCEPT {return _err.c_str();};
       	virtual ~Exception() _NOEXCEPT {};

	protected:
		std::string const _err;	
};

#endif
/**
 * @file		order.hpp
 * @brief		Order class for exchange
 * @author		Jeong Hoon (Sian) Choi
 * @version 	1.0.0
 * @date		2024-06-15
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"

#ifndef _HEADER_ORDER_HPP_
#define _HEADER_ORDER_HPP_

#include "stradian/order.h"

#if _TARGET_OS == OS_WINDOWS

#elif _TARGET_OS == OS_LINUX

/* Inline & Template Definition */

template <typename T>
std::string stradian::Order::to_string_with_precision(const T value, int n) {
	return stradian::to_string_with_precision(value, n);
}

template <typename T>
std::string stradian::to_string_with_precision(const T value, const int n) {
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << value;
	return std::move(out.str());
}

#endif // OS dependency

#endif // Header duplicate

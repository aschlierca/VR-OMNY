#pragma once

#include <stdexcept>
#include <string>

class OutOfRangeInputException : public std::out_of_range
{
public:
	explicit OutOfRangeInputException(const std::string& msg)
		: std::out_of_range("OutOfRangeInput: " + msg) {}
};

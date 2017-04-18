#pragma once

#include <stdexcept>

class ASException : public std::runtime_error
{
public:
	ASException(const std::string& message, int errcode, const std::string& file, int line);
	~ASException() = default;

	const char* what() const noexcept;

	static const char* GetMessage(int code) noexcept;

private:
	std::string mMessage;
};

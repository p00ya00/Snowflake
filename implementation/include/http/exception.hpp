#ifndef SF_HTTP_EXCEPTION_HPP
#define SF_HTTP_EXCEPTION_HPP

#include <boost/exception/all.hpp>
#include <string>

namespace sf
{
namespace http
{

//typedef boost::error_info<struct tag_error_code, unsigned char> error_code;
typedef boost::error_info<struct tag_error_msg , std::string  > errorMessage;

struct BadRequestException: 
	virtual public std::exception, 
	virtual public boost::exception 
{};

struct UriTooLargeException: 
	virtual public std::exception, 
	virtual public boost::exception 
{};

}
}

#endif
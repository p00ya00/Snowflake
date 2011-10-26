#include <boost/exception/all.hpp>
#include <string>

typedef boost::error_info<struct tag_error_msg , std::string  > errorMessage;

struct InvalidLogException: 
	virtual public std::exception, 
	virtual public boost::exception 
{};
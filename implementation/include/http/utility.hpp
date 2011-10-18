#ifndef SF_HTTP_UTILITY_HPP
#define SF_HTTP_UTILITY_HPP

#include <string>
#include <boost/date_time.hpp>

namespace sf
{
namespace http
{

void percentDecode(std::string &uri);
static int hexToDec(const std::string &hex);

class DateTime
{
public:
	static boost::posix_time::ptime fromString(const std::string &ts);
	static std::string toString(const boost::posix_time::ptime &pt);
};

}
}

#endif
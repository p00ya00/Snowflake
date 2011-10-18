#include <sstream>
#include <http/utility.hpp>
using namespace sf::http;

void sf::http::percentDecode(std::string &uri)
{
	for(size_t index = 0; index < uri.size(); ++index)
	{
		if(uri[index] == '%')
		{
			if(index == uri.size() - 1)
				uri[index] = '_';
			else if(index == uri.size() - 2)
			{
				uri[index] = hexToDec(uri.substr(index + 1, 1));
				uri.pop_back();
			}
			else
			{
				uri[index] = hexToDec(uri.substr(index + 1, 2));
				uri.erase(index + 1, 2);
			}
		}
	}
}

int sf::http::hexToDec(const std::string &hex)
{
	int i = 0x5F; //default: _
	std::stringstream sstream;

	sstream << std::hex << hex;
	sstream >> i;

	return i;
}

boost::posix_time::ptime DateTime::fromString(const std::string &ts)
{
	//to be implemented
	return boost::posix_time::ptime();
}

std::string DateTime::toString(const boost::posix_time::ptime &pt)
{
	//to be implemented
	return "";
}
#ifndef SF_HTTP_HPP
#define SF_HTTP_HPP

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <string>

#define CR   '\r'
#define LF   '\n'
#define CRLF "\r\n"

namespace sf
{
namespace http
{

namespace str_algo = boost::algorithm;

typedef unsigned short int StatusCode;

}
}

#endif
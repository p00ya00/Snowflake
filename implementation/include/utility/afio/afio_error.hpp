#ifndef SF_AFIO_AFIO_ERROR_HPP
#define SF_AFIO_AFIO_ERROR_HPP

#include <boost/exception/all.hpp>
#include <string>

namespace sf
{
namespace afio
{
namespace error
{

typedef boost::error_info<struct tag_error_msg , std::string> errorMessage;

struct InvalidFileHandle : virtual public std::exception,
                           virtual public boost::exception
{};

struct UnableToStartAfio : virtual public std::exception,
                           virtual public boost::exception
{};

struct CancellationFailure : virtual public std::exception,
                             virtual public boost::exception
{};

}
}
}

#endif //SF_AFIO_AFIO_ERROR_HPP
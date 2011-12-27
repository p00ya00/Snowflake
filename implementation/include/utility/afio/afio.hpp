#ifndef SF_AFIO_AFIO_HPP
#define SF_AFIO_AFIO_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include <boost/function/function2.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
//#include <boost/assert.hpp>
#include <utility/afio/afio_error.hpp>

//number of worker threads used in the
//implementation, whether in a threadpool
//or as io_service-specific threads
#define AFIO_THREADPOOL_SIZE 10

//afio implementation config
#ifdef SF_AFIO_USES_OVERLAPPED_IO
#	undef SF_AFIO_USES_OVERLAPPED_IO
#endif

#ifdef SF_AFIO_USES_THREADPOOL
#	undef SF_AFIO_USES_THREADPOOL
#endif

#if _WIN32 && BOOST_ASIO_HAS_WINDOWS_RANDOM_ACCESS_HANDLE
#	define SF_AFIO_USES_OVERLAPPED_IO 1
#else
#	define SF_AFIO_USES_THREADPOOL 1
#endif

namespace sf
{
namespace afio
{

class AfioImp;
typedef boost::shared_ptr<AfioImp> AfioImpPtr;

enum FileOpenMode 
{
	READ_M = 1,
	WRITE_M,           // appended writing
	WRITE_TRUNC_M,     // truncate file
	READ_WRITE_M,      // appended writing
	READ_WRITE_TRUNC_M // truncate file
};

//defining a type which will be able to store boost::bind results
//as functor with the same signature as boost::asio read/write handlers
typedef boost::function2<void, 
                         const boost::system::error_code &, 
						 std::size_t>
        ReadHandler;

typedef boost::function2<void, 
                         const boost::system::error_code &, 
						 std::size_t>
        WriteHandler;

}
}

#endif
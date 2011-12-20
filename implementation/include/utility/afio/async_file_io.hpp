#ifndef SF_AFIO_ASYNC_FILE_IO_HPP
#define SF_AFIO_ASYNC_FILE_IO_HPP

#include <utility/afio/async_file_base.hpp>

using boost::asio::mutable_buffers_1;
using boost::asio::const_buffers_1;
using boost::asio::streambuf;

namespace sf
{
namespace afio
{

class AsyncFileIn : AsyncFileBase
{
public:
	AsyncFileIn(const std::string &path, const FileOpenMode mode);

	void asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
	               boost::uint64_t sizeToRead = 0);

	void asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
	               boost::uint64_t sizeToRead, boost::uint64_t offset);

protected:
	AsyncFileIn()
	{}
};

class AsyncFileOut : AsyncFileBase
{
public:
	AsyncFileOut(const std::string &path, const FileOpenMode mode);
	
	void asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                    boost::uint64_t sizeToWrite = 0);

	void asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                    boost::uint64_t sizeToWrite, boost::uint64_t offset);

protected:
	AsyncFileOut()
	{}
};

class AsyncFileInOut : virtual AsyncFileIn, virtual AsyncFileOut
{
public:
	AsyncFileInOut(const std::string &path, const FileOpenMode mode);
};

}
}

#endif
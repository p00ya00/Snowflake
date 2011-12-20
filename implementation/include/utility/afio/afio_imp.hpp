#ifndef SF_AFIO_AFIO_IMP_HPP
#define SF_AFIO_AFIO_IMP_HPP

#include <utility/afio/afio.hpp>

using boost::asio::mutable_buffers_1;
using boost::asio::const_buffers_1;
using boost::asio::streambuf;

namespace sf
{
namespace afio
{

class AfioImp : boost::noncopyable
{
public:
	AfioImp(const std::string &path, const FileOpenMode mode)
	: filePath(path), openMode(mode)
	{}

	std::string path() const
	{
		return filePath;
	}

	virtual void open(const std::string &path, const FileOpenMode mode) = 0;

	virtual bool isOpen() const = 0;

	virtual void cancel() = 0;

	virtual void close() = 0;

	virtual ~AfioImp()
	{}

	virtual void asyncRead( const mutable_buffers_1 &buff, 
                            ReadHandler handler,
							boost::uint64_t sizeToRead = 0
                          ) = 0;

	virtual void asyncRead( const mutable_buffers_1 &buff, 
                            ReadHandler handler,
							boost::uint64_t sizeToRead, 
							boost::uint64_t offset
                          ) = 0;
/*
	virtual void asyncRead( streambuf &buff, 
                            ReadHandler handler,
							boost::uint64_t sizeToRead, 
							boost::uint64_t offset
                          ) = 0;
*/
	virtual void asyncWrite( const const_buffers_1 &buff, 
                             ReadHandler handler,
                             boost::uint64_t sizeToWrite = 0
                           ) = 0;

	virtual void asyncWrite( const const_buffers_1 &buff, 
                             ReadHandler handler,
                             boost::uint64_t sizeToWrite, 
                             boost::uint64_t offset
                           ) = 0;
/*
	virtual void asyncWrite( streambuf &buff, 
                             ReadHandler handler,
                             boost::uint64_t sizeToRead, 
                             boost::uint64_t offset
                           ) = 0;
*/

protected:
	std::string filePath;

	FileOpenMode openMode;

	static const size_t poolSize = AFIO_THREADPOOL_SIZE;
};

}
}

#endif
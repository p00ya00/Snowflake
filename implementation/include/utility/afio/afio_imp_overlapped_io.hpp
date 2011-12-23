#ifndef SF_AFIO_AFIO_IMP_OVERLAPPED_IO_HPP
#define SF_AFIO_AFIO_IMP_OVERLAPPED_IO_HPP

#include <utility/afio/afio_imp.hpp>

#ifdef  SF_AFIO_USES_OVERLAPPED_IO

#include <fstream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/windows/random_access_handle.hpp>
#include <boost/asio/read_at.hpp>
#include <boost/asio/write_at.hpp>
#include <boost/thread.hpp>
using namespace boost::asio;

namespace sf
{
namespace afio
{

typedef boost::shared_ptr<io_service> IoServicePtr;

//Implementation of async file io using windows
//overlapped io with boost::asio
class AfioImpOverlappedIo : public AfioImp
{
public:
	AfioImpOverlappedIo(const std::string &path, const FileOpenMode mode);

	virtual void open(const std::string &path, const FileOpenMode mode);

	virtual bool isOpen() const;

	//Throws CancellationFailure
	virtual void cancel();

	virtual void close();

	virtual void asyncRead( const mutable_buffers_1 &buff, 
                            ReadHandler handler,
							boost::uint64_t sizeToRead = 0
                          );

	virtual void asyncRead( const mutable_buffers_1 &buff, 
                            ReadHandler handler,
							boost::uint64_t sizeToRead, 
							boost::uint64_t offset
                          );

	virtual void asyncWrite( const const_buffers_1 &buff, 
                             ReadHandler handler,
                             boost::uint64_t sizeToWrite = 0
                           );

	virtual void asyncWrite( const const_buffers_1 &buff, 
                             ReadHandler handler,
                             boost::uint64_t sizeToWrite, 
                             boost::uint64_t offset
                           );

	std::fstream  &getFileStream() const;

	virtual ~AfioImpOverlappedIo();

private:
	//Native windows handle for the file
	HANDLE fileHandle;

	//boost::asio wrappers around the windows HANDLE
	//to add async file io operations
	windows::random_access_handle randomAccessHandle;
	
	static io_service ioService;

	static io_service::work work;

	//io_service worker threads
	static boost::thread_group workerThreads;

	//workaround for static constructors and destructors
	//in C++ to start and stop the io_service and worker threads
	struct IoServiceInitializer;
	static IoServiceInitializer iosInitializer;
};

}
}

#endif //SF_AFIO_USES_OVERLAPPED_IO
#endif //SF_AFIO_AFIO_IMP_OVERLAPPED_IO_HPP
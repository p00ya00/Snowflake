#include <utility/afio/afio_imp_overlapped_io.hpp>
using namespace sf::afio;

#include <iostream>
#include <io.h>

namespace asio_error = boost::asio::error;

struct AfioImpOverlappedIo::IoServiceInitializer
{
	IoServiceInitializer()
	{
		for(size_t i = 0; i < poolSize; ++i)
			workerThreads.create_thread(boost::bind(&io_service::run, &ioService));
	}

	~IoServiceInitializer()
	{
		ioService.stop();
		workerThreads.join_all();
	}
};

io_service AfioImpOverlappedIo::ioService(poolSize);

io_service::work AfioImpOverlappedIo::work(ioService);

boost::thread_group AfioImpOverlappedIo::workerThreads;

AfioImpOverlappedIo::IoServiceInitializer AfioImpOverlappedIo::iosInitializer;

AfioImpOverlappedIo::AfioImpOverlappedIo(const std::string &path, const FileOpenMode mode)
: AfioImp(path, mode), randomAccessHandle(ioService)
{
	open(path, mode);
}

void AfioImpOverlappedIo::open(const std::string &path, const FileOpenMode mode)
{
	if(!isOpen())
	{
		DWORD desiredAccess, creationMode;

		switch(mode)
		{
			case READ_M:
				desiredAccess = GENERIC_READ;
				creationMode  = OPEN_EXISTING;
				break;
			case WRITE_M:
				desiredAccess = GENERIC_WRITE;
				creationMode  = OPEN_ALWAYS;
				break;
			case WRITE_TRUNC_M:
				desiredAccess = GENERIC_WRITE;
				creationMode  = TRUNCATE_EXISTING;
				break;
			case READ_WRITE_M:
				desiredAccess = GENERIC_READ | GENERIC_WRITE;
				creationMode  = OPEN_ALWAYS;
				break;
			case READ_WRITE_TRUNC_M:
				desiredAccess = GENERIC_READ | GENERIC_WRITE;
				creationMode  = CREATE_ALWAYS;
				break;
		}
	
		fileHandle = CreateFile ( path.c_str(), 
								  desiredAccess, 
								  FILE_SHARE_READ | FILE_SHARE_WRITE, 
								  NULL,
								  creationMode, 
								  FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,
								  NULL );

		randomAccessHandle.assign(fileHandle);
	}
}

bool AfioImpOverlappedIo::isOpen() const
{
	return randomAccessHandle.is_open();
}

void AfioImpOverlappedIo::cancel()
{
	try
	{
		randomAccessHandle.cancel();
	}
	catch(boost::system::system_error &error)
	{
		if(error.code() == asio_error::bad_descriptor)
		{
			//The random access handle does not point
			//to a valid open file HANDLE
			throw error::CancellationFailure()
			      << error::errorMessage("Invalid file handle");
		}
		else if(error.code() == asio_error::operation_not_supported)
		{
			//Async operations have been started from more than
			//one thread on the same random access handle
			//Incomplete operations cannot be canceled safely
			throw error::CancellationFailure()
			      << error::errorMessage("Multiple threads using file handle");
		}
		else
		{
			throw error::CancellationFailure() << error::errorMessage(error.what());
		}
	}
}

void AfioImpOverlappedIo::close()
{
	try
	{
		randomAccessHandle.close();
	}
	catch(boost::system::system_error &error)
	{
		//error in closing the handle
		std::cerr << "Error while closing the handle. " << error.what() << std::endl;
	}
}

inline void AfioImpOverlappedIo::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                                           boost::uint64_t sizeToRead)
{
	asyncRead(buff, handler, sizeToRead, 0);
}

void AfioImpOverlappedIo::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                                    boost::uint64_t sizeToRead, boost::uint64_t offset)
{
	using namespace boost::system;

	if(fileHandle == INVALID_HANDLE_VALUE)
	{
		error_code err(errc::no_such_file_or_directory, system_category());
		handler(err, 0);
	}
	else if(sizeToRead == 0)
	{
		boost::asio::async_read_at(randomAccessHandle, offset, buff, 
                                   transfer_all(), handler);
	}
	else //sizeToRead > 0
	{
		boost::asio::async_read_at(randomAccessHandle, offset, buff, 
                                   transfer_at_least(static_cast<std::size_t>(sizeToRead)),
		                           handler);
	}
}

inline void AfioImpOverlappedIo::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                                            boost::uint64_t sizeToWrite)
{
	asyncWrite(buff, handler, sizeToWrite, GetFileSize(fileHandle, NULL));
}

void AfioImpOverlappedIo::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                                     boost::uint64_t sizeToWrite, boost::uint64_t offset)
{
	using namespace boost::system;

	if(fileHandle == INVALID_HANDLE_VALUE)
	{
		error_code err(errc::no_such_file_or_directory, system_category());
		handler(err, 0);
	}
	else if(sizeToWrite == 0)
	{
		boost::asio::async_write_at(randomAccessHandle, offset, buff, 
		                            transfer_all(), handler);
	}
	else //sizeToRead > 0
	{
		boost::asio::async_write_at(randomAccessHandle, offset, buff, 
                                    transfer_at_least(static_cast<std::size_t>(sizeToWrite)),
		                            handler);
	}
}

AfioImpOverlappedIo::~AfioImpOverlappedIo()
{
	close();
}
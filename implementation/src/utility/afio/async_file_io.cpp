#include <utility/afio/async_file_io.hpp>
#include <utility/afio/afio_imp.hpp>
using namespace sf::afio;

AsyncFileIn::AsyncFileIn(const std::string &path, const FileOpenMode mode)
: AsyncFileBase(path, mode)
{}

void AsyncFileIn::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                            boost::uint64_t sizeToRead)
{
	pImp->asyncRead(buff, handler, sizeToRead);
}

void AsyncFileIn::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                            boost::uint64_t sizeToRead, boost::uint64_t offset)
{
	pImp->asyncRead(buff, handler, sizeToRead, offset);
}

///////////////////////////////////////////////////////////////////////////////

AsyncFileOut::AsyncFileOut(const std::string &path, const FileOpenMode mode)
: AsyncFileBase(path, mode)
{}

void AsyncFileOut::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                              boost::uint64_t sizeToWrite)
{
	pImp->asyncWrite(buff, handler, sizeToWrite);
}

void AsyncFileOut::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                              boost::uint64_t sizeToWrite, boost::uint64_t offset)
{
	pImp->asyncWrite(buff, handler, sizeToWrite, offset);
}

///////////////////////////////////////////////////////////////////////////////

AsyncFileInOut::AsyncFileInOut(const std::string &path, const FileOpenMode mode)
: AsyncFileBase(path, mode),
  AsyncFileIn(path, mode),
  AsyncFileOut(path, mode)
{}
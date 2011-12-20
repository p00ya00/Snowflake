#include <utility/afio/async_file_io.hpp>
using namespace sf::afio;

AsyncFileIn::AsyncFileIn(const std::string &path, const FileOpenMode mode)
: AsyncFileBase(path, mode)
{}

void AsyncFileIn::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                            boost::uint64_t sizeToRead = 0)
{

}

void AsyncFileIn::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                            boost::uint64_t sizeToRead, boost::uint64_t offset)
{

}

AsyncFileOut::AsyncFileOut(const std::string &path, const FileOpenMode mode)
: AsyncFileBase(path, mode)
{}

void AsyncFileOut::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                              boost::uint64_t sizeToWrite = 0)
{

}

void AsyncFileOut::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                              boost::uint64_t sizeToWrite, boost::uint64_t offset)
{

}

AsyncFileInOut::AsyncFileInOut(const std::string &path, const FileOpenMode mode)
: AsyncFileIn(path, mode)
{}
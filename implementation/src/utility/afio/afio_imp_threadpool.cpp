#include <utility/afio/afio_imp_threadpool.hpp>
using namespace sf::afio;

#ifdef SF_AFIO_USES_THREADPOOL

AfioImpThreadpool::AfioImpThreadpool(const std::string &path, const FileOpenMode mode)
: AfioImp(path, mode)
{}

void AfioImpThreadpool::open(const std::string &path, const FileOpenMode mode)
{}

bool AfioImpThreadpool::isOpen() const
{}

void AfioImpThreadpool::cancel()
{}

void AfioImpThreadpool::close()
{}

inline void AfioImpThreadpool::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                                         boost::uint64_t sizeToRead)
{}

void AfioImpThreadpool::asyncRead(const mutable_buffers_1 &buff, ReadHandler handler,
                                  boost::uint64_t sizeToRead, boost::uint64_t offset)
{}

inline void AfioImpThreadpool::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                                          boost::uint64_t sizeToWrite)
{}

void AfioImpThreadpool::asyncWrite(const const_buffers_1 &buff, ReadHandler handler,
                                   boost::uint64_t sizeToWrite, boost::uint64_t offset)
{}

AfioImpThreadpool::~AfioImpThreadpool()
{}

#endif //SF_AFIO_USES_THREADPOOL
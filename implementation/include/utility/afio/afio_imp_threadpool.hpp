#ifndef SF_AFIO_AFIO_IMP_THREADPOOL_HPP
#define SF_AFIO_AFIO_IMP_THREADPOOL_HPP

#include <utility/afio/afio_imp.hpp>

#ifdef SF_AFIO_USES_THREADPOOL

namespace sf
{
namespace afio
{

class AfioImpThreadpool : AfioImp
{
	public:
	AfioImpThreadpool(const std::string &path, const FileOpenMode mode);

	virtual void open(const std::string &path, const FileOpenMode mode);

	virtual bool isOpen() const;

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

	virtual ~AfioImpThreadpool();
};

}
}

#endif //SF_AFIO_USES_THREADPOOL
#endif //SF_AFIO_AFIO_IMP_THREADPOOL_HPP
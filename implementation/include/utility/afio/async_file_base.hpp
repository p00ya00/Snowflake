#ifndef SF_AFIO_ASYNC_FILE_BASE_HPP
#define SF_AFIO_ASYNC_FILE_BASE_HPP

#include <utility/afio/afio.hpp>

namespace sf
{
namespace afio
{

class AsyncFileBase : private boost::noncopyable
{
public:
	AsyncFileBase(const std::string &path, const FileOpenMode mode);

	std::string path() const;

	void open(const std::string &path, const FileOpenMode mode);

	bool isOpen() const;

	void cancel();

	void close();
	
	virtual ~AsyncFileBase()
	{}

protected:
	AfioImpPtr pImp;

	AfioImp *getAfioImp(const std::string &path, const FileOpenMode mode);
};

}
}

#endif
#ifndef SF_AFIO_ASYNC_FILE_BASE_HPP
#define SF_AFIO_ASYNC_FILE_BASE_HPP

#include <boost/scoped_ptr.hpp>
#include <utility/afio/afio.hpp>

namespace sf
{
namespace afio
{

//forward declaration for the implementor class
class AfioImp;

class AsyncFileBase : boost::noncopyable
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
	typedef boost::scoped_ptr<AfioImp> AfioImpPtr;

	AfioImpPtr pImp;

	virtual AfioImpPtr getAfioImp(const std::string &path, const FileOpenMode mode) = 0;
};

}
}

#endif
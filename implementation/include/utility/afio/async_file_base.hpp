#ifndef SF_AFIO_ASYNC_FILE_BASE_HPP
#define SF_AFIO_ASYNC_FILE_BASE_HPP

#include <utility/afio/afio.hpp>

namespace sf
{
namespace afio
{

//Abstraction of async file io which performs the
//operations by delegation to the back-end implementation
//
//The base defines functions common to in, out
//and in/out files
class AsyncFileBase : private boost::noncopyable
{
public:
	//Throws UnableToStartAfio if it cannot get an implementation
	AsyncFileBase(const std::string &path, const FileOpenMode mode);

	std::string path() const;

	void open(const std::string &path, const FileOpenMode mode);

	bool isOpen() const;

	void cancel();

	void close();
	
	virtual ~AsyncFileBase()
	{}

protected:
	//pointer to afio implementation
	AfioImpPtr pImp;

	AfioImp *getAfioImp(const std::string &path, const FileOpenMode mode);
};

}
}

#endif
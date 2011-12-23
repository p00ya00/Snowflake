#include <utility/afio/async_file_base.hpp>
#include <utility/afio/afio_imp.hpp>
#include <utility/afio/afio_imp_factory.hpp>
using namespace sf::afio;

AsyncFileBase::AsyncFileBase(const std::string &path, const FileOpenMode mode)
: pImp(getAfioImp(path, mode))
{
	if(pImp == NULL)
	{
		//Could not get an implementation
		throw error::UnableToStartAfio()
		      << error::errorMessage("Cannot get an implementation");
	}
}

inline std::string AsyncFileBase::path() const
{
	return pImp->path();
}

inline void AsyncFileBase::open(const std::string &path, const FileOpenMode mode)
{
	pImp->open(path, mode);
}

inline bool AsyncFileBase::isOpen() const
{
	return pImp->isOpen();
}

inline void AsyncFileBase::cancel()
{
	pImp->cancel();
}

inline void AsyncFileBase::close()
{
	pImp->close();
}

AfioImp *AsyncFileBase::getAfioImp(const std::string &path, const FileOpenMode mode)
{
	//Call a factory function to get an implementation
	return createAfioImp(path, mode);
}
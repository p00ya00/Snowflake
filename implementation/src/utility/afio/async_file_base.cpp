#include <utility/afio/async_file_base.hpp>
#include <utility/afio/afio_imp.hpp>
using namespace sf::afio;

AsyncFileBase::AsyncFileBase(const std::string &path, const FileOpenMode mode)
: pImp(getAfioImp(path, mode))
{
	BOOST_ASSERT(pImp != NULL);
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
#include <utility/afio/afio_imp_factory.hpp>
using namespace sf::afio;

AfioImp *sf::afio::createAfioImp(const std::string &path, const FileOpenMode mode)
{
#	if defined(SF_AFIO_USES_OVERLAPPED_IO)
		return new AfioImpOverlappedIo(path, mode);
#	elif defined(SF_AFIO_USES_THREADPOOL)
		return new AfioImpThreadpool(path, mode);
#	endif
	
	return NULL;
}
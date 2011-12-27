#include <utility/afio/afio_service.hpp>
#include <utility/afio/afio_imp_overlapped_io.hpp>
#include <utility/afio/afio_imp_threadpool.hpp>

void sf::afio::join()
{
	using namespace sf::afio;

#	if defined(SF_AFIO_USES_OVERLAPPED_IO)
		AfioImpOverlappedIo::join();
#	elif defined(SF_AFIO_USES_THREADPOOL)
		AfioImpThreadpool::join();
#	endif
}

void sf::afio::stop()
{
	using namespace sf::afio;

#	if defined(SF_AFIO_USES_OVERLAPPED_IO)
		AfioImpOverlappedIo::stop();
#	elif defined(SF_AFIO_USES_THREADPOOL)
		AfioImpThreadpool::stop();
#	endif
}
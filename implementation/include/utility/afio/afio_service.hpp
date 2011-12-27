#ifndef SF_AFIO_AFIO_SERVICE_HPP
#define SF_AFIO_AFIO_SERVICE_HPP

namespace sf
{
namespace afio
{

//Join the afio service, which will keep the
//thread calling it busy untill stop() is called
void join();

//Stop the afio service 
void stop();

}
}

#endif //SF_AFIO_AFIO_SERVICE_HPP
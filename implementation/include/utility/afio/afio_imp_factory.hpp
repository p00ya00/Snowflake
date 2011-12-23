#ifndef SF_AFIO_AFIO_IMP_FACTORY_HPP
#define SF_AFIO_AFIO_IMP_FACTORY_HPP

#include <utility/afio/afio_imp_overlapped_io.hpp>
#include <utility/afio/afio_imp_threadpool.hpp>

namespace sf
{
namespace afio
{

//create an implementation based on the defined macros
AfioImp *createAfioImp(const std::string &path, const FileOpenMode mode);

}
}

#endif
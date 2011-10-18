#ifndef SF_SEDA_EVENT_HPP
#define SF_SEDA_EVENT_HPP

#include <boost/utility.hpp>

namespace sf
{
namespace seda
{

class Event : boost::noncopyable
{
public:
	explicit Event(unsigned short t)
	:type(t)
	{}

	unsigned short getType() const
	{
		return type;
	}

	virtual ~Event()
	{}
	
private:
	unsigned short type;
};

}
}

#endif
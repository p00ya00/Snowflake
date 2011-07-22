#ifndef SF_SEDA_EVENT_HPP
#define SF_SEDA_EVENT_HPP

namespace sf
{
namespace seda
{

class Event
{
public:
	Event(unsigned short t)
	:type(t)
	{}

	unsigned short getType() const
	{
		return type;
	}
	
private:
	unsigned short type;
};

}
}

#endif
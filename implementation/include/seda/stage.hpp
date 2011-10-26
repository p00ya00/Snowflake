#ifndef SF_SEDA_STAGE_HPP
#define SF_SEDA_STAGE_HPP

#include <utility/threadpool.hpp>
#include <seda/event.hpp>
#include <seda/stage_table.hpp>
#include <seda/exception.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace sf
{
namespace seda
{

typedef boost::shared_ptr<sf::utility::Threadpool<>> ThreadPoolPtr;
typedef boost::shared_ptr<sf::seda::Event> EventPtr;

class Stage : boost::noncopyable
{
public:
	explicit Stage(size_t psize)
	:poolSize(psize)
	{}

	void setStageTable(StageTable st)
	{
		stageTable = st;
	}

	void start()
	{
		threadpool.reset(new sf::utility::Threadpool<>(poolSize));
	}

	void stop()
	{
		threadpool->cancelAll();
		threadpool->clearJobs();
	}

	void enqueue(EventPtr e)
	{
		threadpool->schedule(boost::bind(&Stage::eventHandler, this, e));
	}

	virtual ~Stage()
	{}

protected:
	virtual void eventHandler(EventPtr e) = 0;

	void enqueue(unsigned short s, EventPtr e)
	{
		StagePtr stage = stageTable.find(s);
		if(stage == NULL)
			throw InvalidStageException()
				 << errorMessage("There is no stage " + 
					boost::lexical_cast<std::string>(s) + "in the stage table");
		stage->enqueue(e);
	}

private:
	ThreadPoolPtr threadpool;

	size_t poolSize;

	StageTable stageTable;
};

}
}

#endif
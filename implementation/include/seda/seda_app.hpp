#ifndef SF_SEDA_APP_HPP
#define SF_SEDA_APP_HPP

#include <seda/stage.hpp>

namespace sf
{
namespace seda
{

class SedaApp : boost::noncopyable
{
public:
	void run()
	{
		initialize();

		std::vector<StagePtr> stages(stageTable.getAllStages());

		for(std::vector<StagePtr>::iterator itr = stages.begin();
			itr != stages.end();
			++itr)
		{
			(*itr)->setStageTable(stageTable);
			(*itr)->start();
		}

		start();
	}

	void stop()
	{
		std::vector<StagePtr> stages(stageTable.getAllStages());

		for(std::vector<StagePtr>::iterator itr = stages.begin();
			itr != stages.end();
			++itr)
		{
			(*itr)->stop();
		}
	}

	virtual ~SedaApp()
	{}

protected:
	virtual void initialize() = 0;
	
	virtual void start() = 0;

	void addStage(unsigned short st, StagePtr sp)
	{
		stageTable.add(st, sp);
	}

	void enqueue(unsigned short s, EventPtr e)
	{
		StagePtr stage = stageTable.find(s);
		if(stage != NULL)
			stage->enqueue(e);
	}

private:
	sf::seda::StageTable stageTable;
};

}
}

#endif
#ifndef SF_SEDA_STAGE_TABLE_HPP
#define SF_SEDA_STAGE_TABLE_HPP

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace sf
{
namespace seda
{

class Stage;

typedef boost::shared_ptr<Stage> StagePtr;

class StageTable
{
public:
	typedef std::map <unsigned short, StagePtr> LookUpTable;
	typedef std::pair<unsigned short, StagePtr> TableEntry;

	void add(unsigned short st, StagePtr sp)
	{
		table.insert(TableEntry(st, sp));
	}

	StagePtr find(unsigned short st)
	{
		StagePtr sp;
		LookUpTable::iterator res = table.find(st);
		if(res != table.end())
			sp = res->second;

		return sp;
	}

	std::vector<StagePtr> getAllStages()
	{
		std::vector<StagePtr> stages;
		
		for(LookUpTable::iterator itr = table.begin();
			itr != table.end();
			++itr)
		{
			stages.push_back(itr->second);
		}

		return stages;
	}

private:
	LookUpTable table;
};

}
}

#endif SF_SEDA_STAGE_TABLE_HPP
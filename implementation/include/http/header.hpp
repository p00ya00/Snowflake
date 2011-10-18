#ifndef SF_HTTP_HEADER_HPP
#define SF_HTTP_HEADER_HPP

#include <string>
#include <vector>
#include <http/http.hpp>

namespace sf
{
namespace http
{

class Header
{
private:
	struct Value
	{
		Value(const std::string &val, const std::string &qlty)
		:v(val), q(qlty)
		{}

		std::string v;
		std::string q;
	};

public:
	typedef boost::shared_ptr<Header> Ptr;

	typedef std::vector<Value> Values;

	typedef Values::const_iterator ValueConstItr;

	//typedef Values::iterator ValueItr;

	Header(const std::string &n, const std::string &v = "", const std::string &q = "")
	{
		setName(n);
		if(!v.empty())
			values.push_back(Value(v, q));
	}

	inline void setName(const std::string &n)
	{
		name = n;
		str_algo::to_lower(name);
	}

	inline std::string getName() const
	{
		return name;
	}

	inline ValueConstItr valuesBegin() const
	{
		return values.begin();
	}

	inline ValueConstItr valuesEnd() const
	{
		return values.end();
	}

	void addValue(const std::string &v, const std::string &q = "");

	void eraseValue(const std::string &v);

	void clearAllValues()
	{
		values.clear();
	}

	void changeValue(const std::string &curVal, const std::string &newVal, 
					const std::string &q = "");

	std::string toString() const;

	inline size_t valuesSize() const
	{
		return values.size();
	}

	ValueConstItr getValue(const std::string &v);

	// 'input' and 'attribute' MUST both be left and right trimmed,
	// otherwise the results may come out wrong. If necessary
	// use str_algo::trim before extracting parameters.
	static std::string extractParam(const std::string &input, const std::string &attribute);

private:
	std::string name;

	Values values;
};

}
}

#endif
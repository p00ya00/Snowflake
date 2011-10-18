#include <http/header.hpp>
using namespace sf::http;

void Header::addValue(const std::string &v, const std::string &q)
{
	for(Values::const_iterator val = values.begin();
		val != values.end();
		++val)
	{
		if(str_algo::iequals(v, val->v))
			return;
	}
		
	values.push_back(Value(v, q));
}

void Header::eraseValue(const std::string &v)
{
	for(Values::iterator val = values.begin();
		val != values.end();
		++val)
	{
		if(str_algo::iequals(v, val->v))
		{
			values.erase(val);
			return;
		}
	}
}

void Header::changeValue(const std::string &curVal, const std::string &newVal, 
						const std::string &q)
{
	for(Values::iterator val = values.begin();
		val != values.end();
		++val)
	{
		if(str_algo::iequals(curVal, val->v))
		{
			val->v = newVal;
			val->q = q;
			return;
		}
	}
}

std::string Header::toString() const
{
	std::string res(getName());
		
	res += ": ";
	for(Values::const_iterator val = values.begin();
		val != values.end();
		++val)
	{
		res += val->v;
		if(!val->q.empty())
		{
			res += ';';
			res += val->q;
		}
		if(val + 1 != values.end())
			res += ", ";
	}

	return res;
}

std::string Header::extractParam(const std::string &input, const std::string &attribute)
{
	if(str_algo::istarts_with(input, attribute))
	{
		std::string::size_type res = input.find('=');
		if(res != std::string::npos)
		{
			std::string val(input.begin() + res + 1, input.end());
			str_algo::trim_left(val);
			return val;
		}
	}

	return "";
}

Header::ValueConstItr Header::getValue(const std::string &v)
{
	for(Values::const_iterator val = values.begin();
		val != values.end();
		++val)
	{
		if(str_algo::iequals(v, val->v))
			return val;
	}

	return valuesEnd();
}
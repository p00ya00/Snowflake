#include <http/message.hpp>
using namespace sf::http;

void Message::addHeader(const Header::Ptr header)
{
	Header::Ptr res( getHeader(header->getName()) );

	if(res == NULL)
		headers.push_back(header);
}

Header::Ptr Message::getHeader(const std::string &name) const
{
	for(Headers::const_iterator itr = headers.begin();
		itr != headers.end();
		++itr)
	{
		if( str_algo::iequals((*itr)->getName(), name) )
			return *itr;
	}

	return Header::Ptr(); //return a null pointer
}

void Message::eraseHeader(const std::string &name)
{
	for(Headers::iterator itr = headers.begin();
		itr != headers.end();
		++itr)
	{
		if( (*itr)->getName() == name )
		{
			headers.erase(itr);
			return;
		}
	}
}

std::string Message::toString() const
{
	std::string res = getStartLine();

	for(HeaderConstItr header = headersBegin();
		header != headersEnd();
		++header)
	{
		res += (*header)->toString();
		res += CRLF;
	}

	res += CRLF;

	return res;
}
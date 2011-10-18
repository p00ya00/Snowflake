#ifndef SF_HTTP_PARSER_HPP
#define SF_HTTP_PARSER_HPP

#include <http/request.hpp>
#include <boost/asio.hpp>
#include <istream>
#include <boost/tokenizer.hpp>

#ifndef NDEBUG //if in debug mode
namespace parser_test_suite
{
	struct test_isRequestLine;
	struct test_parseRequestLine;
	struct test_getLine;
}
#endif

namespace sf
{
namespace http
{

class Parser
{
public:
	//parses a complete HTTP request read into a buffer,
	//including a start line and a set of headers
	void parseRequest(Request &req, boost::asio::streambuf &src);

	//parses only one header
	Header::Ptr parseHeader(const std::string &line);

	unsigned long setMaxHeaderLength(unsigned long max)
	{
		unsigned long old = maxHeaderLength;
		maxHeaderLength = max;
		return old;
	}

	unsigned long getMaxHeaderLength()
	{
		return maxHeaderLength;
	}

	unsigned long setMaxHeaders(unsigned long max)
	{
		unsigned long old = maxHeaders;
		maxHeaders = max;
		return old;
	}

	unsigned long getMaxHeaders()
	{
		return maxHeaders;
	}
	
	unsigned long setMaxUriLength(unsigned long max)
	{
		unsigned long old = maxUriLength;
		maxUriLength = max;
		return old;
	}

	unsigned long getMaxUriLength()
	{
		return maxUriLength;
	}
	
	unsigned long setMaxTotalHeadSize(unsigned long max)
	{
		unsigned long old = maxTotalHeadSize;
		maxTotalHeadSize = max;
		return old;
	}

	unsigned long getMaxTotalHeadSize()
	{
		return maxTotalHeadSize;
	}

private:

#ifndef NDEBUG //if in debug mode
	friend parser_test_suite::test_isRequestLine;
	friend parser_test_suite::test_parseRequestLine;
	friend parser_test_suite::test_getLine;
#endif

	typedef boost::tokenizer< boost::char_separator<char> > CharSeparatorTokenizer;	
	typedef boost::tokenizer< boost::escaped_list_separator<char> > EscapedListTokenizer;

	bool getLine(std::istream &input, std::string &line, std::size_t maxLength, 
		char delim = LF);
	
	bool isRequestLine(const std::string &line) const;

	bool isWSpace(char c) const;

	bool isMethodValid(const std::string &method) const;

	void parseRequestLine(Request &req, const std::string &line);

	bool valueIsDate(const std::string &header) const;

private:
	//max size of each header: 4KB
	static unsigned long maxHeaderLength;
	
	//max number of headers in one message
	static unsigned long maxHeaders;

	//max URI length of a request message: 4KB
	static unsigned long maxUriLength;
	
	//max head size of a message: 32KB
	static unsigned long maxTotalHeadSize;
};

}
}

#endif
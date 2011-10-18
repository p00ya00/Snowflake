#include <http/exception.hpp>
#include <http/utility.hpp>
#include <http/parser.hpp>
using namespace sf::http;


unsigned long Parser::maxHeaderLength	= 4 * 1024;
unsigned long Parser::maxHeaders		= 100;
unsigned long Parser::maxTotalHeadSize	= 32 * 1024;
unsigned long Parser::maxUriLength		= 4 * 1024;


void Parser::parseRequest(Request &req, boost::asio::streambuf &src)
{
	std::istream input(&src);
	std::string curLine, nextLine;
	std::size_t totalHeadSize = 0;

	bool getLineResult = getLine(input, curLine, std::max(maxUriLength, maxHeaderLength));
	//removing \r if the line ends with \r\n
	if(getLineResult && curLine[curLine.size() - 1] == CR)
		curLine.pop_back();
	//check whether the buffer contains at least one line
	if(!getLineResult || curLine.empty())
		//recieved an empty message!
		throw BadRequestException();
	
	if(isRequestLine(curLine))
	{
		if(curLine.size() >= maxUriLength)
			throw UriTooLargeException();

		totalHeadSize += curLine.size();
		if(totalHeadSize >= maxTotalHeadSize)
			throw BadRequestException();
		
		//extract method, uri and version
		parseRequestLine(req, curLine);

		//try to get another line
		curLine.clear();
		if(getLine(input, curLine, maxHeaderLength) && curLine[curLine.size() - 1] == CR)
			curLine.pop_back();
	}

	if(curLine.size() >= maxHeaderLength)
		throw BadRequestException();
	//parse headers
	Header::Ptr hp;
	while(!curLine.empty())
	{
		getLineResult = getLine(input, nextLine, maxHeaderLength);
		if(getLineResult && nextLine[nextLine.size() - 1] == CR)
			nextLine.pop_back();
		while( getLineResult && !nextLine.empty() && isWSpace(nextLine[0]))
		{
			curLine.append(nextLine);
			if(curLine.size() >= maxHeaderLength)
				throw BadRequestException();
			nextLine.clear();
			getLineResult = getLine(input, nextLine, maxHeaderLength);
			if(getLineResult && nextLine[nextLine.size() - 1] == CR)
				nextLine.pop_back();
		}

		totalHeadSize += curLine.size();
		if(totalHeadSize >= maxTotalHeadSize)
			throw BadRequestException();

		//parse one header
		hp = parseHeader(curLine);
		if(hp)
		{
			if(req.headersSize() >= maxHeaders)
				throw BadRequestException();
			req.addHeader(hp);
		}

		curLine = nextLine;
	}
}

//reads one line from the input up to delimiter or when maxLength chars
//are read before the reaching the delimiter.
//returns true if it can read at least one character
bool Parser::getLine(std::istream &input, std::string &line, std::size_t maxLength,
	char delim)
{
	char c;
	std::size_t readCount = 0;
	
	line.clear();
	while(input.get(c))
	{
		++readCount;
		if(c != delim)
		{
			line.push_back(c);
			if(readCount >= maxLength)
				return true;
		}
		else
			break;
	}
	
	return (readCount ? true : false);
}

bool Parser::isRequestLine(const std::string &line) const
{
	std::string::const_iterator cur = line.begin();
	
	while(cur != line.end() && !isWSpace(*cur) && *cur != ':')
		++cur;
	while(cur != line.end() && isWSpace(*cur))
		++cur;
	if(cur != line.end() && *cur != ':')
		return true;
	
	return false;
}

inline bool Parser::isWSpace(char c) const
{
	if(c == ' ' || c == '\t')
		return true;
	return false;
}

inline bool Parser::isMethodValid(const std::string &method) const
{
	if( method != "GET" && method != "HEAD" && method != "POST" 
		&& method != "TRACE" && method != "DELETE" && method != "OPTIONS" 
		&& method != "PUT" && method != "CONNECT")
	{
		return false;
	}

	return true;
}

void Parser::parseRequestLine(Request &req, const std::string &line)
{
	boost::char_separator<char> sep(" \t");
	CharSeparatorTokenizer tokens(line, sep);
	CharSeparatorTokenizer::iterator tok = tokens.begin();

	//if no method present -> bad request
	if(tok == tokens.end())
		throw BadRequestException();
	
	//if invalid method -> bad request
	std::string method(*tok);
	str_algo::to_upper(method);
	if(!isMethodValid(method))
		throw BadRequestException();

	if(method == "GET")
		req.setMethod(Request::GET_M);
	else if(method == "HEAD")
		req.setMethod(Request::HEAD_M);
	else if(method == "POST")
		req.setMethod(Request::POST_M);
	else if(method == "DELETE")
		req.setMethod(Request::DELETE_M);
	else if(method == "OPTIONS")
		req.setMethod(Request::OPTIONS_M);
	else if(method == "CONNECT")
		req.setMethod(Request::CONNECT_M);
	else if(method == "PUT")
		req.setMethod(Request::PUT_M);
	else if(method == "TRACE")
		req.setMethod(Request::TRACE_M);

	//if no URI present -> bad request
	if(++tok == tokens.end())
		throw BadRequestException();

	std::string uri(*tok);
	percentDecode(uri);
	req.setUri(uri);

	//if no protocol version present -> bad request
	if(++tok == tokens.end())
		throw BadRequestException();

	std::string version((*tok));
	if(!str_algo::istarts_with(version, "HTTP/"))
		throw BadRequestException();
	std::size_t pos = version.find('/');
	if(pos == std::string::npos)
		throw BadRequestException();
	else
		req.setVersion(version.substr(pos + 1));
}

Header::Ptr Parser::parseHeader(const std::string &line)
{
	boost::escaped_list_separator<char> sep("\\", ",:", "\"");
	EscapedListTokenizer tokens(line, sep);
	EscapedListTokenizer::iterator tok = tokens.begin();
	Header::Ptr hp;
	
	if(tok != tokens.end())
	{
		hp.reset(new Header(str_algo::trim_copy(*tok)));
		size_t pos;

		//headers with a date field have only one value
		//and the comma present in their value is not
		//a value list separator
		if(valueIsDate(hp->getName()))
		{
			pos = line.find(':');
			if(pos != std::string::npos)
				hp->addValue(str_algo::trim_copy(line.substr(pos + 1)));
		}
		//for headers without a date field, values are separated
		//with comma including a possible quality parameter
		else
			while(++tok != tokens.end())
			{
				pos = tok->find(';');
				if(pos == std::string::npos)
					hp->addValue(str_algo::trim_copy(*tok));
				else
					hp->addValue(str_algo::trim_copy(tok->substr(0, pos)), 
								str_algo::trim_copy(tok->substr(pos + 1)));
			}
	}

	return hp;
}

bool Parser::valueIsDate(const std::string &header) const
{
	return (header == "date" || header == "expires" 
			|| header == "if-modified-since" 
			|| header == "if-unmodified-since" 
			|| header == "if-range" || header == "last-modified" 
			|| header == "retry-after");
}
#ifndef SF_HTTP_REQUEST_HPP
#define SF_HTTP_REQUEST_HPP

#include <http/message.hpp>

namespace sf
{
namespace http
{

class Request: public Message
{
public:
	typedef boost::shared_ptr<Request> Ptr;

	enum METHOD_T {GET_M = 1, HEAD_M, POST_M, TRACE_M, DELETE_M, 
		OPTIONS_M, PUT_M, CONNECT_M};

	Request(const METHOD_T m = GET_M, const std::string &u = "", 
			const std::string &v = "", const std::string &b = "")
	:Message(v, b), method(m), uri(u)
	{}

	inline METHOD_T getMethod() const
	{
		return method;
	}

	inline void setMethod(METHOD_T m)
	{
		method = m;
	}

	inline std::string getUri() const
	{
		return uri;
	}

	inline void setUri(const std::string &u)
	{
		uri = u;
	}

	std::string getMethodStr() const
	{
		switch(method)
		{
		case CONNECT_M:
			return "CONNECT";
		case DELETE_M:
			return "DELETE";
		case GET_M:
			return "GET";
		case HEAD_M:
			return "HEAD";
		case OPTIONS_M:
			return "OPTIONS";
		case POST_M:
			return "POST";
		case PUT_M:
			return "PUT";
		case TRACE_M:
			return "TRACE";
		default:
			return "";
		};
	}

	virtual std::string getStartLine() const
	{
		return getMethodStr() + " " + getUri() + " HTTP/" + getVersion() + CRLF;
	}

private:
	METHOD_T method;

	std::string uri;
};

}
}

#endif
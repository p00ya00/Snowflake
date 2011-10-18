#ifndef SF_HTTP_RESPONSE_HPP
#define SF_HTTP_RESPONSE_HPP

#include <http/message.hpp>
#include <http/reason_phrase.hpp>
#include <boost/lexical_cast.hpp>

namespace sf
{
namespace http
{

class Response: public Message
{
public:
	typedef boost::shared_ptr<Response> Ptr;

	Response(const StatusCode sc = 500, const std::string &v = "", 
			const std::string &b = "")
	:Message(v, b)
	{
		setStatusCode(sc);
	}

	void setStatusCode(const StatusCode sc)
	{
		statusCode = sc;
		rPhrase = reasonPhrase(statusCode);
	}

	inline StatusCode getStatusCode() const
	{
		return statusCode;
	}

	std::string getReasonPhrase() const
	{
		return rPhrase;
	}

	virtual std::string getStartLine() const
	{
		std::string sCodeStr;

		try
		{
			sCodeStr = boost::lexical_cast<std::string>(getStatusCode());
		}
		catch(boost::bad_lexical_cast &)
		{
			sCodeStr = "";
		}
		
		return "HTTP/" + getVersion() + " " + sCodeStr + " " + getReasonPhrase() + CRLF;
	}

private:
	StatusCode statusCode;

	std::string rPhrase;
};

}
}

#endif
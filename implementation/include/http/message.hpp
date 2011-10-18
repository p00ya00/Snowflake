#ifndef SF_HTTP_MESSAGE_HPP
#define SF_HTTP_MESSAGE_HPP

#include <http/header.hpp>
#include <list>

namespace sf
{
namespace http
{

class Message
{
public:
	typedef boost::shared_ptr<Message> Ptr;

	typedef std::list<Header::Ptr> Headers;

	typedef Headers::iterator HeaderItr;

	typedef Headers::const_iterator HeaderConstItr;

	Message(const std::string &ver, const std::string &b)
	:version(ver), body(b), bodyIsOnDisk(false)
	{}

	virtual std::string getStartLine() const = 0;

	void addHeader(const Header::Ptr header);

	Header::Ptr getHeader(const std::string &name) const;

	void eraseHeader(const std::string &name);

	inline HeaderConstItr headersBegin() const
	{
		return headers.begin();
	}

	inline HeaderItr headersBegin()
	{
		return headers.begin();
	}
	
	inline HeaderConstItr headersEnd() const
	{
		return headers.end();
	}

	inline std::string getVersion() const
	{
		return version;
	}

	inline void setVersion(const std::string &v)
	{
		version = v;
	}

	inline size_t headersSize() const
	{
		return headers.size();
	}

	inline size_t bodySize() const
	{
		return body.size();
	}

	inline void clearAllHeaders()
	{
		headers.clear();
	}

	// the returned value does not include the entity body
	// since the 'body' member of the message is public
	std::string toString() const;

protected:
	Headers headers;

	std::string version;

public:
	std::string body;

	bool bodyIsOnDisk;
};

}
}

#endif
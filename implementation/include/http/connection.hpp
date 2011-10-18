#ifndef SF_HTTP_CONNECTION_HPP
#define SF_HTTP_CONNECTION_HPP

#include <http/request.hpp>
#include <http/response.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

namespace sf
{
namespace http
{

class Connection: public boost::noncopyable
{
public:
	typedef boost::shared_ptr<Connection> Ptr;

	Connection(boost::asio::io_service &ios)
	:request(new Request), response(new Response), socket(ios), keepAlive(false)
	{}

	Request::Ptr request;

	Response::Ptr response;
	
	boost::asio::streambuf readBuffer;
	
	tcp::socket socket;
	
	bool keepAlive;
};

}
}

#endif
#include <boost/test/unit_test.hpp>
#include <http/response.hpp>
#include <boost/lexical_cast.hpp>
using namespace sf::http;

BOOST_AUTO_TEST_SUITE(response_class)

BOOST_AUTO_TEST_CASE(response_message)
{
	Response::Ptr rp(new Response);

	BOOST_CHECK(rp->getReasonPhrase() == "Internal Server Error");

	rp->setVersion("1.1");
	rp->body += "<html><head><title>test</title></head>"
				"<body><h1>Hi!</h1></body></html>";
	Header::Ptr h1p(new Header("server", "Snowflake 0.2"));
	Header::Ptr h2p(new Header("content-type", "text/html"));
	std::string bodySize = boost::lexical_cast<std::string>(rp->body.length());
	Header::Ptr h3p(new Header("content-length", bodySize));
	Header::Ptr h4p(new Header("content-language", "en"));
	Header::Ptr h5p(new Header("accept-ranges", "none"));
	
	rp->addHeader(h1p);
	rp->addHeader(h2p);
	rp->addHeader(h3p);
	rp->addHeader(h4p);
	rp->addHeader(h5p);

	BOOST_CHECK(rp->headersSize() == 5);

	BOOST_CHECK(rp->toString() ==	"HTTP/1.1 500 Internal Server Error" CRLF
									"server: Snowflake 0.2" CRLF
									"content-type: text/html" CRLF
									"content-length: " + bodySize + CRLF
									"content-language: en" CRLF
									"accept-ranges: none" CRLF CRLF );

}

BOOST_AUTO_TEST_SUITE_END()
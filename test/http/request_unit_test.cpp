#include <boost/test/unit_test.hpp>
#include <http/request.hpp>
using namespace sf::http;

BOOST_AUTO_TEST_SUITE(request_class)

BOOST_AUTO_TEST_CASE(request_with_no_headers)
{
	Request::Ptr rp(new Request);
	
	BOOST_CHECK(rp->getMethod() == Request::GET_M);
	BOOST_CHECK(rp->getUri().empty());
	BOOST_CHECK(rp->bodySize() == 0);
	BOOST_CHECK(rp->headersSize() == 0);
	BOOST_CHECK(rp->headersBegin() == rp->headersEnd());
	BOOST_CHECK(rp->getHeader("date") == NULL);
	BOOST_CHECK(rp->toString() == "GET  HTTP/" CRLF CRLF);
}

BOOST_AUTO_TEST_CASE(request_with_one_header)
{
	Request::Ptr rp(new Request(Request::POST_M, "/index.html", "1.0"));

	BOOST_CHECK(rp->getMethod() == Request::POST_M);
	BOOST_CHECK(rp->getVersion() == "1.0");
	BOOST_CHECK(rp->getUri() == "/index.html");

	Header::Ptr h1p(new Header("date", "8.4.2011 03:08:23"));
	rp->addHeader(h1p);
	
	BOOST_CHECK(rp->headersSize() == 1);

	Header::Ptr hp = rp->getHeader("Date");
	BOOST_REQUIRE(hp);
	BOOST_CHECK(hp->valuesBegin()->v == "8.4.2011 03:08:23");
	BOOST_CHECK(hp->valuesBegin()->q.empty());

	rp->setUri("*");
	rp->setMethod(Request::OPTIONS_M);
	BOOST_CHECK(rp->toString() ==	"OPTIONS * HTTP/1.0" CRLF
									"date: 8.4.2011 03:08:23"  CRLF CRLF);

	rp->body += "id:11234000";
	BOOST_CHECK(rp->bodySize() > 0);

	rp->clearAllHeaders();
	BOOST_CHECK(rp->headersSize() == 0);
}

BOOST_AUTO_TEST_CASE(request_with_several_headers)
{
	Request::Ptr rp(new Request(Request::POST_M));
	rp->setVersion("1.0");
	rp->setUri("www.example.com/products/modelXYZ/catalog.html");

	Header::Ptr h1p(new Header("accept", "text/html", "q=1.0"));
	Header::Ptr h2p(new Header("server"));
	Header::Ptr h3p(new Header("host", "www.example.com"));
	rp->addHeader(h1p);
	rp->addHeader(h2p);
	rp->addHeader(h3p);
	BOOST_CHECK(rp->headersSize() == 3);
	BOOST_CHECK(rp->getHeader("server")->valuesSize() == 0);
	BOOST_CHECK(rp->getHeader("Host")->valuesBegin()->v == "www.example.com");
	BOOST_CHECK(rp->getHeader("if-match") == NULL);

	BOOST_CHECK(rp->toString() ==
		"POST www.example.com/products/modelXYZ/catalog.html HTTP/1.0" CRLF
		"accept: text/html;q=1.0" CRLF
		"server: " CRLF
		"host: www.example.com" CRLF CRLF);

	rp->getHeader("server")->addValue("Snowflake 0.2");
	BOOST_CHECK(rp->getHeader("server")->valuesSize() == 1);

	rp->eraseHeader("host");
	BOOST_CHECK(rp->headersSize() == 2);
	BOOST_CHECK(rp->getHeader("host") == NULL);

	rp->clearAllHeaders();
	BOOST_CHECK(rp->headersSize() == 0);
	BOOST_CHECK(rp->headersBegin() == rp->headersEnd());
}

BOOST_AUTO_TEST_SUITE_END()
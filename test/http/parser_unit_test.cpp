#include <istream>
#include <ostream>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <http/parser.hpp>
#include <http/exception.hpp>
using namespace sf::http;

BOOST_AUTO_TEST_SUITE(parser_test_suite)

BOOST_AUTO_TEST_CASE(test_isRequestLine)
{
	Parser parser;

	BOOST_CHECK(parser.isRequestLine("GET /index.html HTTP/1.1"));
	BOOST_CHECK(!parser.isRequestLine("Server: Snowflake/1.0"));
	BOOST_CHECK(!parser.isRequestLine(""));
	BOOST_CHECK(!parser.isRequestLine("accept"));
	BOOST_CHECK(!parser.isRequestLine(CRLF));
	BOOST_CHECK(parser.isRequestLine("POST /home"));
	BOOST_CHECK(!parser.isRequestLine("Content-Length   :  1024"));
	BOOST_CHECK(parser.isRequestLine("HEAD http://www.example.com/dl.exe HTTP/1.0"));
}

BOOST_AUTO_TEST_CASE(test_parseRequestLine)
{
	Parser parser;
	Request req;
	std::string line;

	line = "GET / HTTP/1.1";
	parser.parseRequestLine(req, line);
	BOOST_CHECK(req.getMethod() == Request::GET_M);
	BOOST_CHECK(req.getUri() == "/");
	BOOST_CHECK(req.getVersion() == "1.1");

	line = "     POST /main.html HTTP/";
	parser.parseRequestLine(req, line);
	BOOST_CHECK(req.getMethod() == Request::POST_M);
	BOOST_CHECK(req.getUri() == "/main.html");
	BOOST_CHECK(req.getVersion() == "");

	line = "";
	BOOST_CHECK_THROW(parser.parseRequestLine(req, line), BadRequestException);

	line = "DEL /files/cat.pdf HTTP/1.0";
	BOOST_CHECK_THROW(parser.parseRequestLine(req, line), BadRequestException);

	line = "HEAD \t";
	BOOST_CHECK_THROW(parser.parseRequestLine(req, line), BadRequestException);

	line = "\t          CONNect http://www.yahoo.com:443";
	BOOST_CHECK_THROW(parser.parseRequestLine(req, line), BadRequestException);
	BOOST_CHECK(req.getMethod() == Request::CONNECT_M);
	BOOST_CHECK(req.getUri() == "http://www.yahoo.com:443");

	line = "get   http://www.kkkkk.dddddd/ HTTP";
	BOOST_CHECK_THROW(parser.parseRequestLine(req, line), BadRequestException);

	line = "HEAD   http://www.ets.co/kkkkk.dddddd HT";
	BOOST_CHECK_THROW(parser.parseRequestLine(req, line), BadRequestException);

	line = "\tOPTIONS\t*\tHTTP/1.1";
	parser.parseRequestLine(req, line);
	BOOST_CHECK(req.getMethod() == Request::OPTIONS_M);
	BOOST_CHECK(req.getUri() == "*");
	BOOST_CHECK(req.getVersion() == "1.1");

	line = "get   /p00ya00%40yahoo.com%7C9 HTTP/1.1";
	parser.parseRequestLine(req, line);
	BOOST_CHECK(req.getMethod() == Request::GET_M);
	BOOST_CHECK(req.getUri() == "/p00ya00@yahoo.com|9");
	BOOST_CHECK(req.getVersion() == "1.1");
}

BOOST_AUTO_TEST_CASE(test_parseHeader)
{
	Header::Ptr hp;
	std::string line;
	Parser parser;
	Header::ValueConstItr vItr;

	hp = parser.parseHeader(line);
	BOOST_CHECK(hp == NULL);

	line = "           accePT\t\t:";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "accept");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v.empty());
	
	line = "Host: www.example.com";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "host");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "www.example.com");
	BOOST_CHECK(hp->valuesBegin()->q.empty());

	line = "Accept-Encoding : gzip;q=1.0, identity; q=0.5, *;q=0";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "accept-encoding");
	BOOST_CHECK(hp->valuesSize() == 3);
	vItr = hp->valuesBegin();
	BOOST_CHECK(vItr->v == "gzip");
	BOOST_CHECK(vItr->q == "q=1.0");
	++vItr;
	BOOST_CHECK(vItr->v == "identity");
	BOOST_CHECK(vItr->q == "q=0.5");
	++vItr;
	BOOST_CHECK(vItr->v == "*");
	BOOST_CHECK(vItr->q == "q=0");
	BOOST_CHECK(++vItr == hp->valuesEnd());
	vItr = hp->getValue("gzip");
	BOOST_REQUIRE(vItr != hp->valuesEnd());
	BOOST_CHECK(Header::extractParam(vItr->q, "q") == "1.0");

	line = "Accept-Language: da, en-gb, en";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "accept-language");
	BOOST_CHECK(hp->valuesSize() == 3);
	vItr = hp->valuesBegin();
	BOOST_CHECK(vItr->v == "da");
	BOOST_CHECK(vItr->q.empty());
	++vItr;
	BOOST_CHECK(vItr->v == "en-gb");
	BOOST_CHECK(vItr->q.empty());
	++vItr;
	BOOST_CHECK(vItr->v == "en");
	BOOST_CHECK(vItr->q.empty());
	BOOST_CHECK(++vItr == hp->valuesEnd());
	vItr = hp->getValue("fa");
	BOOST_CHECK(vItr == hp->valuesEnd());

	line = "\tContent-Range\t:\tbytes 21010-47021/47022";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "content-range");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "bytes 21010-47021/47022");

	line = "Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "last-modified");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "Tue, 15 Nov 1994 12:45:26 GMT");

	line = "If-Unmodified-Since:";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "if-unmodified-since");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v.empty());

	line = "Expires:Sun Nov  6 08:49:37 1994";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "expires");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "Sun Nov  6 08:49:37 1994");

	line = "Content-Type : text/html ; charset = ISO-8859-4";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "content-type");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "text/html");
	BOOST_CHECK(hp->valuesBegin()->q == "charset = ISO-8859-4");

	line = "ETag: \"xyzzy\"";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "etag");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "xyzzy");
	BOOST_CHECK(hp->valuesBegin()->q.empty());

	line = "ETag: W/\"xy,zzy\"";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "etag");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "W/xy,zzy");
	BOOST_CHECK(hp->valuesBegin()->q.empty());

	line = "If-Match: \"xyzzy\", \"r2d2xxxx\", \"c3piozzzz\"";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "if-match");
	BOOST_CHECK(hp->valuesSize() == 3);
	vItr = hp->valuesBegin();
	BOOST_CHECK(vItr->v == "xyzzy");
	vItr++;
	BOOST_CHECK(vItr->v == "r2d2xxxx");
	vItr++;
	BOOST_CHECK(vItr->v == "c3piozzzz");

	line = "Server: CERN/3.0 libwww/2.17";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "server");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "CERN/3.0 libwww/2.17");

	line = "       Upgrade :   HTTP/2.0   ,SHTTP/1.3, IRC/6.9, RTA/x11,";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "upgrade");
	BOOST_CHECK(hp->valuesSize() == 5);
	vItr = hp->valuesBegin();
	BOOST_CHECK(vItr->v == "HTTP/2.0");
	vItr++;
	BOOST_CHECK(vItr->v == "SHTTP/1.3");
	vItr++;
	BOOST_CHECK(vItr->v == "IRC/6.9");
	vItr++;
	BOOST_CHECK(vItr->v == "RTA/x11");
	vItr++;
	BOOST_CHECK(vItr->v.empty());
	BOOST_CHECK(++vItr == hp->valuesEnd());

	line = "Accept: text/*;q=0.3, text/html;level=2;q=0.7, text/html;level=1,"
			"text/html;q=0.4, */*;q=0.5";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "accept");
	BOOST_CHECK(hp->valuesSize() == 3);
	vItr = hp->valuesBegin();
	BOOST_CHECK(vItr->v == "text/*");
	BOOST_CHECK(vItr->q == "q=0.3");
	vItr++;
	BOOST_CHECK(vItr->v == "text/html");
	BOOST_CHECK(vItr->q == "level=2;q=0.7");
	vItr++;
	BOOST_CHECK(vItr->v == "*/*");
	BOOST_CHECK(vItr->q == "q=0.5");
	BOOST_CHECK(++vItr == hp->valuesEnd());

	// warning: "Beware of quotes!"
	line = "warning: \\\"Beware of quotes!\\\"";
	hp = parser.parseHeader(line);
	BOOST_REQUIRE(hp != NULL);
	BOOST_CHECK(hp->getName() == "warning");
	BOOST_CHECK(hp->valuesBegin()->v == "\"Beware of quotes!\"");
}

BOOST_AUTO_TEST_CASE(test_getLine)
{
	std::string line;
	Parser parser;
	std::stringbuf buf;
	std::istream input(&buf);

	buf.str("");
	BOOST_CHECK(parser.getLine(input, line, 100) == false);

	input.clear();
	buf.str("\n");
	BOOST_CHECK(parser.getLine(input, line, 100) == true);
	BOOST_CHECK(line.empty());

	input.clear();
	buf.str("test new line");
	BOOST_REQUIRE(parser.getLine(input, line, 4 * 1024) == true);
	BOOST_CHECK(line == "test new line");

	input.clear();
	buf.str("GET /en/main.html HTTP/1.1\r\n");
	BOOST_REQUIRE(parser.getLine(input, line, 100) == true);
	if(line[line.size() - 1] == CR)
			line.pop_back();
	BOOST_CHECK(line == "GET /en/main.html HTTP/1.1");

	input.clear();
	buf.str("POST /cgi/students.php HTTP/1.0\n"
			"Host: www.github.com\n"
			"\n"
			"id=123098");
	BOOST_REQUIRE(parser.getLine(input, line, 100) == true);
	BOOST_CHECK(line == "POST /cgi/students.php HTTP/1.0");
	BOOST_REQUIRE(parser.getLine(input, line, 100) == true);
	BOOST_CHECK(line == "Host: www.github.com");
	BOOST_REQUIRE(parser.getLine(input, line, 100) == true);
	BOOST_CHECK(line.empty());
	BOOST_REQUIRE(parser.getLine(input, line, 100) == true);
	BOOST_CHECK(line == "id=123098");
	BOOST_REQUIRE(parser.getLine(input, line, 100) == false);

	input.clear();
	buf.str("0123456789-qwertyuiopasdfghjklzxcvbnm\nNEW_LINE");
	BOOST_REQUIRE(parser.getLine(input, line, 20) == true);
	BOOST_CHECK(line == "0123456789-qwertyuio");
	BOOST_REQUIRE(parser.getLine(input, line, 20) == true);
	BOOST_CHECK(line == "pasdfghjklzxcvbnm");
	BOOST_REQUIRE(parser.getLine(input, line, 100) == true);
	BOOST_CHECK(line == "NEW_LINE");
	BOOST_REQUIRE(parser.getLine(input, line, 100) == false);
}

BOOST_AUTO_TEST_CASE(test_parseRequest)
{
	boost::asio::streambuf buf;
	std::ostream stream(&buf);
	Request::Ptr req;
	Parser parser;
	Request::HeaderConstItr hItr;
	Header::ValueConstItr	vItr;
	const char *bufData;
	std::string uri;

	req.reset(new Request());
	stream << "GET /index.htm HTTP/1.1\r\n";
	BOOST_REQUIRE_NO_THROW(parser.parseRequest(*req, buf));
	BOOST_CHECK(req->getMethod() == Request::GET_M);
	BOOST_CHECK(req->getUri() == "/index.htm");
	BOOST_CHECK(req->getVersion() == "1.1");
	BOOST_CHECK(req->headersSize() == 0);

	req.reset(new Request());
	stream << "POST / HTTP/1.0\r\n"
		<< "Accept: text/plain; q=0.5, text/html, text/x-dvi; q=0.8, text/x-c\r\n"
		<< "Connection: close";
	BOOST_REQUIRE_NO_THROW(parser.parseRequest(*req, buf));
	BOOST_CHECK(req->getMethod() == Request::POST_M);
	BOOST_CHECK(req->getUri() == "/");
	BOOST_CHECK(req->getVersion() == "1.0");
	BOOST_CHECK(req->headersSize() == 2);
	hItr = req->headersBegin();
	BOOST_CHECK((*hItr)->getName() == "accept");
	BOOST_CHECK((*hItr)->valuesSize() == 4);
	vItr = (*hItr)->valuesBegin();
	BOOST_CHECK(vItr->v == "text/plain");
	BOOST_CHECK(vItr->q == "q=0.5");
	++vItr;
	BOOST_CHECK(vItr->v == "text/html");
	BOOST_CHECK(vItr->q.empty());
	++vItr;
	BOOST_CHECK(vItr->v == "text/x-dvi");
	BOOST_CHECK(vItr->q == "q=0.8");
	++vItr;
	BOOST_CHECK(vItr->v == "text/x-c");
	BOOST_CHECK(vItr->q.empty());
	BOOST_CHECK(++vItr == (*hItr)->valuesEnd());
	++hItr;
	BOOST_CHECK((*hItr)->getName() == "connection");
	vItr = (*hItr)->valuesBegin();
	BOOST_CHECK(vItr->v == "close");
	BOOST_CHECK(vItr->q.empty());

	buf.consume(buf.size());
	BOOST_REQUIRE_THROW(parser.parseRequest(*req, buf), BadRequestException);

	stream << CRLF;
	BOOST_REQUIRE_THROW(parser.parseRequest(*req, buf), BadRequestException);

	req.reset(new Request());
	stream << "Date: Wed, 15 Nov 1995 06:25:24 GMT\n"
		<< "Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\n"
		<< "Content-Range: bytes 21010-47021/47022\n"
		<< "Content-Length: 26012\n"
		<< "Content-Type: image/gif\n";
	BOOST_REQUIRE_NO_THROW(parser.parseRequest(*req, buf));
	BOOST_CHECK(req->getMethod() == Request::GET_M);
	BOOST_CHECK(req->getUri().empty());
	BOOST_CHECK(req->getVersion().empty());
	BOOST_CHECK(req->headersSize() == 5);
	hItr = req->headersBegin();
	BOOST_CHECK((*hItr)->getName() == "date");
	BOOST_CHECK((*hItr)->valuesSize() == 1);
	BOOST_CHECK((*hItr)->valuesBegin()->v == "Wed, 15 Nov 1995 06:25:24 GMT");
	++hItr;
	BOOST_CHECK((*hItr)->getName() == "last-modified");
	BOOST_CHECK((*hItr)->valuesSize() == 1);
	BOOST_CHECK((*hItr)->valuesBegin()->v == "Wed, 15 Nov 1995 04:58:08 GMT");
	++hItr;
	BOOST_CHECK((*hItr)->getName() == "content-range");
	BOOST_CHECK((*hItr)->valuesSize() == 1);
	BOOST_CHECK((*hItr)->valuesBegin()->v == "bytes 21010-47021/47022");
	++hItr;
	BOOST_CHECK((*hItr)->getName() == "content-length");
	BOOST_CHECK((*hItr)->valuesSize() == 1);
	BOOST_CHECK((*hItr)->valuesBegin()->v == "26012");
	++hItr;
	BOOST_CHECK((*hItr)->getName() == "content-type");
	BOOST_CHECK((*hItr)->valuesSize() == 1);
	BOOST_CHECK((*hItr)->valuesBegin()->v == "image/gif");
	++hItr;
	BOOST_CHECK(hItr == req->headersEnd());

	req.reset(new Request());
	stream << "DELETE /files/hi.txt HTTP/1.1\r\n"
		<< "Accept: text/html\r\n"
		<< "Accept-Language: en\r\n"
		<< "Accept-Ranges: none\r\n"
		<< "\r\n"
		<< "<html><head><body></html>";
	BOOST_REQUIRE_NO_THROW(parser.parseRequest(*req, buf));
	BOOST_CHECK(req->getMethod() == Request::DELETE_M);
	BOOST_CHECK(req->getUri() == "/files/hi.txt");
	BOOST_CHECK(req->getVersion() == "1.1");
	BOOST_CHECK(req->headersSize() == 3);
	hItr = req->headersBegin();
	BOOST_CHECK((*hItr)->getName() == "accept");
	BOOST_CHECK((*hItr)->valuesBegin()->v == "text/html");
	++hItr;
	BOOST_CHECK((*hItr)->getName() == "accept-language");
	BOOST_CHECK((*hItr)->valuesBegin()->v == "en");
	++hItr;
	BOOST_CHECK((*hItr)->getName() == "accept-ranges");
	BOOST_CHECK((*hItr)->valuesBegin()->v == "none");
	bufData = boost::asio::buffer_cast<const char *>(buf.data());
	BOOST_CHECK(std::string(bufData, buf.size()) == "<html><head><body></html>");

	req.reset(new Request());
	stream << "GET /files/cgi/s?id=";
	for(int i = 0; i < 1019; ++i)
		stream << "1234";
	stream << " HTTP/1.1";
	BOOST_REQUIRE_THROW(parser.parseRequest(*req, buf), UriTooLargeException);

	//a request line with a length of (4*1024 - 1)
	buf.consume(buf.size());
	req.reset(new Request());
	stream << "GET ";
	uri = "/main?id=000";
	for(int i = 0; i < 407; ++i)
		uri += "asdfghjklp";
	stream << uri << " HTTP/1.0";
	BOOST_REQUIRE_NO_THROW(parser.parseRequest(*req, buf));
	BOOST_CHECK(req->getMethod() == Request::GET_M);
	BOOST_CHECK(req->getUri() == uri);
	BOOST_CHECK(req->getVersion() == "1.0");
	BOOST_CHECK(req->headersSize() == 0);

	buf.consume(buf.size());
	req.reset(new Request());
	unsigned long oldMaxHeaders = parser.setMaxHeaders(5);
	stream << "GET / HTTP/1\r\n"
			"From: \r\n"
			"accept-language : en\r\n"
			"accept-encoding: none\r\n"
			"date: 12:34:00 GMT\r\n"
			"if-modified-since\r\n"
			"TE: trailers";
	BOOST_REQUIRE_THROW(parser.parseRequest(*req, buf), BadRequestException);
	BOOST_CHECK(req->headersSize() == parser.getMaxHeaders());
	parser.setMaxHeaders(oldMaxHeaders);
	BOOST_CHECK(parser.getMaxHeaders() == oldMaxHeaders);

	buf.consume(buf.size());
	req.reset(new Request());
	stream << 
			"OPTIONS * HTTP/1.1" CRLF
			"accept-language: en, fr, de,"
			"\tfa" CRLF
			"server: Snowflake" CRLF
			"Accept: text/plain; q=0.5, text/html,"
			"        text/x-dvi; q=0.8, text/x-c" CRLF
			CRLF
			"entity-body";
	BOOST_REQUIRE_NO_THROW(parser.parseRequest(*req, buf));
	BOOST_CHECK(req->getMethod() == Request::OPTIONS_M);
	BOOST_CHECK(req->getUri() == "*");
	BOOST_CHECK(req->getVersion() == "1.1");
	BOOST_CHECK(req->headersSize() == 3);
	hItr = req->headersBegin();
	BOOST_CHECK((*hItr)->getName() == "accept-language");
	BOOST_CHECK((*hItr)->valuesSize() == 4);
	vItr = (*hItr)->valuesBegin();
	BOOST_CHECK(vItr->v == "en");
	BOOST_CHECK((++vItr)->v == "fr");
	BOOST_CHECK((++vItr)->v == "de");
	BOOST_CHECK((++vItr)->v == "fa");
	BOOST_CHECK(++vItr == (*hItr)->valuesEnd());
	BOOST_CHECK((*++hItr)->getName() == "server");
	BOOST_CHECK((*hItr)->valuesSize() == 1);
	BOOST_CHECK((*++hItr)->getName() == "accept");
	BOOST_CHECK((*hItr)->valuesSize() == 4);
	BOOST_CHECK(++hItr == req->headersEnd());
	bufData = boost::asio::buffer_cast<const char *>(buf.data());
	BOOST_CHECK(std::string(bufData, buf.size()) == "entity-body");
	std::string httpReq("OPTIONS * HTTP/1.1" CRLF
						"accept-language: en, fr, de, fa" CRLF
						"server: Snowflake" CRLF
						"accept: text/plain;q=0.5, text/html, text/x-dvi;q=0.8, text/x-c" CRLF
						CRLF);
	BOOST_CHECK(req->toString() == httpReq);

	//test maxHeaderLength

	//test maxTotalHeadSize
}

BOOST_AUTO_TEST_SUITE_END()
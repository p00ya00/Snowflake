#define BOOST_TEST_MODULE HTTP

#include <boost/test/unit_test.hpp>
#include <http/header.hpp>
using namespace sf::http;

BOOST_AUTO_TEST_SUITE(header_class)

BOOST_AUTO_TEST_CASE(header_with_no_values)
{
	Header::Ptr headerp(new Header("server"));
	BOOST_CHECK(headerp->getName() == "server");
	BOOST_CHECK(headerp->valuesSize() == 0);
	BOOST_CHECK(headerp->valuesBegin() == headerp->valuesEnd());
	BOOST_CHECK(headerp->toString() == "server: ");

	Header::ValueConstItr itr = headerp->getValue("text/html");
	BOOST_CHECK(itr == headerp->valuesEnd());
}

BOOST_AUTO_TEST_CASE(header_with_one_value)
{
	Header::Ptr hp(new Header("Content-Type", "text/html"));
	BOOST_CHECK(hp->getName() == "content-type");
	
	BOOST_CHECK(hp->valuesSize() == 1);
	
	Header::ValueConstItr itr = hp->valuesBegin();
	BOOST_CHECK(itr->v == "text/html");
	BOOST_CHECK(itr->q.empty());

	hp->changeValue("text/html", "text/text", "charset=ascii");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(itr->v == "text/text");
	BOOST_CHECK(itr->q == "charset=ascii");
	BOOST_CHECK(Header::extractParam(itr->q, "charset") == "ascii");

	itr = hp->getValue("Text/text");
	BOOST_CHECK(itr->v == "text/text");
	BOOST_CHECK(itr->q == "charset=ascii");

	BOOST_CHECK(hp->toString() == "content-type: text/text;charset=ascii");

	hp->clearAllValues();
	BOOST_CHECK(hp->valuesSize() == 0);
	BOOST_CHECK(hp->valuesBegin() == hp->valuesEnd());
}

BOOST_AUTO_TEST_CASE(header_with_several_values)
{
	Header::Ptr hp;
	hp.reset( new Header("accept", "Audio/Wave", "q=0.6") );

	hp->addValue("text/html");
	BOOST_CHECK(hp->valuesSize() == 2);
	BOOST_CHECK(hp->toString() == "accept: Audio/Wave;q=0.6, text/html");

	Header::ValueConstItr itr = hp->getValue("Text/HTML");
	BOOST_CHECK(itr->v == "text/html");
	BOOST_CHECK(itr->q.empty());

	itr = hp->getValue("audio/wave");
	BOOST_CHECK(itr->v == "Audio/Wave");
	BOOST_CHECK(Header::extractParam(itr->q, "q") == "0.6");

	itr = hp->getValue("video/*");
	BOOST_CHECK(itr == hp->valuesEnd());

	BOOST_CHECK(Header::extractParam(hp->valuesBegin()->q, "q") == "0.6");

	hp->eraseValue("application/flash");
	BOOST_CHECK(hp->valuesSize() == 2);

	hp->changeValue("Audio/Wave", "video/avi");
	BOOST_CHECK(hp->toString() == "accept: video/avi, text/html");

	hp->eraseValue("text/html");
	BOOST_CHECK(hp->valuesSize() == 1);
	BOOST_CHECK(hp->valuesBegin()->v == "video/avi");
	BOOST_CHECK(hp->valuesBegin()->q.empty());

	hp->addValue("application/msword");
	hp->addValue("text/html", "q=1");
	BOOST_CHECK(hp->toString() == "accept: video/avi, application/msword, text/html;q=1");
	BOOST_CHECK(hp->valuesSize() == 3);

	hp->clearAllValues();
	BOOST_CHECK(hp->valuesSize() == 0);
}

BOOST_AUTO_TEST_CASE(extract_parameter)
{
	std::string input, res;

	res = Header::extractParam(input, "q");
	BOOST_CHECK(res.empty());

	input = "q = 10";
	res = Header::extractParam(input,"q");
	BOOST_CHECK(res == "10");

	input = "charset=ISO1001";
	res = Header::extractParam(input, "charset");
	BOOST_CHECK(res == "ISO1001");

	input = "param = ";
	res = Header::extractParam(input, "param");
	BOOST_CHECK(res.empty());

	input = "q =	 0.900";
	res = Header::extractParam(input, "q");
	BOOST_CHECK(res == "0.900");

	input = "parameter			=text";
	res = Header::extractParam(input, "parameter");
	BOOST_CHECK(res == "text");

	res = Header::extractParam(input, "q");
	BOOST_CHECK(res.empty());

	// for now this will always issue a warning,
	// since str_algo::istarts_with is used to
	// check if 'input' begins with 'attribute'.
	// Because quality values used in http are
	// limited and none of them starts with another
	// one, this won't be a problem!
	res = Header::extractParam(input, "param");
	BOOST_WARN(res.empty());
}

BOOST_AUTO_TEST_SUITE_END()
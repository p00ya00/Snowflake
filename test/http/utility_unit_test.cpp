#include <boost/test/unit_test.hpp>
#include <http/utility.hpp>
using namespace sf::http;

BOOST_AUTO_TEST_SUITE(http_utility)

BOOST_AUTO_TEST_CASE(percent_decode)
{
	std::string uri;

	uri = "http://www.example.com/%7epooya";
	percentDecode(uri);
	BOOST_CHECK(uri == "http://www.example.com/~pooya");

	uri = "http://www.example.com/hello%20World";
	percentDecode(uri);
	BOOST_CHECK(uri == "http://www.example.com/hello World");

	uri = "http://www.example.com/%25%5c6";
	percentDecode(uri);
	BOOST_CHECK(uri == "http://www.example.com/%\\6");

	uri = "";
	percentDecode(uri);
	BOOST_CHECK(uri == "");

	uri = "www.stackoverflow.com";
	percentDecode(uri);
	BOOST_CHECK(uri == "www.stackoverflow.com");

	uri = "www.example.com/%5F";
	percentDecode(uri);
	BOOST_CHECK(uri == "www.example.com/_");

	uri = "www.example.com/%hu";
	percentDecode(uri);
	BOOST_CHECK(uri == "www.example.com/_");

	uri = "www.example.com/%";
	percentDecode(uri);
	BOOST_CHECK(uri == "www.example.com/_");

	uri = "www.example.com/%A";
	percentDecode(uri);
	BOOST_CHECK(uri == "www.example.com/\n");

	uri = "www.example.com/%ArF";
	percentDecode(uri);
	BOOST_CHECK(uri == "www.example.com/\nF");
}

BOOST_AUTO_TEST_SUITE_END()
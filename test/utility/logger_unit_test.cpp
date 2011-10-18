#define BOOST_TEST_MODULE UTILITY

//Disabling MSVC's warning about fopen not being secure
#pragma warning(disable: 4996)

#include <boost/test/unit_test.hpp>
#include <utility/logger.hpp>
using namespace sf::utility;

BOOST_AUTO_TEST_SUITE(logger_class)

BOOST_AUTO_TEST_CASE(create_logger_s)
{
	BOOST_REQUIRE_THROW(Logger("", Logger::WARN_L), InvalidLogException);

	Logger log("d:\\log.txt", Logger::DEBUG_L);
	BOOST_REQUIRE(log.logLevel() == Logger::DEBUG_L);
	BOOST_REQUIRE(log.outputPath() == "d:\\log.txt");

	BOOST_REQUIRE_NO_THROW
	(
		log(Logger::WARN_L, "Logging an %s message on line %d from file '%s'.\n", 
			log.logLevelString(log.WARN_L).c_str(), __LINE__, __FILE__);
	);

	BOOST_REQUIRE_NO_THROW
	(
		log(Logger::ERROR_L, "Logging an %s message on line %d from file '%s'.\n",
			"error", __LINE__, __FILE__);
	);

	BOOST_REQUIRE_NO_THROW
	(
		log(Logger::DEBUG_L, "This should not appear in the log. NOT!\n");
	);

	BOOST_REQUIRE_NO_THROW
	(
		log(Logger::INFO_L, "This one neither!!\n");
	);

	BOOST_REQUIRE_NO_THROW
	(
		log("This is a free log entry without any %s or %s.\n", "level", "timestamp");
	);
}

BOOST_AUTO_TEST_SUITE_END()
#define BOOST_TEST_MODULE UTILITY_AFIO
#include <boost/test/unit_test.hpp>

#include <utility/afio.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

using namespace sf::afio;
using namespace boost::system;
namespace asio_error = boost::asio::error;

//Sync access to std::cout
boost::mutex coutMutex;

//Enable/disable logging diagnostics information
#define LOG_INFO true

std::string datasetPath()
{
	std::string path(__FILE__);
	std::string folderName("afio_dataset");

#	if defined(_WIN32)
		path = path.substr(0, path.find_last_of('\\')) + "\\" + folderName + "\\";
#	else
		path = path.substr(0, path.find_last_of('/')) + "/" + folderName + "/";
#	endif

	return path;
}

void assertRead(const boost::system::error_code &error, std::size_t transferred,
                const std::vector<char> &buf, std::size_t requestedReadSize,
                const boost::system::error_code &expectedError, bool printDiagnostics)
{
    BOOST_REQUIRE(error == expectedError);

    if(printDiagnostics && LOG_INFO)
    {
        std::size_t bufferSize = 
            requestedReadSize < transferred ? requestedReadSize : transferred;
        boost::lock_guard<boost::mutex> lock(coutMutex);
        std::clog << "\n\n\nError: " << error << std::endl;
        std::clog << "Transferred: " << transferred << std::endl;
        std::clog << "Expected error: " << expectedError << std::endl;
        std::clog << "Requested read size: " << requestedReadSize << std::endl;
        std::cout << "Buffer contents: [";
        std::copy(buf.begin(), buf.begin() + bufferSize,
                    std::ostream_iterator<char>(std::cout));
	    std::cout << "]\n";
    }
}

void assertWrite(const boost::system::error_code &error, std::size_t transferred,
                 std::size_t expectedTransferred, const boost::system::error_code &expectedError,
                 bool printDiagnostics)
{
    BOOST_REQUIRE(error == expectedError);
    BOOST_REQUIRE_EQUAL(transferred, expectedTransferred);

    if(printDiagnostics && LOG_INFO)
    {
        boost::lock_guard<boost::mutex> lock(coutMutex);
        std::clog << "\n\n\nError: " << error << std::endl;
        std::clog << "Transferred: " << transferred << std::endl;
        std::clog << "Expected error: " << expectedError << std::endl;
        std::clog << "Expected transfer: " << expectedTransferred << std::endl;
    }
}

void handleWrite(const boost::system::error_code &error, std::size_t transferred)
{
    if(LOG_INFO)
    {
        boost::lock_guard<boost::mutex> lock(coutMutex);
        std::clog << "\n\n\nError: " << error << std::endl;
        std::clog << "Transferred: " << transferred << std::endl;
    }
}

void pause(boost::uint16_t sec)
{
    static boost::asio::io_service ios;
    boost::asio::deadline_timer timer(ios);

    timer.expires_from_now(boost::posix_time::seconds(sec));
    timer.wait();
}

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(afio_unit_test)

BOOST_AUTO_TEST_CASE(is_open)
{
	AsyncFileIn theStranger(datasetPath() + "the_stranger.txt", READ_M);
	BOOST_REQUIRE(!theStranger.isOpen());

	AsyncFileOut outputFile(datasetPath(), WRITE_TRUNC_M);
	BOOST_REQUIRE(!outputFile.isOpen());

    AsyncFileInOut sampleFile(datasetPath() + "fake.file", READ_WRITE_M);
    BOOST_REQUIRE(sampleFile.isOpen());

    AsyncFileInOut data_file(datasetPath() + "data.dat", READ_WRITE_TRUNC_M);
    BOOST_REQUIRE(data_file.isOpen());

    AsyncFileOut results(datasetPath() + "results.bin", WRITE_TRUNC_M);
    BOOST_REQUIRE(!results.isOpen());
}

BOOST_AUTO_TEST_CASE(path)
{
    AsyncFileOut tempFile(datasetPath() + "temp.bin", WRITE_M);
    BOOST_REQUIRE_EQUAL(tempFile.path(), datasetPath() + "temp.bin");
}

BOOST_AUTO_TEST_CASE(cancel_invalid_file)
{
    AsyncFileIn invalidFile(datasetPath() + "invalid_file.pdf", READ_M);

    BOOST_REQUIRE(!invalidFile.isOpen());
    BOOST_REQUIRE_THROW(invalidFile.cancel(), error::CancellationFailure);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(afio_async_read_test)

BOOST_AUTO_TEST_CASE(successful)
{
    std::vector<char> buf(1024), buf2(512);
	AsyncFileIn txtFile(datasetPath() + "is_there_a_god_russell.txt", READ_M);
	std::size_t readSize = 100;
    error_code successErrorCode; //default constructor creates 'success' error code
	
    BOOST_REQUIRE(txtFile.isOpen());
    txtFile.asyncRead(boost::asio::buffer(buf),
	                  boost::bind(&assertRead ,
	                              boost::asio::placeholders::error, 
	                              boost::asio::placeholders::bytes_transferred, 
	                              boost::ref(buf), readSize,
                                  successErrorCode, true),
	                  readSize, 200);
    
    readSize = 200;
    txtFile.asyncRead(boost::asio::buffer(buf2),
	                  boost::bind(&assertRead ,
	                              boost::asio::placeholders::error, 
	                              boost::asio::placeholders::bytes_transferred, 
	                              boost::ref(buf2), readSize,
                                  successErrorCode, true),
	                  readSize, 17500);

    pause(1);
    BOOST_REQUIRE_NO_THROW(txtFile.cancel());

    AsyncFileInOut review(datasetPath() + "no_country_for_old_men.txt", READ_WRITE_M);
    readSize = 1024;
    review.asyncRead( boost::asio::buffer(buf),
	                  boost::bind(&assertRead ,
	                              boost::asio::placeholders::error, 
	                              boost::asio::placeholders::bytes_transferred, 
	                              boost::ref(buf), readSize,
                                  successErrorCode, true),
	                  readSize );
    pause(1);
}

BOOST_AUTO_TEST_CASE(eof)
{
    std::vector<char> buf(1024);
    std::size_t readSize = 500;
    error_code eofErrorCode = asio_error::make_error_code(asio_error::eof);
    AsyncFileIn beckett(datasetPath() + "the_unnamable_beckett.htm", READ_M);

    BOOST_REQUIRE(beckett.isOpen());
    beckett.asyncRead(boost::asio::buffer(buf),
	                  boost::bind(&assertRead ,
	                              boost::asio::placeholders::error, 
	                              boost::asio::placeholders::bytes_transferred, 
	                              boost::ref(buf), readSize,
                                  eofErrorCode, true),
	                  readSize, 31500);
    
    pause(1);
    BOOST_REQUIRE_NO_THROW(beckett.close());

    AsyncFileInOut review(datasetPath() + "no_country_for_old_men.txt", READ_WRITE_M);
    readSize = 1024;
    review.asyncRead( boost::asio::buffer(buf),
	                  boost::bind(&assertRead ,
	                              boost::asio::placeholders::error, 
	                              boost::asio::placeholders::bytes_transferred, 
	                              boost::ref(buf), readSize,
                                  eofErrorCode, true),
	                  readSize, 4000 );
    pause(1);
}

BOOST_AUTO_TEST_CASE(non_existing_file)
{
    AsyncFileInOut theClown(datasetPath() + "the_clown.pdf", READ_M);
    std::vector<char> buf(667);
    std::size_t readSize = 666;
    error_code invalidFileErrorCode(errc::no_such_file_or_directory, system_category());

    BOOST_REQUIRE(!theClown.isOpen());
    theClown.asyncRead(boost::asio::buffer(buf),
                       boost::bind(&assertRead ,
	                               boost::asio::placeholders::error, 
	                               boost::asio::placeholders::bytes_transferred, 
	                               boost::ref(buf), readSize,
                                   invalidFileErrorCode, true));

	pause(1);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(afio_async_write_test)

BOOST_AUTO_TEST_CASE(successful)
{
    std::string buf =  "In order to investigate nature Beckett exposes in his "
                       "work the frames within which the two events (of being "
                       "and of signification and symbolization) are produced."
                       " \n\nFor Beckett, nature is (dis)placed between intervals"
                       " when the faculty of reason is asleep, and when it is "
                       "active. Nature is apprehended differently, at distance "
                       "from reason (metaphorically) and from within reason "
                       "(metonymically).";

	AsyncFileOut outputFile(datasetPath() + "write_test.txt", WRITE_M);
    std::size_t writeSize = buf.size();
    error_code successErrorCode; //default constructor creates 'success' error code
	
    BOOST_REQUIRE(outputFile.isOpen());
    outputFile.asyncWrite(boost::asio::buffer(buf),
	                      boost::bind(&assertWrite,
	                                  boost::asio::placeholders::error, 
	                                  boost::asio::placeholders::bytes_transferred, 
	                                  writeSize, successErrorCode, true));
    
    std::vector<char> buf2(20, '$');
    AsyncFileInOut bufContent(datasetPath() + "buf_content.bin", READ_WRITE_M);
    writeSize = 10;
    const std::vector<char> &bufRef = buf2;

    BOOST_REQUIRE(bufContent.isOpen());
    bufContent.asyncWrite(boost::asio::buffer(bufRef, writeSize),
                          boost::bind(&assertWrite,
                                      boost::asio::placeholders::error, 
	                                  boost::asio::placeholders::bytes_transferred, 
	                                  writeSize, successErrorCode, true), 
                          writeSize, 5);
    
    pause(1);
    outputFile.close();
    bufContent.close();
}

BOOST_AUTO_TEST_CASE(non_existing_file)
{
    char data[] = "data to be written.";
    std::size_t dataSize = strlen(data);
    AsyncFileOut outputFile(datasetPath() + "non_extisting_file.txt", WRITE_TRUNC_M);
    error_code invalidFileErrorCode(errc::no_such_file_or_directory, system_category());

    WriteHandler wHandler = boost::bind(&assertWrite, boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred,
                                        0, invalidFileErrorCode, true);

    BOOST_REQUIRE(!outputFile.isOpen());
    boost::asio::const_buffers_1 buf(data, dataSize);
    outputFile.asyncWrite(buf, wHandler);

    pause(1);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(afio_mixed_rw_test)

BOOST_AUTO_TEST_CASE(read_write)
{
    AsyncFileInOut reviewFile(datasetPath() + "no_country_for_old_men.txt", READ_WRITE_M);
    
    std::string dataToWrite("\n\nIt's one hell of a movie!!");
    boost::asio::const_buffers_1 writeBuf = boost::asio::buffer(dataToWrite);
    
    std::vector<char> readBuf(512);
    std::size_t readSize = 256;

    error_code successErrorCode;

    ReadHandler rHandler = boost::bind(&assertRead, boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred,
                                       boost::ref(readBuf), readSize, 
                                       successErrorCode, true);
    
    WriteHandler wHandler = boost::bind(&assertWrite, boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred,
                                        dataToWrite.size(), successErrorCode, true);

    BOOST_REQUIRE(reviewFile.isOpen());
    reviewFile.asyncRead(boost::asio::buffer(readBuf), rHandler, readSize);
    reviewFile.asyncWrite(writeBuf, wHandler);

    pause(1);
    reviewFile.close();
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(afio_service)

BOOST_AUTO_TEST_CASE(stop)
{
    boost::thread t(&sf::afio::join);
    BOOST_REQUIRE_NO_THROW(sf::afio::stop());
    t.join();
}

BOOST_AUTO_TEST_SUITE_END()
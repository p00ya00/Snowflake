#include <iostream>
#include "reader_server.hpp"

int main()
{
	boost::asio::io_service ios;
	ReaderServer rs(ios);

	rs.run();
	ios.run();

	std::cout << "\nExiting...\n";
	return 0;
}
#include <utility/afio.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

void readHandler(const boost::system::error_code &e, std::size_t readCount,
                 const std::vector<char> &buf, std::size_t size)
{
	std::cout << "Error: '" << e << "'\n";
	std::cout << "Transferred: " << readCount << " B\n";
	std::cout << "Buffer contents:";
	std::cout << "\n--------------------\n";
	std::copy(buf.begin(), buf.begin() + size, std::ostream_iterator<char>(std::cout));
	std::cout << "\n--------------------\n";
}


int main()
{
	sf::afio::AsyncFileIn reqs("d:\\reqs.txt", sf::afio::READ_M);
	std::vector<char> buf(1000);

	reqs.asyncRead(boost::asio::buffer(buf), 
	               boost::bind(&readHandler, boost::asio::placeholders::error, 
	                           boost::asio::placeholders::bytes_transferred,
							   boost::ref(buf), 20), 
	               20, 3);
	std::cin.get();

	return 0;
}
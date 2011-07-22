#include <iostream>
#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <utility/threadpool.hpp>
#include <fstream>
#include <istream>

class Client
{
public:
	Client(boost::mutex &m, std::string u, boost::asio::io_service &ios)
	:mutex(m), uri(u), socket(ios)
	{
		uri.append("\r\n");
	}

	void sendRequest()
	{
		try
		{
			boost::asio::ip::tcp::endpoint endpoint(
				boost::asio::ip::address::from_string("127.0.0.1"), 123);
			socket.open(boost::asio::ip::tcp::v4());
			socket.connect(endpoint);
			boost::asio::write(socket, boost::asio::buffer(uri));
			boost::asio::read_until(socket, buffer, "<--END-->");
			printResponse();
			socket.close();
		}
		catch(boost::system::system_error &e)
		{
			boost::mutex::scoped_lock sl(mutex);
			std::cout << e.what() << std::endl;
		}
	}

private:
	void printResponse()
	{
		boost::mutex::scoped_lock sl(mutex);
		std::string::iterator itr = uri.end();
		itr--;
		*itr-- = ' ';
		*itr = '>';
		std::cout << "<" << uri << ": " << &buffer << std::endl;
	}

	boost::mutex &mutex;
	boost::asio::ip::tcp::socket socket;
	boost::asio::streambuf buffer;
	std::string uri;
};

int main()
{
	boost::asio::io_service ios;
	boost::mutex coutMutex;

	typedef boost::shared_ptr<Client> ClientPtr;
	typedef std::vector<ClientPtr>    ClientVector;
	typedef std::vector<std::string>  UriVector;

	UriVector uris;
	//populate uris
	//uris.push_back("envy.txt.txt");
	//uris.push_back("ma_man.ghi");
	//uris.push_back("bjork.txt.txt");
	//uris.push_back("robert_cray.txt.txt");
	///////////////////////////////////////
	std::ifstream file("./reqs.txt", std::ios::in);
	if(!file)
	{
		std::cout << "Can not open reqs.txt\n";
		exit(1);
	}
	char line[100];
	int i = 1;
	while(!file.eof())
	{
		file.getline(line, 100, '\n');
		uris.push_back(line);
		if(i++ == 150) break;
	}
	file.close();

	ClientVector clients;
	for(UriVector::const_iterator u = uris.begin();
		u != uris.end();
		++u)
	{
		clients.push_back(ClientPtr(new Client(coutMutex, *u, ios)));
	}

	sf::utility::Threadpool<> threadPool(50);
	for(ClientVector::const_iterator c = clients.begin();
		c != clients.end();
		++c)
	{
		threadPool.schedule(boost::bind(&Client::sendRequest, boost::ref(**c)));
		//boost::this_thread::sleep(boost::posix_time::seconds(1));
	}

	//ClientVector::const_iterator c = clients.begin();
	//(*c)->sendRequest();

	//std::cout << "\nPress any key to exit...";
	std::cin.get();

	return 0;
}
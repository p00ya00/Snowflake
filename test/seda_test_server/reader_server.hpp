/***********************************************************************************
 * This is an example to show how to create an staged event-driven application,
 * using the base general SEDA implementation provided in sf::seda.
 * ReaderServer is a simple TCP server which reads a one-line request, which is
 * a txt file name and if found, sends back the content of that file to the client.
 * If the file was not found an error will be sent back.
 * ReaderServer has three events REQUEST_EV, SEND_RESP_EV and SEND_ERR_EV, and
 * four stages:
 * 1) READ_REQ_STG: reading a request from the client.
 * 2) MAP_STG: mapping the requested file to the served directory, if requested
 *    file is present, an event will be sent to content generation stage, otherwise
 *    an event will be generated indicating that an error response should be sent
 *    back.
 * 3) GEN_CONT_STG: generates required content.
 * 4) SEND_STG: sends back response or error reply to the client.
 **********************************************************************************/
#ifndef APP_HPP
#define APP_HPP

#define DEBUG_FLG

#include <seda.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <istream>
#include <ostream>
#include <fstream>
#include <locale>

boost::mutex coutMtx;

void debugLog(const char *msg)
{
#ifdef DEBUG_FLG
	boost::mutex::scoped_lock sl(coutMtx);
	std::cout << msg << std::endl;
#endif
}

enum EventType {REQUEST_EV = 1, SEND_RESP_EV = 2, SEND_ERR_EV = 3};

enum StageType {READ_REQ_STG = 1, MAP_STG = 2, GEN_CONT_STG = 3, SEND_STG = 4};

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

/////////////////////////////////////////////////////////////////////

class RequestEvent: public sf::seda::Event
{
public:
	RequestEvent(SocketPtr s)
	:Event(REQUEST_EV), peer(s)
	{}
	
	SocketPtr peer;

	boost::asio::streambuf buffer;
	
	std::string uri;
};

/////////////////////////////////////////////////////////////////////

class SendRespEvent: public sf::seda::Event
{
public:
	SendRespEvent(SocketPtr s)
	:Event(SEND_RESP_EV), peer(s)
	{}

	SocketPtr peer;
	
	boost::asio::streambuf resp;
};

/////////////////////////////////////////////////////////////////////

class SendErrorEvent: public sf::seda::Event
{
public:
	SendErrorEvent(SocketPtr s)
	:Event(SEND_ERR_EV), peer(s)
	{}
	
	SocketPtr peer;

	boost::asio::streambuf error;
};

/////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<RequestEvent> RequestEvPtr;

typedef boost::shared_ptr<SendRespEvent> SendRespEvPtr;

typedef boost::shared_ptr<SendErrorEvent> SendErrorEvPtr;

/////////////////////////////////////////////////////////////////////

class ReadReqStage: public sf::seda::Stage
{
public:
	ReadReqStage(size_t psize)
	:sf::seda::Stage(psize)
	{}

	virtual void eventHandler(sf::seda::EventPtr ep)
	{
		debugLog("Handling a read request event.");
		RequestEvPtr rep = boost::static_pointer_cast<RequestEvent>(ep);
		boost::asio::async_read_until(*(rep->peer), rep->buffer, "\r\n",
			boost::bind(&ReadReqStage::handleRead, this,
			rep, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

private:
	void handleRead(RequestEvPtr rep, const boost::system::error_code &error, 
					size_t bytes_transferred)
	{
		if(!error)
		{
			debugLog("An async read completed.");
			std::string line;
			std::istream request(&(rep->buffer));
			
			std::getline(request, line);
			rep->uri = line;
			enqueue(MAP_STG, rep);
		}
	}
};

/////////////////////////////////////////////////////////////////////

class MappingStage: public sf::seda::Stage
{
public:
	MappingStage(size_t psize)
	:sf::seda::Stage(psize)
	{}

	virtual void eventHandler(sf::seda::EventPtr ep)
	{
		debugLog("Handling a map request event.");
		RequestEvPtr rep = boost::static_pointer_cast<RequestEvent>(ep);
		rep->uri.insert(0, "./serve/");		//Win   : "D:\\serve\\"
							//Posix : "./serve/"
		std::basic_string<char>::iterator itr = rep->uri.end();
		itr--;
		std::locale loc("");
		while(!std::isalnum(*itr, loc))
			*itr-- = 0;

		debugLog("Requested URI:");
		debugLog(rep->uri.c_str());

		boost::filesystem3::path p(rep->uri.c_str());

		if(boost::filesystem3::exists(p) && boost::filesystem3::is_regular_file(p))
		{
			debugLog("Map stage is enqueuing an event in gen_cont stage.");
			enqueue(GEN_CONT_STG, rep);
		}
		else
		{
			debugLog("Map stage is enqueuing an error event in send stage.");
			SendErrorEvPtr sep(new SendErrorEvent(rep->peer));
			enqueue(SEND_STG, sep);
		}
	}
};

/////////////////////////////////////////////////////////////////////

class ContentGenStage: public sf::seda::Stage
{
public:
	ContentGenStage(size_t psize)
	:sf::seda::Stage(psize)
	{}

	virtual void eventHandler(sf::seda::EventPtr ep)
	{
		debugLog("Handling a content generation event.");
		
		RequestEvPtr rep = boost::static_pointer_cast<RequestEvent>(ep);
		std::ifstream file(rep->uri.c_str(), std::ios::in);
		if(!file)
		{
			debugLog("Requested file can not be opened!");
			SendErrorEvPtr sep(new SendErrorEvent(rep->peer));
			enqueue(SEND_STG, sep);
		}
		else
		{
			SendRespEvPtr srep(new SendRespEvent(rep->peer));
			std::ostream respStream(&srep->resp);
			char c;
			file.get(c);
			while(!file.eof())
			{
				respStream << c;
				file.get(c);
			}
			respStream << "<--END-->";
			enqueue(SEND_STG, srep);
		}
		file.close();
	}
};

/////////////////////////////////////////////////////////////////////

class SendStage: public sf::seda::Stage
{
public:
	SendStage(size_t psize)
	:sf::seda::Stage(psize)
	{}

	virtual void eventHandler(sf::seda::EventPtr ep)
	{
		debugLog("Handling a send event.");
		if(ep->getType() == SEND_RESP_EV)
		{
			debugLog("Sending back response.");
			SendRespEvPtr rp = boost::static_pointer_cast<SendRespEvent>(ep);

			boost::asio::async_write(*(rp->peer), rp->resp, 
				boost::bind(&SendStage::handleWrite, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else if(ep->getType() == SEND_ERR_EV)
		{
			debugLog("Sending back error.");
			SendErrorEvPtr erp = boost::static_pointer_cast<SendErrorEvent>(ep);

			std::ostream bufStream(&(erp->error));
			bufStream << "ERROR! Not Found!\r\n<--END-->";

			boost::asio::async_write(*(erp->peer), erp->error, 
				boost::bind(&SendStage::handleWrite, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			debugLog("ERROR! Can not handle unknown event in the Send stage.");
		}
	}

private:
	void handleWrite(const boost::system::error_code &error, 
					size_t bytes_transferred)
	{
		if(error)
		{
			debugLog("ERROR! Could not send response to client.");
		}
	}
};

/////////////////////////////////////////////////////////////////////

class ReaderServer: public sf::seda::SedaApp
{
public:
	ReaderServer(boost::asio::io_service &ios)
	:acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 123))
	{}

	virtual void initialize()
	{
		sf::seda::StagePtr readReqStage(new ReadReqStage(1));
		sf::seda::StagePtr mapStage(new MappingStage(2));
		sf::seda::StagePtr genStage(new ContentGenStage(6));
		sf::seda::StagePtr sendStage(new SendStage(1));

		addStage(READ_REQ_STG, readReqStage);
		addStage(MAP_STG, mapStage);
		addStage(GEN_CONT_STG, genStage);
		addStage(SEND_STG, sendStage);
	}

	virtual void start()
	{
		SocketPtr socketPtr(new boost::asio::ip::tcp::socket(acceptor.get_io_service()));
		RequestEvPtr reqEvPtr(new RequestEvent(socketPtr));

		acceptor.async_accept(*(reqEvPtr->peer), 
			boost::bind(&ReaderServer::handleAccept, this,
			boost::asio::placeholders::error, reqEvPtr));
	}

private:
	void handleAccept(const boost::system::error_code &error, RequestEvPtr reqEvPtr)
	{
		if(!error)
		{
			enqueue(READ_REQ_STG, reqEvPtr);
		}
		
		SocketPtr socketPtr(new boost::asio::ip::tcp::socket(acceptor.get_io_service()));
		RequestEvPtr newReqEvPtr(new RequestEvent(socketPtr));
		
		acceptor.async_accept(*(newReqEvPtr->peer), 
			boost::bind(&ReaderServer::handleAccept, this,
			boost::asio::placeholders::error, newReqEvPtr));
	}

	boost::asio::ip::tcp::acceptor acceptor;
};

#endif
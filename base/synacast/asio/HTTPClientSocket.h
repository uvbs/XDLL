
#pragma once

#include <synacast/asio/socket.h>
#include <boost/asio/streambuf.hpp>


class HTTPClientSocket : public TCPClientSocket
{
public:
	HTTPClientSocket();

public:
	bool HttpGet(const string& url);

	// void RawRequest( const string & const host, unsigned short port, const string & const rawRequest );
	void RawRequest( unsigned long ip, unsigned short port, const string & rawRequest );
	virtual bool Close();

	//virtual void OnTimerElapsed(Timer* sender);

protected:
	void HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

	void HandleConnect(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	void HandleWriteRequest(shared_ptr<boost::asio::streambuf> sbuf, const boost::system::error_code& err);
//	void HandleReadStatusLine(shared_ptr<boost::asio::streambuf> sbuf, const boost::system::error_code& err, size_t bytes_transferred );
//	void HandleReadHeaders(shared_ptr<boost::asio::streambuf> sbuf, const boost::system::error_code& err, size_t bytes_transferred );

	//// by liwei
	//void HandleReadHeader( const boost::asio::error& err );
	void HandleReadContent(shared_ptr<boost::asio::streambuf> sbuf, const boost::system::error_code& err, size_t bytes_transferred );

	void BeginRequest( const string & domain, const unsigned short port );
	bool ParseUrl2Header( const string & url, string & domain, unsigned short & port );
	bool ParseStatusLine( const string & first_line, string & version, unsigned int & code, string & message);

	void OnHttpGet(shared_ptr<boost::asio::streambuf> responseBody);
	void OnHttpGetFail(int nErrCode);

	void BeginConnect( boost::asio::ip::tcp::resolver::iterator endpoint_iterator );
protected:
	// timers
	void HandleResolveTimeout(const boost::system::error_code& err);
	void HandleConnectTimeout( const boost::system::error_code & err );
	void HandleReceiveTimeout(const boost::system::error_code& err);

	template<typename TimeoutHandler>
	void StartTimer( boost::asio::deadline_timer & timer, size_t timeout, TimeoutHandler handler )
	{
		timer.expires_from_now( boost::posix_time::milliseconds(timeout) );
		timer.async_wait( handler );
	}
	void CancelTimer( boost::asio::deadline_timer & timer )
	{
		timer.cancel();
	}

	//virtual void OnConnect(const error_code& error);

protected:
	boost::asio::ip::tcp::endpoint endpoint_;

	shared_ptr<boost::asio::streambuf> request_;
//	shared_ptr<boost::asio::streambuf> response_header_;
//	shared_ptr<boost::asio::streambuf> response_;
	//	boost::asio::streambuf total_respose_;

	//string header_;

	boost::asio::ip::tcp::resolver resolver_;
	//	boost::asio::deadline_timer http_timer_;


	//std::vector<byte>	response_header_;

	//std::vector<byte>	response_data_;
	//std::size_t			received_pos_;

	//string	domain_;
	//unsigned short port_;

};




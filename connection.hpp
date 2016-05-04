#pragma once
#include <iostream>
#include <memory>
#include <utility>
#include <atomic>
#include <boost/asio.hpp>
#include "common.h"

using boost::asio::ip::tcp;

class connection : public std::enable_shared_from_this<connection>, private boost::noncopyable
{
public:
	connection(boost::asio::io_service& io_service) : socket_(io_service)
	{
	}

	void start()
	{
		do_read();
	}

	tcp::socket& socket()
	{
		return socket_;
	}

private:
	void do_read()
	{
		auto self(this->shared_from_this());
		boost::asio::async_read(socket_, boost::asio::buffer(data_, max_length), [this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				//a successful request, can be used to statistic qps
				do_write();
			}
			else
			{
				//log
				return;
			}
		});
	}

	void do_write()
	{
		auto self(this->shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				g_succeed_count++;
				do_read();
			}
			else
			{
				//log
				return;
			}
		});
	}

	tcp::socket socket_;
	enum { max_length = 1 };
	char data_[max_length];
};


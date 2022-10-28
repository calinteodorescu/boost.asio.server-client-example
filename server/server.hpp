#pragma once

#include <iostream>
#include <boost/asio.hpp>

class session : public std::enable_shared_from_this< session >
{
public:
    session( boost::asio::ip::tcp::socket socket )
    :   m_socket( std::move( socket ) )
    {
    }

    void run( ) 
    {
        wait_for_request( );
    }

private:

    void wait_for_request( ) 
    {
        auto self( shared_from_this( ) );

        boost::asio::async_read_until
        ( m_socket,
          m_buffer,
          "\0",
          [ this, self ]( boost::system::error_code ec, 
                          std::size_t               /**/
                        )
          {
              if ( ! ec )
              {
                  std::string data{ std::istreambuf_iterator< char >( & m_buffer ),
                                    std::istreambuf_iterator< char >( )
                                  };
                  std::cout << data << std::endl;

                  wait_for_request( );
              }
              else 
              {
                  std::cout << "error: " << ec << std::endl;;
              }
          }            
        );
    }

private:

    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf       m_buffer;
};

class server
{
public:

    server( boost::asio::io_context& io_context, 
            short                    port
          ) 
    :   m_acceptor( io_context, 
                    boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4( ),
                                                    port
                                                  )
                  )
    {
        do_accept( );
    }

private:

    void do_accept( void )
    {
        m_acceptor.async_accept
        ( [ this ]( boost::system::error_code    ec,
                    boost::asio::ip::tcp::socket socket
                  )
            {
                if ( ! ec )
                {
                    std::cout << "creating session on: " 
                                << socket.remote_endpoint( ).address( ).to_string( )
                                << ":"
                                << socket.remote_endpoint( ).port( )
                                << '\n'
                    ;

                    std::make_shared< session >( std::move( socket ) )->run( );
                }
                else
                {
                    std::cout << "error: " << ec.message() << std::endl;
                }

                do_accept( );
            }
        );
    }

private: 

    boost::asio::ip::tcp::acceptor m_acceptor;
};
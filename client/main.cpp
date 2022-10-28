
#include <iostream>
#include <string>

#include <boost/asio.hpp>

int main( int   argc,
          char* argv[ ]
        )
{
    boost::asio::io_context io_context;
    
    boost::asio::ip::tcp::socket   socket  ( io_context );
    boost::asio::ip::tcp::resolver resolver( io_context );

    boost::asio::connect( socket,
                          resolver.resolve( "127.0.0.1", 
                                            "25000"
                                          )
                        );
    for( ;; )
    {
        std::string data{ "some client data ..." };

        auto result = boost::asio::write( socket, 
                                          boost::asio::buffer( data )
                                        );
    
        std::cout << "data sent: " << data.length( ) << '/' << result << std::endl;

        std::cout << "? "; char c; std::cin >> c;
    }

    boost::system::error_code ec;

    socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both,
                     ec
                   );

    socket.close( );

    return 0;
}
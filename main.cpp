/*

    Copyright (C) 2017 Tadeusz Puźniakowski

    This file is part of mhttp.

    This file is an example usage of Http class. You can use it like a part of documentation, you can
    copy, modify and incorporate in other projects without any limitation.


*/

#include <iostream>
#include <sstream>
#include <string>
#include <http.hpp>
#include <chrono>
#include <thread>
#include <fstream>


using namespace tp::http;
using namespace std;

int main ( int argc, char **argv ) {
	if ( argc > 1 ) {
		if ( std::string( argv[1] ) == "-h" ) {
			std::cout << "Example program for mhttp library." << std::endl;
		}
	}
	{
		Http srv( "localhost", 8090, 1024 );
		bool loggedin = false;

		srv.filter_GET( "/", [&]( Request & req )->void {
			req.queryString = "/index.html";
		} );

		srv.filter_GET( "/main.cpp", [&]( Request & req )->void {
			if ( !loggedin ) req.queryString = "/";
		} );

		srv.GET( "/login", [&loggedin]( Request & )->Response {
			loggedin = true;
			return ResponseFactory::response( "Logged in :D" );
		} );

		srv.GET( "/hello", []( Request & req )->Response {
			std::stringstream ss;
			ss << "<p>Hello world!!</p><table>";
			for ( auto p : req.getParams() ) ss << "<tr><td>" << p.first << "</td><td>" << p.second << "<td></tr>\r\n";
			ss << "</table>";
			return ResponseFactory::response( ss.str(), 200, "OK" );
		} );

		srv.GET( "/(.*)", getStaticFileHandler( "", true ) );

		std::thread t( [&]() {
			srv.start();
		} );
		t.detach();

		// just for test purposes:
		if ( 1 == 2 ) { // you can execute simple query
			Request req;
			req.method = "GET";
			req.proto = "HTTP/1.1";
			req.queryString = "/hello?witaj=januszu";
			req.remoteAddress = "localhost:8090";
			Response res = Http::doHttpQuery( req );
			std::cout << res << std::endl;

		}

		std::cout << "---server " << VER << " started. type some text to exit --" << std::endl;
		std::string i;
		std::cin >> i;
		std::cout << "Closing server" << std::endl;
		srv.stop();
	}
	std::cout << "Closing server - OK" << std::endl;
	return 0;

}

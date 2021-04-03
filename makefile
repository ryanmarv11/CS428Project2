all: TCPServer.cpp TCPClientX.cpp TCPClientY.cpp
	g++ -o TCPServer TCPServer.cpp
	g++ -o TCPClientX TCPClientX.cpp
	g++ -o TCPClientY TCPClientY.cpp

clean:
	rm -rf TCPServer TCPClientX TCPClientY	

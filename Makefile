server: server.cpp
	g++ server.cpp -o server -std=c++11 -lpthread

client: client.cpp
	g++ client.cpp -o client -std=c++11 -lpthread

all: server.cpp client.cpp
	g++ server.cpp -o server -std=c++11 -lpthread
	g++ client.cpp -o client -std=c++11 -lpthread
	gnome-terminal -e 'sh -c ./server'
	gnome-terminal -e 'sh -c ./client'
	gnome-terminal -e 'sh -c ./client'
	gnome-terminal -e 'sh -c ./client'
	gnome-terminal -e 'sh -c ./client'


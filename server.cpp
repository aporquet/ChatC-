#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <pthread.h>
#include <thread>

using namespace std;

/*void *thread_listen_connection_function(void *arg){
	cout << "Ecoute de connection ! " << endl;

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	while(true) {
		sockaddr_in newSockAddr;
                socklen_t newSockAddrSize = sizeof(newSockAddr);

                int newSd = accept(serverSocket, (sockaddr *)&newSockAddr, &newSockAddrSize);
                if(newSd < 0){
                        cerr << "Erreur tentative de connexion !" << endl;
                } else {
                        cout << "Connexion client !" << endl;
                }
      }

	(void) arg;
    	pthread_exit(NULL);
}*/

void thread_listen_connection_function(int serverSocket) {
	cout << "Attente des connexion clients ... : " << endl;
	
	int numberRequest = 10;
	listen(serverSocket, numberRequest);

        while(true) {
                sockaddr_in newSockAddr;
                socklen_t newSockAddrSize = sizeof(newSockAddr);

                int newSd = accept(serverSocket, (sockaddr *)&newSockAddr, &newSockAddrSize);
                if(newSd < 0){
                        cerr << "Erreur tentative de connexion !" << endl;
                } else {
                        cout << "Connexion client !" << endl;
                }
       }
}

int main() {
	//thread thread_connection (thread_listen_connection);
	//thread_connection.join();

	int port = 5555;
	int msg_max_length = 100;

	sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0) {
		cerr << "Erreur initialisation socket" << endl;
	} else {
		cout << "Socket initialisation..." << endl;
	}

	int socketStatus = bind(serverSocket, (struct sockaddr*) &addr, sizeof(addr));
	if(socketStatus < 0) {
		cerr << "Erreur status adresse" << endl;
		return EXIT_FAILURE;
	} else {
		cout << "Socket initialisé !" << endl;
	}

	thread thread_listen_connection (thread_listen_connection_function, serverSocket);
	thread_listen_connection.join();

/*
	cout << "Attente des connexion clients ..." << endl;
        listen(serverSocket, numberRequest);

	while(true) {
                sockaddr_in newSockAddr;
                socklen_t newSockAddrSize = sizeof(newSockAddr);

                int newSd = accept(serverSocket, (sockaddr *)&newSockAddr, &newSockAddrSize);
                if(newSd < 0){
                        cerr << "Erreur tentative de connexion !" << endl;
                } else {
			thread thread_connectionaaa (thread_listen_connection);
                }
       }*/

//	if(pthread_create(&thread_listen_connection, NULL, thread_listen_connection_function, "A") == -1) {
//		perror("pthread_create");
//		return EXIT_FAILURE;
  //  	}

	/*char msg[msg_max_length];         
	while(true) {
          	string data;
             	getline(cin, data);
             	memset(&msg, 0, sizeof(msg));
               	strcpy(msg, data.c_str());

               	if(strcmp(msg, "exit") == 0) {
			cout << "Arrêt du serveur ..." << endl;
		}
       }*/

	cout<< "Coucou" << endl;

	return 1;
}


#include <map>
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

void thread_listen_msg_function (int newSd) {
	cout << "Ecoute msg" << endl;
	while(true) {
		char msg[1500];
		int bytesRead, bytesWritten = 0;

		memset(&msg, 0, sizeof(msg));
	        bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
        	if(!strcmp(msg, "exit")){
            		cout << "Client has quit the session" << endl;
			break;
        	} else {
			cout << "Client : "  << msg << endl;
		}
	}
}

void thread_listen_connection_function(int serverSocket, int *allClient, int *countClient) {
	while(true) {
                sockaddr_in newSockAddr;
                socklen_t newSockAddrSize = sizeof(newSockAddr);

                int newSd = accept(serverSocket, (sockaddr *)&newSockAddr, &newSockAddrSize);
                if(newSd < 0){
                        cerr << "Erreur tentative de connexion !" << endl;
                } else {
			allClient[*countClient] = newSd;
			*countClient = *countClient +1;

			cout << "Nombre de Client : "  << *countClient << endl;
			for (int i=0; i < *countClient; i++) { cout << allClient[i] << endl; }

//                        cout << "Connexion client !" << endl;

			char pseudo[50];
                	int bytesRead, bytesWritten = 0;

                	memset(&pseudo, 0, sizeof(pseudo));
                	bytesRead += recv(newSd, (char*)&pseudo, sizeof(pseudo), 0);

			cout << pseudo << " vient de se connecter !" << endl;
			thread thread_listen_msg (thread_listen_msg_function, newSd);
                	thread_listen_msg.detach();
		}
       }
}

int main() {
	int port = 5555;
	int numberRequest = 10;
	int* allSd;
	int allClient[20] = {0};
	int countClient = 0;

	sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0) {
		cerr << "Erreur initialisation serveur" << endl;
		return EXIT_FAILURE;
	}

	int socketStatus = bind(serverSocket, (struct sockaddr*) &addr, sizeof(addr));
	if(socketStatus < 0) {
		cerr << "Erreur status adresse" << endl;
		return EXIT_FAILURE;
	} else {
		cout << "Serveur initialisé !" << endl;

		listen(serverSocket, numberRequest);

	        thread thread_listen_connection (thread_listen_connection_function, serverSocket, allClient, &countClient);
        	thread_listen_connection.join();
	}

	return 1;
}


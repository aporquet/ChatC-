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

void thread_listen_msg_client_function (int currentClientId, int *allClient, int *countClient) {
	while(true) {
		char message[1500];
		int bytesRead, bytesWritten = 0;

		memset(&message, 0, sizeof(message));
	        bytesRead += recv(currentClientId, (char*)&message, sizeof(message), 0);

		for (int index = 0; index < *countClient; index++) {
			if (allClient[index] != currentClientId) {
				send(allClient[index], message, sizeof(message), 0);
			}
		}
	}
}

void thread_listen_client_connection_function(int serverId, int *allClient, int *countClient) {
	while(true) {
                sockaddr_in newSockAddr;
                socklen_t newSockAddrSize = sizeof(newSockAddr);

                int newClientId = accept(serverId, (sockaddr *)&newSockAddr, &newSockAddrSize);
                if(newClientId < 0){
                        cerr << "Erreur tentative de connexion !" << endl;
                } else {
			allClient[*countClient] = newClientId;
			*countClient = *countClient +1;

			char pseudo[50];
                	int bytesRead, bytesWritten = 0;

                	memset(&pseudo, 0, sizeof(pseudo));
                	bytesRead += recv(newClientId, (char*)&pseudo, sizeof(pseudo), 0);

			cout << pseudo << " vient de se connecter !" << endl;

			thread thread_listen_client_msg (thread_listen_msg_client_function, newClientId, allClient, countClient);
                	thread_listen_client_msg.detach();
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

	int serverId = socket(AF_INET, SOCK_STREAM, 0);
	if(serverId < 0) {
		cerr << "Erreur initialisation serveur" << endl;
		return EXIT_FAILURE;
	}

	int socketStatus = bind(serverId, (struct sockaddr*) &addr, sizeof(addr));
	if(socketStatus < 0) {
		cerr << "Erreur status adresse" << endl;
			return EXIT_FAILURE;
		} else {
			cout << "Serveur initialisé !" << endl;

			listen(serverId, numberRequest);

	        	thread thread_listen_client_connection (thread_listen_client_connection_function, serverId, allClient, &countClient);
        		thread_listen_client_connection.join();
	}

	return EXIT_SUCCESS;
}


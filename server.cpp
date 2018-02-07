#include <iostream>
#include <string.h>
#include <netdb.h>
#include <thread>

using namespace std;

void thread_listen_msg_client_function (int currentClientId, int *allClient, int *countClient) {
	while(true) {
		char message[2000];
		int bytesRead, bytesWritten = 0;

		memset(&message, 0, sizeof(message));
	        bytesRead += recv(currentClientId, (char*)&message, sizeof(message), 0);

		/* If not exit, send message for all user Else remove user and exit this thread */
		if(strcmp(message, "exit")) {
			for (int index = 0; index < *countClient; index++) {
				if (allClient[index] != currentClientId) {
					send(allClient[index], message, sizeof(message), 0);
				}
			}
		} else {
			*countClient = *countClient - 1;
			break;
		}
	}
}

void thread_listen_client_connection_function(int serverId, int *allClient, int *countClient) {
	while(true) {
                sockaddr_in newSockAddr;
                socklen_t newSockAddrSize = sizeof(newSockAddr);

                int newClientId = accept(serverId, (sockaddr *)&newSockAddr, &newSockAddrSize);

		/* If connection, add one client and save clientId. Add new thread for each new client to listen his message */
                if(newClientId < 0){
                        cerr << "Erreur tentative de connexion !" << endl;
                } else {
			allClient[*countClient] = newClientId;
			*countClient = *countClient +1;

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

	/* Init socket config */
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

			/* Thread Listen connection */
	        	thread thread_listen_client_connection (thread_listen_client_connection_function, serverId, allClient, &countClient);
        		thread_listen_client_connection.join();
	}

	return EXIT_SUCCESS;
}


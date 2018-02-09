#include <iostream>
#include <string.h>
#include <netdb.h>
#include <thread>
#include <fstream>

using namespace std;

/* Get current time "hh:mm:ss" in result */
void getCurrentTime(char* result) {
    time_t secondes = time(NULL);
    struct tm *instant = localtime(&secondes);

    strcpy(result, to_string(instant->tm_hour).c_str());
    strcat(result, ":");
    strcat(result, to_string(instant->tm_min).c_str());
    strcat(result, ":");
    strcat(result, to_string(instant->tm_sec).c_str());
}

void thread_listen_msg_client_function (int currentClientId, int *allClient, int *countClient) {
	ofstream file;
      	file.open ("logs.txt", ofstream::out | ofstream::app);

	while(true) {
		char message[2000];
		int bytesRead, bytesWritten = 0;

		memset(&message, 0, sizeof(message));
	        bytesRead += recv(currentClientId, (char*)&message, sizeof(message), 0);

		char final_message[2000];
		char currentTime[10];

		getCurrentTime(currentTime);
		strcpy(final_message, currentTime);
		strcat(final_message, " ");
		strcat(final_message, message);

		/* If not exit, send message for all user Else remove user and exit this thread */
		if(strcmp(message, "exit")) {
			file << final_message << endl;
			for (int index = 0; index < *countClient; index++) {
//				if (allClient[index] != currentClientId) {
					send(allClient[index], final_message, sizeof(final_message), 0);
//				}
			}
		} else {
			*countClient = *countClient - 1;
			break;
		}
	}

	file.close();
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

int main(int argc, char *argv[]) {
	int defaultPort = 5555;

	int port = argc >= 2 ? atoi(argv[1]) : defaultPort;
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
			cout << "Serveur initialisé sur le port " << port << " !" << endl;

			listen(serverId, numberRequest);

			/* Thread Listen connection */
	        	thread thread_listen_client_connection (thread_listen_client_connection_function, serverId, allClient, &countClient);
        		thread_listen_client_connection.join();
	}

	return EXIT_SUCCESS;
}


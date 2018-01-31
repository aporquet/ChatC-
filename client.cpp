#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

int main(){
	const char* serverIp = "localhost";

	int port = 5555;
	int msg_max_length = 1500;

	struct hostent* host = gethostbyname(serverIp);

	sockaddr_in sendSocketAddress;
	bzero((char*)&sendSocketAddress, sizeof(sendSocketAddress));

	sendSocketAddress.sin_family = AF_INET;
	sendSocketAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSocketAddress.sin_port = htons(port);

	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	cout << "Entre ton pseudo : ";
	char pseudo[50];
	string data_pseudo;

	getline(cin, data_pseudo);
	memset(&pseudo, 0, sizeof(pseudo));
	strcpy(pseudo, data_pseudo.c_str());

	int connectionStatus = connect(clientSocket, (sockaddr*) &sendSocketAddress, sizeof(sendSocketAddress));

	if(connectionStatus < 0) {
		cerr << "Erreur de connexion!" << endl;
		return EXIT_FAILURE;
	} else {
		send(clientSocket, &pseudo, sizeof(pseudo), 0);

		cout << "Bonjour " << pseudo << ", connexion avec le serveur réussis !" << endl;

		char msg[msg_max_length];
		while(true) {
			string data;
			getline(cin, data);
			memset(&msg, 0, sizeof(msg));
			strcpy(msg, data.c_str());

			if(!strcmp(msg, "exit")){
				send(clientSocket, "exit", sizeof(msg), 0);
				break;
			} else {
				send(clientSocket, &msg, sizeof(msg), 0);
			}
		}
	}

	cout << "Deconnecté" << endl;

	return 1;
}


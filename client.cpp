#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <fstream>
#include <thread>

using namespace std;

void getCurrentTime(char* result) {
    time_t secondes = time(NULL);
    struct tm *instant = localtime(&secondes);

    strcpy(result, to_string(instant->tm_hour).c_str());
    strcat(result, ":");
    strcat(result, to_string(instant->tm_min).c_str());
    strcat(result, ":");
    strcat(result, to_string(instant->tm_sec).c_str());
}

void thread_listen_msg_function(int serverId) {
	while(true) {
		char message[2000];
		int bytesRead, bytesWritten = 0;

		memset(&message, 0, sizeof(message));
		bytesRead += recv(serverId, (char*)&message, sizeof(message), 0);

		cout << message << endl;
	}
}

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

	int serverId = socket(AF_INET, SOCK_STREAM, 0);

	cout << "Entre ton pseudo : ";
	char pseudo[50];
	string data_pseudo;

	getline(cin, data_pseudo);
	memset(&pseudo, 0, sizeof(pseudo));
	strcpy(pseudo, data_pseudo.c_str());

	int connectionStatus = connect(serverId, (sockaddr*) &sendSocketAddress, sizeof(sendSocketAddress));

	if(connectionStatus < 0) {
		cerr << "Erreur de connexion!" << endl;
		return EXIT_FAILURE;
	} else {
		thread thread_listen_msg (thread_listen_msg_function, serverId);
                thread_listen_msg.detach();

		cout << "Bonjour " << pseudo << ", connexion avec le serveur réussis !" << endl;

                ofstream file;
                file.open ("logs.txt", ofstream::out | ofstream::app);

		char message[msg_max_length];
		char final_message[2000];
		char currentTime[10];

		getCurrentTime(currentTime);
		strcpy(final_message, currentTime);
                strcat(final_message, " ");
               	strcat(final_message, pseudo);
		strcat(final_message, " vient de se connecter !");

		send(serverId, &final_message, sizeof(final_message), 0);
              	file << final_message <<  "\n";
              	file.flush();

		while(true) {
			string data;
			getline(cin, data);
			memset(&message, 0, sizeof(message));
			strcpy(message, data.c_str());

			getCurrentTime(currentTime);

                       	strcpy(final_message, currentTime);
                      	strcat(final_message, " ");
                      	strcat(final_message, pseudo);

			if(!strcmp(message, "exit")){
				strcat(final_message, " à quitté le serveur");
			} else {
				strcat(final_message, " : " );
    				strcat(final_message, message);
			}

			send(serverId, &final_message, sizeof(final_message), 0);
                    	file << final_message <<  "\n";
			file.flush();

			if(!strcmp(message, "exit")){
				send(serverId, "exit", sizeof(message), 0);
				break;
			}
		}

		file.close();
	}

	cout << "Deconnecté !" << endl;

	return 1;
}


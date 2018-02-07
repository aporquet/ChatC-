#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <fstream>
#include <thread>

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

	/* Init sokcet config */
	sockaddr_in sendSocketAddress;
	bzero((char*)&sendSocketAddress, sizeof(sendSocketAddress));

	sendSocketAddress.sin_family = AF_INET;
	sendSocketAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	sendSocketAddress.sin_port = htons(port);

	int serverId = socket(AF_INET, SOCK_STREAM, 0);

	/* Ask and save pseudo user*/
	cout << "Entre ton pseudo : ";
	char pseudo[50];
	string data_pseudo;

	getline(cin, data_pseudo);
	memset(&pseudo, 0, sizeof(pseudo));
	strcpy(pseudo, data_pseudo.c_str());

	/* client connection to server */
	int connectionStatus = connect(serverId, (sockaddr*) &sendSocketAddress, sizeof(sendSocketAddress));

	if(connectionStatus < 0) {
		cerr << "Erreur de connexion!" << endl;
		return EXIT_FAILURE;
	} else {
		/*connection ok --> thread who listen server message */
		thread thread_listen_msg (thread_listen_msg_function, serverId);
                thread_listen_msg.detach();

		cout << "Bonjour " << pseudo << ", connexion avec le serveur réussis !" << endl;

		/* prepare logs file */
                ofstream file;
                file.open ("logs.txt", ofstream::out | ofstream::app);

		char message[msg_max_length];
		char final_message[2000];
		char currentTime[10];

		getCurrentTime(currentTime);
		strcpy(final_message, "\033[32m");
		strcat(final_message, currentTime);
                strcat(final_message, " ");
               	strcat(final_message, pseudo);
		strcat(final_message, " vient de se connecter !");
		strcat(final_message, "\033[0m");

		send(serverId, &final_message, sizeof(final_message), 0);
              	file << final_message <<  "\n";
              	file.flush();

		while(true) {
			string data;
			getline(cin, data);
			memset(&message, 0, sizeof(message));
			strcpy(message, data.c_str());

			getCurrentTime(currentTime);

			if(!strcmp(message, "exit")){
				strcpy(final_message, "\033[31m");
				strcat(final_message, currentTime);
                        	strcat(final_message, " ");
                        	strcat(final_message, pseudo);
				strcat(final_message, " à quitté le serveur \033[0m");
			} else {
				strcpy(final_message, currentTime);
                        	strcat(final_message, " ");
                        	strcat(final_message, pseudo);
				strcat(final_message, " : " );
    				strcat(final_message, message);
			}

			/* Send to server and white in logs file the format message */
			send(serverId, &final_message, sizeof(final_message), 0);
                    	file << final_message <<  "\n";
			file.flush();

			/* If exit */
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


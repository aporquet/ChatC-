#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <thread>


using namespace std;

void playSound() {
	system("paplay /usr/share/sounds/gnome/default/alerts/drip.ogg");
}

void showNotif(char *message) {
	char final_command[2100];
	char title[4] = "IRC";

	strcpy(final_command, "notify-send \"");
	strcat(final_command, title);
	strcat(final_command, "\" \"");
	strcat(final_command, message);
	strcat(final_command, "\"");

	system(final_command);
}

void thread_listen_msg_function(int serverId) {
	while(true) {
		char message[2000];
		int bytesRead, bytesWritten = 0;

		memset(&message, 0, sizeof(message));
		bytesRead += recv(serverId, (char*)&message, sizeof(message), 0);

		if(strcmp(message, "") {
			cout << message << endl;
			showNotif(message);
			playSound();
		}
	}
}

int main(int argc, char *argv[]){
	const char* defaultIp = "localhost";
	int defaultPort = 5001;

	const char* serverIp = argc >= 2 ? argv[1] : defaultIp;
	int port = argc >= 3 ? atoi(argv[2]) : defaultPort;
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

	/* Client connection to server */
	int connectionStatus = connect(serverId, (sockaddr*) &sendSocketAddress, sizeof(sendSocketAddress));

	if(connectionStatus < 0) {
		cerr << "Erreur de connexion!" << endl;
		return EXIT_FAILURE;
	} else {
		/* Connection ok --> thread who listen server message */
		thread thread_listen_msg (thread_listen_msg_function, serverId);
                thread_listen_msg.detach();

		cout << "Bonjour " << pseudo << ", connexion avec le serveur réussis !" << endl;

		char message[msg_max_length];
		char final_message[2000];
		char currentTime[10];

		strcpy(final_message, "\033[32m");
               	strcat(final_message, pseudo);
		strcat(final_message, " vient de se connecter !");
		strcat(final_message, "\033[0m");

		send(serverId, &final_message, sizeof(final_message), 0);

		while(true) {
			string data;
			getline(cin, data);
			memset(&message, 0, sizeof(message));
			strcpy(message, data.c_str());

			if(!strcmp(message, "exit")){
				strcpy(final_message, "\033[31m");
                        	strcat(final_message, pseudo);
				strcat(final_message, " à quitté le serveur \033[0m");
			} else {
                        	strcpy(final_message, pseudo);
				strcat(final_message, " : " );
    				strcat(final_message, message);
			}

			/* Send to server and white in logs file the format message */
			send(serverId, &final_message, sizeof(final_message), 0);

			/* If exit */
			if(!strcmp(message, "exit")){
				send(serverId, "exit", sizeof(message), 0);
				break;
			}
		}
	}

	cout << "Deconnecté !" << endl;

	return 1;
}


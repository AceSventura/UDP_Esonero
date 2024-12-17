/*
 * 19/11/2024
 *
 * authors: Fabio Gargaro, Alexandro Gabriele Capuano
 */
#if defined WIN32
#include <winsock.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "protocol.h"

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *errorMessage) {
	printf("%s", errorMessage);
}

// Function to generate a full numeric password
void generate_numeric(char* psw, int length) {
    for (int i = 0; i < length; i++) {
        int r = rand() % 10;
        psw[i] = '0' + r;
    }
    psw[length] = '\0';

}

// Function to generate a lowercase alpabetic password
void generate_alpha(char* psw, int length) {

    for (int i = 0; i < length; i++) {
        char r = 'a' + rand() % 26;
        psw[i] = r;
    }
    psw[length] = '\0';

}

// Function to generate a mixed (lowercases, uppercases, numerical) password
void generate_mixed(char* psw, int length) {

    psw[0] = 'A' +rand()%26;
	psw[1] = 'a' +rand()%26;
	psw[2] = rand()%10;

    for (int i = 3; i < length; i++) {
        char r = (rand() % 2) ? ('a' + rand() % 26) : ('A' + rand() % 26);
        psw[i] = r;
    }
    psw[length] = '\0';
}

// Function to generate a secure password (mixed password with special chars)
void generate_secure(char* psw, int length) {
	char special[] = "!@#$%^&*()_-+=<>?";

    // Being sure having all of the requirements types of chars
    psw[0] = 'A' + rand()%26;
    psw[1] = 'a' + rand()%26;
    psw[2] = '0' + rand()%10;
    psw[3] = special[rand() % strlen(special)];

    for (int i = 4; i < length; i++) {
    	int opt = rand()%4;
    	switch(opt){
			case 0:
				psw[i] = 'A' +rand()%26;
				break;
    		case 1:
    			psw[i] = 'a' +rand()%26;
    			break;
    		case 2:
    			psw[i] = '0' + rand()%10;
    			break;
    		case 3:
    			psw[i] = special[rand() % strlen(special)];
    			break;
    	}
    }

    psw[length] = '\0';
}

// Function to generate a secure password but without ambiguous letters
void generate_unambiguous_secure(char* psw, int length) {
	char special[] = "!@#$%^&*()_-+=<>?";
	char lowercases[] = "abcdefghjkmnpqrtuvwxy";
	char uppercases[] = "ACDEFGHJKLMNPQRTUWXY";
	char integers[] = "34679";

    // Being sure having all of the requirements types of chars
    psw[0] = uppercases[rand() % strlen(uppercases)];
    psw[1] = lowercases[rand() % strlen(lowercases)];
    psw[2] = integers[rand() % strlen(integers)];
    psw[3] = special[rand() % strlen(special)];

    for (int i = 4; i < length; i++) {
    	int opt = rand()%4;
    	switch(opt){
			case 0:
				psw[i] = uppercases[rand() % strlen(uppercases)];
				break;
    		case 1:
    			psw[i] = lowercases[rand() % strlen(lowercases)];
    			break;
    		case 2:
    			psw[i] = integers[rand() % strlen(integers)];
    			break;
    		case 3:
    			psw[i] = special[rand() % strlen(special)];
    			break;
    	}
    }

    psw[length] = '\0';
}

// function to decapsulate a string in a struct pwd_message
int parse_string_to_struct(const char *str, pwd_message *message) {
    // Usa sscanf per analizzare la stringa
    if (sscanf(str, "%c#%d", &message->type_pwd, &message->length_pwd) == 2) {
        return 0;  // Successo
    } else {
        fprintf(stderr, "Error: string format not valid.\n");
        return -1;  // Errore
    }
}


// MAIN
int main(int argc, char *argv[]) {
#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKlEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		fprintf(stderr, "Error at WSAStartup()\n");
		return 0;
	}
#endif

	srand(time(NULL));	// initialization of random

	// Socket configurations
	int my_socket;
	struct sockaddr_in server_address;
	unsigned int server_address_size = sizeof(server_address);

	/* create a UDP socket */
	if ((my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		errorhandler("Error creating socket");
		clearwinsock();
		return EXIT_FAILURE;
	}

	/* set the server address */
	struct sockaddr_in client_address;
	unsigned int client_address_length = sizeof(client_address);
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PROTO_PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* set server address */
	if ((bind(my_socket, (struct sockaddr *)&server_address,
			  sizeof(server_address))) < 0)
	{
		errorhandler("bind() failed");
		closesocket(my_socket);
		clearwinsock();
		return EXIT_FAILURE;
	}

	char buffer[BUFFMAX];
	int rcv_msg_size;
	while (1)
	{
		puts("\nServer listening...");
		/* clean buffer */
		memset(buffer, 0, BUFFMAX);
		/* receive message from client */
		if ((rcv_msg_size = recvfrom(my_socket, buffer, BUFFMAX, 0,
									 (struct sockaddr *)&client_address, &client_address_length)) < 0)
		{
			errorhandler("recvfrom() failed");
			closesocket(my_socket);
			clearwinsock();
			return EXIT_FAILURE;
		}
		struct hostent *client_host = gethostbyaddr((char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);

		printf("New request from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

		//Decapsulation the struct from the string
		pwd_message msg;
		if (parse_string_to_struct(buffer, &msg) != 0) {
			fprintf(stderr, "Error occurred in decapsulating the message");
			return EXIT_FAILURE;
		}

		// Generation of password depending by its length and type
		memset(buffer, 0, BUFFMAX);

		switch(msg.type_pwd) {
			case 'n':
				generate_numeric(buffer, msg.length_pwd);
				break;
			case 'a':
				generate_alpha(buffer, msg.length_pwd);
				break;
			case 'm':
				generate_mixed(buffer, msg.length_pwd);
				break;
			case 's':
				generate_secure(buffer, msg.length_pwd);
				break;
			case 'u':
				generate_unambiguous_secure(buffer, msg.length_pwd);
				break;

		}

		/* send a message to the server */
		if (sendto(my_socket, buffer, strlen(buffer), 0,
				   (struct sockaddr *)&client_address, sizeof(client_address)) != strlen(buffer))
		{
			errorhandler("Error sending data");
			closesocket(my_socket);
			clearwinsock();
			return EXIT_FAILURE;
		}

	}

}

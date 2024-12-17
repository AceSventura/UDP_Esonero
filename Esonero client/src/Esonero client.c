/*
 * 19/11/2024
 *
 * authors: Fabio Gargaro, Alexandro Gabriele Capuano
 */

#if defined WIN32
	#include <winsock.h>
#else
	#include <string.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#define closesocket close
#endif

#include <stdio.h>
#include "protocol.h"

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *errorMessage) {
	printf("%s", errorMessage);
}

void helpMenu(){
	printf("Password Generator Help Menu Commands:\n");
	printf("h        : show this help menu\n");
	printf("n LENGTH : generate numeric password (digits only)\n");
	printf("a LENGTH : generate alphabetic password (lowercase letters)\n");
	printf("m LENGTH : generate mixed password (lowercase letters and numbers)\n");
	printf("s LENGTH : generate secure password (uppercase, lowercase, numbers, symbols)\n");
	printf("u LENGTH : generate unambiguous secure password (no similar-looking characters)\n");
	printf("q        : quit application\n");
	printf("LENGTH must be between 6 and 32 characters\n");
	printf("Ambiguous characters excluded in 'u' option:\n");
	printf("0 O o (zero and letters O)\n");
	printf("1 l I i (one and letters l, I)\n");
	printf("2 Z z (two and letter Z)\n");
	printf("5 S s (five and letter S)\n");
	printf("8 B (eight and letter B)\n");
}

// Function to parse the input string and store the values in the pwd_message struct
int parse_input(const char* input, pwd_message* pwd_msg) {

    // Check if the input or pwd_msg are NULL
    if (input == NULL || pwd_msg == NULL) {
        fprintf(stderr, "Invalid arguments.\n");
        return -1; // Error: invalid arguments
    }

    if (strncmp(input, "h", 1) == 0){
    	helpMenu();
    	return 1;
    }
    // Use sscanf to read the input string
    int num_items = sscanf(input, "%c %d", &(pwd_msg->type_pwd), &(pwd_msg->length_pwd));

    // Check if sscanf successfully read the expected number of items
    if (num_items != 2) {
        printf("Invalid input format. Expected format: \"%%c %%d\".\n");
        return -1; // Error: input format is not correct
    }

    // Check if the password length is valid
    if (pwd_msg->length_pwd < 6 || pwd_msg->length_pwd > 32) {
        printf("ERR: Password length must be in [6-32].\n");
        return -1; // Error: invalid password length
    }
    // Check if the chars are in [n, a, m, s]
	switch(pwd_msg->type_pwd){
		case 'n':
		case 'a':
		case 'm':
		case 's':
		case 'u':
			return 0;		// if is a correct input format
		default:
			printf("ERR: Type must be any of [n, a, m, s, u].\n");
			return -1;		// if is not correct input
	}

    return 0; // Success
}

// function to encapsulate a struct in a string
void modify_string(char *str, size_t max_len, pwd_message message) {
    char buffer[50];  // Buffer temporaneo per costruire la concatenazione

    // making the string by the struct members
    snprintf(buffer, sizeof(buffer), "%c#%d", message.type_pwd, message.length_pwd);

    // Check for enough space
    strcpy(str, buffer);
}

int main(int argc, char *argv[]) {
#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	while (1) {
		char *nameServer = "passwdgen.uniba.it";
		struct hostent *host = gethostbyname(nameServer);
		if (host == NULL)
		{
			errorhandler("Error getting host");
			clearwinsock();
			return -1;
		}
		// Debugging DNS
		// printf("Server name: %s (IP: %s, port: %d)\n", nameServer, inet_ntoa(*(struct in_addr *)host->h_addr), PROTO_PORT);

		int my_socket;
		struct sockaddr_in server_address;
		unsigned int server_address_size = sizeof(server_address);
		int rcv_msg_size;

		/* create a UDP socket */
		if ((my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{
			errorhandler("Error creating socket");
			clearwinsock();
			return -1;
		}

		/* set the server address */
		memset(&server_address, 0, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(PROTO_PORT);
		server_address.sin_addr = *((struct in_addr *)host->h_addr);

		// Start user interface
		printf("Enter the type of password to generate with its length [es. n 8] (or type 'q' to quit): ");

		// Read input from user (password's type and length)
		char input[100];
		char buffer[BUFFMAX] = "";
		pwd_message pwd_msg;
		fgets(input, sizeof(input), stdin);

		// Check for exit condition
		if (strncmp(input, "q", 1) == 0) {
			// visualize blue message
			fprintf(stderr, "\033[34m%s\033[0m\n", "Exited correctly.");

			closesocket(my_socket);
			clearwinsock();
			return 0;
		}


		// Call the function to parse the input
		while (parse_input(input, &pwd_msg) != 0) {
			printf("Enter the type of password to generate with its length [es. n 8] (or type 'q' to quit): ");
			fgets(input, sizeof(input), stdin);

			// Check for exit condition
			if (strncmp(input, "q", 1) == 0) {
				// visualize blue message
				fprintf(stderr, "\033[34m%s\033[0m\n", "Exited correctly.");

				closesocket(my_socket);
				clearwinsock();
				return 0; // Exit the loop
			}
		}

		/* Encapsulating the message in string*/
	    modify_string(buffer, sizeof(BUFFMAX), pwd_msg);

		/* Send password request to the server */
		if (sendto(my_socket, buffer, strlen(buffer), 0,
				   (struct sockaddr *)&server_address, sizeof(server_address)) != strlen(buffer))
		{
			errorhandler("Error sending data");
			closesocket(my_socket);
			clearwinsock();
			return -1;
		}

		/* Receive password from the server */
		memset(buffer, 0, BUFFMAX);
		if ((rcv_msg_size = recvfrom(my_socket, buffer, BUFFMAX, 0,
									 (struct sockaddr *)&server_address, &server_address_size)) < 0)
		{
			errorhandler("Error receiving data");
			closesocket(my_socket);
			clearwinsock();
			return -1;
		}

		// Write the generated password on stdout
		printf("Password generated: %s\n", buffer);
		printf("***************************************\n");
		closesocket(my_socket);
		clearwinsock();
	}

	return 0;

} // main end

//Collin Seaman
//Lab 2
//server.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>

//A number out of 10 that defines the fraction of data to be artificially dropped from the server side

#define SERVER_DROP_RATE 3

/*
 * struct to store data and header files from the client
*/
struct Package {
	
	int seqnum;
	char checksum;
	char datadum[10];
	int nbytes;
};

/*
 * read_pkg()
 *
 * ARGS: socket value, pointer to the package struct that the data will go into, flag for recvfrom and sendto, pointer to struct to hold
 * address info of the client, size of the address info of the client, pointer to sequence number (will be incremented after data is verified)
 *
 * INFO: recieves package from client and runs checksum to ensure data integrity, then sends the sequence number of the next expected package
 * to the client. Returns the number of recieved bytes, or -1 if checksum does not match recieved checksum
*/

int readpkg(int sock, struct Package *buffer, struct sockaddr_in *serv_addr, socklen_t *addr_size, int *nextseq, FILE* fp);

/*
 * checksum()
 *
 * ARGS: pointer to data to be checksummed, size of data
 *
 * INFO: identical to client side checksum, returns a one byte checksum to be compared to recieved checksum to ensure data integrity
*/

char checksum(char *buffer, int size);

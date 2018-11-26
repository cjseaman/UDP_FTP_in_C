/* 
 * Collin Seaman
 * COEN 146
 * Lab 4
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>

//A number out of 10 that defines the fraction of data to be artificially corrupted
//set to 0 for no corruption, or 10 for 100% corruption (nothing gets sent)

#define CORRUPT_RATE 3

//A number out of 10 that defines the fraction out of 10 of data to be artificially 
//dropped from the client side

#define SEND_DROP_RATE 3

/*
 * Structure that holds the data, sequence number, and checksum that we are sending to the server
*/
struct Package {
	
	int seqnum;
	char checksum;
	char datadum[10];
	int nbytes;
};
	
/*
 * package()
 * ARGS: socket number, data to be packaged, size of the data to be packaged, pointer to
 * the structure that holds the address info for the sendto and recvfrom functions, size of that address, pointer to package struct to hold
 * data and head, pointer to ack (so it can be incremented over many calls of package())
 *
 * INFO: Takes 10 bytes of data from the buffer and puts it into a structure with its checksum and sequence number, then sends it to the 
 * server. Returns the number of bytes of data sent, not including chacksum or sequence number, or -1 if the sequence number sent by the server
 * is incorrect. Does not exit until it recieves data from the server.
*/
int package(int sockfd, char *buffer, int n, struct sockaddr* serv_addr, socklen_t addr_size, struct Package *packageo, int *ack);

/*
 * checksum()
 * ARGS: Data to be checksummed, size of the data
 *
 * INFO: XORs each character in the buffer, starting with buffer[0] and buffer[1], then XORs the result with buffer[i] until i = n,
 * then XORs the final result with buffer[0] and returns the resulting byte of data.
*/
char checksum(char *buffer, int size);

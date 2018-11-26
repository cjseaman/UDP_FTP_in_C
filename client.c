 /*	
 *	Collin Seaman
 * 	COEN 146
 * 	S+W UDP Lab
 *
 * 	Contacts a server using its ip and a port number, then passes a file name and data to the server using UDP.
 * 	Uses a package function that adds a sequence number and checksum to the data, the server will read the checksum and sequence number and return the next sequence number if the checksum checks out. The client will only move on to the next packet when that sequence number is recieved.
 * 	Also uses a timeout to account for packets and acks that are lost
 * 	The server saves the file locally and the client closes the connection.
*/

#include "client.h"

int main(int argc, char *argv[]) {
	
	if (argc != 5) {
		printf("Usage: ./client <file input name> <file output name (must be less than 10bytes)> <server ip> <port number>\n");
		return 1;
	}
	if (strlen(argv[2]) > 9) {
		printf("File Output Name must be 9 characters or shorter\n");
		return 2;
	}
	
	int i, filec = 0, bufloc = 0;
	int ack = 0;
	int sockfd = 0, nbytes = 0;
	char buffer[10];
	struct sockaddr_in serv_addr;
	socklen_t addr_size;
	char *finname = argv[1];
	char *foutname = argv[2];
	FILE *filei;
	FILE *fileo;
	struct Package packageo;
	struct timeval timeo;
	
	//set timeout interval
	timeo.tv_sec = 1;
	timeo.tv_usec = 0;

	packageo.seqnum = 0;
	
	//init mem
	memset(&buffer, '\0', sizeof(buffer));
	memset(&serv_addr, '0', sizeof(serv_addr));
	
	if((filei = fopen(finname, "r")) == NULL) {
		printf("Error reading file\n");
		return 1;
	}
	
	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 


	if(sockfd < 0) {
		printf("socket() exited with a return value of %d\n", sockfd);
		return 1;
	}

	//set address
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (atoi(argv[4]));
	
	if (inet_pton (AF_INET, argv[3], &serv_addr.sin_addr.s_addr) <= 0) {
		printf("Invalid IP or port number. Is server running?\n");
		return 1;
	}
	
	sockfd = socket (PF_INET, SOCK_DGRAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));

	//read and send, filename first
	
	strncpy(buffer, foutname, 10);
	nbytes = strnlen(buffer, 10);
	package(sockfd, buffer, nbytes, (struct sockaddr*)&serv_addr, sizeof(serv_addr), &packageo, &ack);

	nbytes = 10;
	while(nbytes >= 10) {
		memset(buffer, '\0', sizeof(buffer));
		nbytes = fread(buffer, 1, 10, filei);
		package(sockfd, buffer, nbytes, (struct sockaddr*)&serv_addr, sizeof(serv_addr), &packageo, &ack);
	}
	printf("DONE\n");
	fclose(filei);	
	close(sockfd);
		
	return 0;
}

int package (int sockfd, char *buffer, int nbytes, struct sockaddr* serv_addr, socklen_t addr_size, struct Package *packageo, int *ack) {
	
	int i;
	int tries = 0;
	
	
	do {
		tries ++;	
		packageo->checksum = checksum(buffer, nbytes);
		packageo->nbytes = nbytes;
		strncpy(packageo->datadum, buffer, 10);
		if((rand() % 10) > SEND_DROP_RATE) {
			sendto(sockfd, packageo, sizeof(struct Package), 0, serv_addr, addr_size);	
		}
		recvfrom(sockfd, ack, sizeof(int), 0, serv_addr, &addr_size);

	} while (*ack == packageo->seqnum);
	
	packageo->seqnum = ((packageo->seqnum + 1)%2);
	
	return i+1;
}

char checksum(char *buffer, int size) {
	char result = buffer[0];
	int i;
	if((rand() % 10) < CORRUPT_RATE) {
		return -1;
	}
	for(i = 1; i < size; i++) {
		result = result^buffer[i];
	}
	result = result^buffer[0];
	return result;
}
	

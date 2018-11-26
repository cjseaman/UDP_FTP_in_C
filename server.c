//Collin Seaman
//Lab 3
//server.c

#include "server.h"

int main(int argc, char *argv[]) 
{

	int sock, nextseq = 0;
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	int i, flag;
	FILE *fp;
	char filename[10];
	struct Package packi;

	if(argc != 2) 
	{
		printf("need the port number\n");
		return 1;
	}

	if(atoi(argv[1]) <= 1024) {
		printf("Error: Port number below 1024 \n");
		return 1;
	}
	
	//initialize values
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi (argv[1]));
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset((char*)serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
	memset(packi.datadum, '\0', sizeof(packi.datadum));
	addr_size = sizeof(serverStorage);

	//create socket
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		printf("socket error\n");
		return 1;
	}

	//binds and starts waiting to receive
	if(bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) 
	{
		printf("bind error\n");
		return 1;
	}

	while(1)
	{
		//flush values (so multiple files can be sent)
		memset(packi.datadum, '\0', sizeof(packi.datadum));
		i = 0;
		flag = 0;
		nextseq = 0;
		packi.nbytes = 10;
		
		//wait for and read data	
		printf("waiting for connection...\n");	
		readpkg(sock, &packi, &clientAddr, &client_addr_size, &nextseq, NULL);
		strncpy(filename, packi.datadum, 10);
		
		if((fp = fopen(filename, "w")) == NULL) 
		{
			printf("could not open file\n");
		}
		memset(filename, '\0', sizeof(filename));
			
		packi.nbytes = 10;
		
		//reads file data
		while(flag != -99) 
		{
			flag = readpkg(sock, &packi, &clientAddr, &client_addr_size, &nextseq, fp);
		}
		
		
		fclose(fp);
		printf("DONE\n");
	}
	return 0;
}

//returns checksum (or corrupted checksum)
char checksum(char *buffer, int size) 
{

	int i;
	char result = buffer[0];

	for(i = 1; i < size; i++) 
	{
		result = result ^ buffer[i];
	}
	
	result = result ^ buffer[0];
	return result;
}

//returns -99 when done
int readpkg(int sock, struct Package *buffer, struct sockaddr_in* serv_addr, socklen_t *addr_size, int *nextseq, FILE* fp) 
{

	int i;
	char check;
	
	*nextseq = (*nextseq + 1)%2;
	//runs until checksum is correct
	do {
		recvfrom(sock, buffer, sizeof(struct Package), 0, (struct sockaddr *)serv_addr, addr_size);
		
		if(checksum(buffer->datadum, buffer->nbytes) != buffer->checksum) {
			if((rand() % 10) > SERVER_DROP_RATE) {
				sendto(sock, &buffer->seqnum, sizeof(int), 0, (struct sockaddr *)serv_addr, *addr_size);
			}
		}
		
	} while((check = checksum(buffer->datadum, buffer->nbytes)) != buffer->checksum); 
	
	sendto(sock, nextseq, sizeof(int), 0, (struct sockaddr *)serv_addr, *addr_size);
	
	if(fp != NULL)
	{
		fwrite(buffer->datadum, buffer->nbytes, 1, fp);
	}
	
	if(buffer->nbytes < 10) {
		return -99;	
	}
	return 0;
}

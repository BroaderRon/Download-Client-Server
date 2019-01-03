/*
Name: Ronald Markovich
Major:Computer Science
Assignment: Download Server
Name of File: project5.c
Purpose: Client for download server
Language: C using gcc version 4.4.7
Compilation Command: gcc DLClient.c -o client
Execution Command: ./client <hostname> <optional port>
*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> 
#define defaultPort 62123
#define DLSIZE 1024

//functions
int byeSend(int);
int pwdSend(int);
int dirSend(int);
int cdSend(int,char[]);
int UI(int);
int download(int,char[]);
int processInput(char[],int);
int processSend(char[],int);

int main(int argc,char*argv[]){
	if (argc < 2 || argc > 3 ){
		printf("\nusage: %s <hostname> <optional port>\n\n", argv[0]);
		exit(1);
	}
	int newPort;// used when the user specifies a port to use
	//allocate hostEnt
	struct hostent *hostEnt = (struct hostent *)malloc(sizeof(struct hostent));
	//allocate IPaddr
	struct in_addr *IPaddr = (struct in_addr *)malloc(sizeof(struct in_addr));
	int sockfd, chk, imsgSize;
	struct sockaddr_in server; 
	server.sin_family = AF_INET;
	
	if(argc == 3){// used to check for optional port
		newPort = atoi(argv[2]);
		server.sin_port = htons(newPort);
	}
	else{
		server.sin_port = htons(defaultPort);
	}
	
	hostEnt = gethostbyname(argv[1]);
	if(hostEnt==NULL){
		herror("error in gethostbyname");
		exit(1);
	}
	
	IPaddr = (struct in_addr *)hostEnt->h_addr;
	server.sin_addr = *IPaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		perror("error in socket");
		exit(1);
	}
	
	chk = connect(sockfd, (struct sockaddr *) &server, sizeof(server));
	if(chk == -1){
		perror("error connecting to server");
		exit(-1);
	}
	
	int len=20;
	char buffer[len];//buffer for IP adress output
	char msgSize[3];
	inet_ntop(AF_INET, &(server.sin_addr), buffer, len);
	char input [256];
	processInput(input,sockfd);
	if(strcmp(input,"HELLO")== 0){
		printf("connected to address:%s\n",buffer);
			
	}
	else{
		printf("unknown message %s\n",input);	
	}
	

/*	n = pwdSend(sockfd);
	n = dirSend(sockfd);
	n = cdSend(sockfd);
	n = pwdSend(sockfd);
	n = dirSend(sockfd);
	n = byeSend(sockfd);*/
	
	int n =UI(sockfd);
	printf("disconnected from address:%s\n",buffer);
	
	close(sockfd);//close socket
	
}
/*
  Function name: byeSend
  Description: sends BYE to the server
  Parameters: sockfd- socket file descriptor
  Return Value: 0
*/
int byeSend(int sockfd){
	char msgSend[] = "003BYE";//message to send
	processSend(msgSend,sockfd);
	return 0;
}
/*
  Function name: pwdSend
  Description: sends pwd to server and recieves the results
  Parameters: sockfd- socket file descriptor
  Return Value: 0 or -1 on error
*/
int pwdSend(int sockfd){
	int n;
	char msgSend[] = "003PWD";//message to send
	processSend(msgSend,sockfd);
	int imsgSize;
	char input[256];
	char msgSize[3];
	if(n == -1){
		printf("error in send PWD: \n");
	}
	n = processInput(input,sockfd);
	if(n == -1){
		return -1;
	}
	return 0;
}
/*
  Function name: dirSend
  Description: sends the dir command to the server and recieves the results
  Parameters: sockfd- socket file descriptor
  Return Value: 0 or -1 on error
*/
int dirSend(int sockfd){
	int n;
	char msgSend[] = "004DIR";//message to send
	processSend(msgSend,sockfd);
	int imsgSize;
	char input[256];
	char msgSize[3];
	n = processInput(input,sockfd);
	if(n == -1){
		return -1;
	}
	return 0;

}
/*
  Function name: cdSend
  Description: sends cd followed by the directory name to the server recieves 0 on success and -1 on error
  Parameters: sockfd- socket file descriptor,buf- directoy name to change to
  Return Value: 0 or -1 on error
*/
int cdSend(int sockfd, char buf[]){	
	int n;
	int num = strlen(buf);
	char temp[3];
	sprintf(temp,"%d",num);
	char size[4];
	int imsgSize;
	char msgSize[3];
	memset(size,0,3);
	if(num<10){
		size[0] = '0';
		size[1] = '0';
		size[2] = temp[0];
		size[3] = '\0';
	}
	else if(num<100){
		size[0] = '0';
		size[1] = temp[0];
		size[2] = temp[1];
		size[3] = '\0';
	}
	else{
		size[0] = temp[0];
		size[1] = temp[1];
		size[2] = temp[2];
		size[3] = '\0';
	}
	char msgSend2[256];
	sprintf(msgSend2,"%s%s",size,buf);
	char msgSend[] = "002CD";//sending command
	processSend(msgSend,sockfd);
	//char msgSend2[] = "004test";//directory to change to
	processSend(msgSend2,sockfd);
	n = recv(sockfd , msgSize , 3 , 0);
	imsgSize = atoi(msgSize);//change msgSize into an int
	if(imsgSize == -1){
		
		return -1;
	}
	return 0;
}
/*
  Function name: download
  Description: sends download followed by the file name to the server recieves 
  Parameters: sockfd- socket file descriptor,buf- file name to change to
  Return Value: 0 or -1 on error
*/
int download(int sockfd, char buf[]){
	char input[512];
	int n;
	if( access( buf, F_OK ) != -1 ) {
		printf("File %s exits do you want to overwrite.(y/n)",buf);
		char temp[20];
		scanf("%s",temp);
		if((strcmp(temp,"n")== 0)){
			return -2;
		}
		else if((strcmp(temp,"y")!= 0)){
			printf("error in input: user entered %s. Need to enter(y/n)",temp);
		}

	} 
	int num = strlen(buf);
	FILE *received_file;
	char temp[3];
	sprintf(temp,"%d",num);
	char size[4];
	memset(size,0,3);
	if(num<10){
		size[0] = '0';
		size[1] = '0';
		size[2] = temp[0];
		size[3] = '\0';
	}
	else if(num<100){
		size[0] = '0';
		size[1] = temp[0];
		size[2] = temp[1];
		size[3] = '\0';
	}
	else{
		size[0] = temp[0];
		size[1] = temp[1];
		size[2] = temp[2];
		size[3] = '\0';
	}
	char msgSend2[256];
	sprintf(msgSend2,"%s%s\0",size,buf);
	char msgSend[] = "008DOWNLOAD";//sending command
	processSend(msgSend,sockfd);
	processSend(msgSend2,sockfd);
	n = processInput(input,sockfd);
	if(n == -1){
		return -1;
	}
	else if((strcmp(input,"READY") == 0))
	{
		char msgSend[] = "005READY";//sending command
		processSend(msgSend,sockfd);
	}


	
	int imsgSize,fileSize;
	char msgSize[3];

	
	n = recv(sockfd , msgSize , 3 , 0);// used to recieve the message size
	if (n == -1){
		perror("error in download:");
		return -1;
	}
	
	imsgSize = atoi(msgSize);//change msgSize into an int
	char sfileSize[imsgSize];//used to store the message
	
	n =recv(sockfd,sfileSize, imsgSize,0); //reading message
	if (n == -1){
		perror("error in download:");
		return -1;
	}
	fileSize = atoi(sfileSize);
	 received_file = fopen(buf, "w");
	 char buffer[DLSIZE];
	 while(fileSize > 0){
		memset(buffer,0,DLSIZE);
		n = recv(sockfd , buffer, DLSIZE, 0);
		if(fileSize <= 1024){
			fwrite(buffer , 1 , fileSize , received_file );	
		}
		else{
			fwrite(buffer , 1 , DLSIZE , received_file );
		}
		fileSize -= DLSIZE;
	 }
	 fclose(received_file);
}
/*
  Function name: processInput
  Description: receive message from server
  Parameters: sockfd- socket file descriptor,input- location to store message from server
  Return Value: 0 or -1 on error
*/
int processInput(char input[],int sockfd){
	int imsgSize;
	int n;
	char msgSize[3];

	
	n = recv(sockfd , msgSize , 3 , 0);// used to recieve the message size
	if (n == -1){
		perror("error in recv in process input:");
	}
	
	imsgSize = atoi(msgSize);//change msgSize into an int
	if(imsgSize == -1){
		return -1;
	}
	int totSize=0;
	while(totSize < imsgSize-1){
		n =recv(sockfd,input+totSize, imsgSize,0);
		if (n == -1){
		perror("error in recv:");
	}
		totSize+=n;
	}
	input[totSize] = '\0';
	if(strcmp(input,"HELLO")!= 0){
		printf("\n%s",input);
		printf("\n\n");	
	};
	
	return 0;
}
/*
  Function name: processSend
  Description: send message to server
  Parameters: sockfd- socket file descriptor,sending- message being sent to server
  Return Value: 0 or -1 on error
*/
int processSend(char sending[],int sockfd){
	int bytesSent=0;
	while(bytesSent< strlen(sending)){
		int n = send(sockfd,sending+bytesSent,strlen(sending),0);
		bytesSent += n;
	}

	
}
/*
  Function name: UI
  Description: provide UI for user
  Parameters: sockfd- socket file descriptor
  Return Value: 0 or -1 on error
*/
int UI(int sockfd){
	int cont = 1,n,i =0;
	while(cont == 1){
		
		char buf1[256];
		memset(buf1,0,256);
		char buf2[256];
		memset(buf2,0,256);
		printf("Please Enter the command to use or type HELP:");
		scanf("%s",buf1);
		for( i=0;i<strlen(buf1);i++){
			buf1[i] = toupper(buf1[i]);
			}
			if(strcmp(buf1,"PWD")== 0){
				
				n = pwdSend(sockfd);
				if(n==-1){
					processInput(buf2,sockfd);
				}
						
				
			}	
			else if(strcmp(buf1,"DIR") == 0){
				n = dirSend(sockfd);
				if(n==-1){
					processInput(buf2,sockfd);
				}
				
			}
			else if(strcmp(buf1,"CD") == 0){
				printf("Please Enter the directory: ");
				scanf("%s",buf2);
				n = cdSend(sockfd,buf2);
				if(n==-1){
					processInput(buf2,sockfd);
				}
			}
			else if(strcmp(buf1,"BYE") == 0){
				n = byeSend(sockfd);
				if(n==-1){
					processInput(buf2,sockfd);
				}
				cont = 0;
				
			}
			else if(strcmp(buf1,"DOWNLOAD") == 0){
				printf("Please Enter the name of file: ");
				scanf("%s",buf2);
				n = download(sockfd,buf2);
				if(n==-1){
					processInput(buf2,sockfd);
				}
			}
			else if(strcmp(buf1,"HELP") == 0){
				printf("\nPWD- enter pwd to get the current directories absolute path\n");
				printf("DIR- enter dir to list the current directory\n");
				printf("CD- enter cd and on the next line enter the directory\n");
				printf("DOWNLOAD- enter download and on the next line enter the file\n");
				printf("BYE- enter bye to disconnect from the server\n\n");
			}
		
	}
}
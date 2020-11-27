
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{

	if(argc<2){
		printf("give a url\n");
		return 0;
	}

	char *url=argv[1];
	char * token = strtok(url, "://");
	char * schema = token;
	printf("The Schema is '%s'\n", token);
	token = strtok(NULL,"://");
	char *host = token;
	printf("The rest of the url is '%s'\n", token);
	char * route = &url[strlen(schema) + 1 + strlen("://") +strlen(host)];

	int rv;
	struct addrinfo hints, *servinfo;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;	
		
	if ((rv = getaddrinfo(host, "80", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	struct sockaddr_in *addr;
	addr = (struct sockaddr_in *)servinfo->ai_addr;
	printf("the ip is = %s\n", inet_ntoa((struct in_addr)addr->sin_addr));


	int sockfd;
	if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		perror("client: socket");
		return 1;
	}

	
	if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		close(sockfd);
		perror("client: connect");
		return 1;
	}






	char getLine[512];
	snprintf(getLine, sizeof(getLine), "GET /%s HTTP/1.0\r\n", route);
	rv=send(sockfd, getLine, strlen(getLine), 0);

	if (rv == -1){
		perror("client: send");
		return 1;
	}



	char *hostLine = "Host: illinois.edu\r\n";
	rv=send(sockfd, hostLine, strlen(hostLine), 0);

	if (rv == -1){
		perror("client: send");
		return 1;
	}

	char * emptyLine = "\r\n\r\n";
	rv = send(sockfd, emptyLine, strlen(emptyLine), 0);
	if (rv == -1){
		perror("client: send");
		return 1;
	}


	char response[512]; //points to a memory which is 512 bytes long
	memset(response, 0, sizeof(response));
	rv = recv(sockfd, response, sizeof(response), 0);
	if (rv == -1){
		perror("client: send");
		return 1;
	}
	
	int length = rv;
	printf("Response: '%s'\n", response);
	FILE* out = fopen("output", "w");
	fwrite(response, 1, length, out);
	fclose(out);



}
/*
void test()
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}
*/

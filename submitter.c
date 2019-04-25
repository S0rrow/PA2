#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
	struct sockaddr_in serv_addr;
	int sock_fd;
	int colon, i, opt, s, len;
	char buffer[1024] = {0};
	char *data, *IPnPort, *ip, *port, *sID, *pw, *filename;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd <= 0) {
		perror("socket failed : ");
		exit(EXIT_FAILURE);
	}

	while ((opt = getopt(argc, argv, "n:u:k:")) != -1) {
		switch(opt) {
			case 'n':
				IPnPort = optarg;
				ip = strtok(IPnPort, ":");
				port = strtok(NULL, ":");
				printf("IP : %s, port : %s\n", ip, port);
				break;
			case 'u':
				sID = optarg;
				printf("student ID : %s\n", sID);
				break;
			case 'k':
				pw = optarg;
				filename = argv[optind];
				printf("password : %s\n", pw);
				printf("filename : %s\n", filename);
				break;
		}
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(7260);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed : ");
		exit(EXIT_FAILURE);
	}

	if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed : ");
                exit(EXIT_FAILURE);
        }

	printf("buffer : ");
	scanf("%s", buffer) ;
	data = buffer ;

	len = strlen(buffer) ;
	s = 0 ;

	while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
		data += s ;
		len -= s ;
	}
	

	shutdown(sock_fd, SHUT_WR) ;

	char buf[1024] ;
	data = 0x0 ;
	len = 0 ;
	while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
		buf[s] = 0x0 ;
		if (data == 0x0) {
			data = strdup(buf) ;
			len = s ;
		}
		else {
			data = realloc(data, len + s + 1) ;
			strncpy(data + len, buf, s) ;
			data[len + s] = 0x0 ;
			len += s ;
		}
	}
	printf(">%s\n", data); 
} 

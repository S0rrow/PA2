#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

const char pair1[2][128] = {"21700726", "csh726"};
const char pair2[2][128] = {"21600830", "dyj830"};

void child_proc(int conn) {
	char buf[1024];
	char *data = 0x0, *orig = 0x0;

	int argc = 0;
	char **argv;
	
	int len = 0;
	int s;
	while ((s=recv(conn, buf, 1023, 0))>0) {
		buf[s] = 0x0;
		if(data == 0x0) {
			data = strdup(buf);
			len = s;
		}
		else {
			data = realloc(data, len + s + 1);
			strncpy(data + len, buf, s);
			data[len+s] = 0x0;
			len += s;
		}
	}
	printf(">%s\n", data);
	
	orig = data;
	while (len > 0 && (s = send(conn, data, len, 0)) > 0) {
		data += s;
		len -= s;
	}
	shutdown(conn, SHUT_WR);
	if(orig != 0x0) free(orig);
}

int main(int argc, char const *argv[]) {
	int listen_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	char buffer[1024] = {0};

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);//IPv4, TCP, IP
	if(listen_fd == 0) {
		perror("socket failed : ");
		exit(EXIT_FAILURE);
	}

	memset(&address, '0', sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // the local host
	address.sin_port = htons(7260);//port number to other encoding

	if(bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("bind failed : ");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if(listen(listen_fd, 16) < 0) { // size of waiting queue
			perror("listen failed : ");
			exit(EXIT_FAILURE);
		}

		new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t *)&addrlen);

		if(new_socket < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		
		if(fork() > 0) {
			child_proc(new_socket);
		}
		else {
			close(new_socket);
		}
	}
}

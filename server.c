#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<pthread.h>

#define	MAXLINE	 8192  /* Max text line length */
#define LISTENQ  1024  /* Second argument to listen() */
char* questions[] = {"You study in which university?", "which course are you studying?", "what is your area of interest?", "Which degree have you registered for?"};

char* answers[] = {"DAIICT\n", "Systems Programming\n", "Embedded Systems\n", "MSc IT\n"};

int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;
	char host[MAXLINE],service[MAXLINE];
    int flags;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;            
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; 
    hints.ai_flags |= AI_NUMERICSERV;
    getaddrinfo(NULL, port, &hints, &listp);

    for (p = listp; p; p = p->ai_next) {
      
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue; 

        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,  
                   (const void *)&optval , sizeof(int));

		flags = NI_NUMERICHOST | NI_NUMERICSERV;
		getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, service, MAXLINE, flags);
        printf("host:%s, service:%s\n", host, service);

    
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; 
        close(listenfd);
    }

    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
	return -1;
    }
    return listenfd;
}

void *echo(void * p_connfd)
{   	
	void *tp;
	int *t_connfd=p_connfd;
	int connfd=*t_connfd;	
    size_t n;
    char buf[MAXLINE];
    while((n = read(connfd, buf, MAXLINE)) != 0) {
        
	buf[n] = '\0';
        printf("server received %s \n", buf);
        char temp=buf[0];
        if(temp -'0'==0) return tp;
        write(connfd, answers[temp-'0'-1], 100);	
    }
    
    return tp;
}

int main(int argc, char **argv)
{
    int listenfd, connfd[1024];
    int cnt=0;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    listenfd = open_listenfd(argv[1]);
        pthread_t t[1024];
    while (1) {
        cnt++;
	printf("Waiting for a new Client to connect\n");
        printf("Ready to connect to new client...\n");
        clientlen = sizeof(struct sockaddr_storage);
        connfd[cnt] = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
        getnameinfo((struct sockaddr *) &clientaddr, clientlen, client_hostname, 8192, client_port, 8192, 0);
        printf("Connected to host %s, port %s)\n", client_hostname, client_port);
        printf("Communication Started. \n");
        pthread_create(&t[cnt], NULL, echo, (void *) &connfd[cnt]);
    }
    exit(0);
}


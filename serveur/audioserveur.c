#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <unistd.h>

int main (){
    int fd, err;
    char msg[64];
    socklen_t len, flen;
    struct sockaddr_in addr;
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0) {perror("erreur socket");exit(0);}

    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(1234);
    addr.sin_addr.s_addr    = htonl(INADDR_ANY);

    err = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    if(err<0) {perror("erreur bind");exit(0);}
    
    flen = sizeof(struct sockaddr_in);
    len = recvfrom(fd,msg,sizeof(msg),0, (struct sockaddr *) &addr, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    
    printf("Received %d bytes from host %s port %d: %s \n", err, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port),msg);
    close(fd);
    return 1;
}

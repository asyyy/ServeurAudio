#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int main (){
    int fd, err;
    char msg[64] = "Hello World \n";
    struct sockaddr_in dest;
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0) {perror("erreur socket");exit(0);}

    dest.sin_family         = AF_INET;
    dest.sin_port           = htons(1234);
    dest.sin_addr.s_addr    = inet_addr("148.60.2.37");

    err = sendto(fd,msg,strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);}
    close(fd);
    return 1;   
}

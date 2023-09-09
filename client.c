#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>


#define MAX_LEN 200
#define err(x,val) if(x<0){printf("%s error\n",val); exit(1);}

void* receiver(void* args){
    char msg[MAX_LEN];
    int sock = *(int*)args;

    while(1){
        memset(msg,0,sizeof(msg));

        if(recv(sock,msg,sizeof(msg),0)==0){
            printf("\nChatroom dismissed!\n");
            exit(0);
        }
        
        printf("\t");
        for(int i = 0;i<strlen(msg)-1;i++){
            printf("%c",msg[i]);
            if(msg[i]=='\n'){
                printf("\t");
            }
        }
        printf("\n");
        if(!strcmp(msg,"You were removed form chatroom!\n")){
            close(sock);
            exit(0);
        }
    }
}

int main(){
    int sock;
    struct sockaddr_in serinfo;
    char input[MAX_LEN];
    char name[MAX_LEN];
    char msg[2*MAX_LEN];
    pthread_t t;
    

    sock = socket(AF_INET,SOCK_STREAM,0);   //socket creation
    err(sock,"Socket creation");

    serinfo.sin_family=AF_INET;             //server info population
    serinfo.sin_port=htons(22000);
    serinfo.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset(name,0,sizeof(name));
    printf("Enter your name: ");        //get user name
    fgets(name,sizeof(name),stdin);
    name[strlen(name)-1]='\0';        

    err(connect(sock,(struct sockaddr*)&serinfo,sizeof(serinfo)),"Connection");     //connection request

    memset(msg,0,sizeof(msg));
    recv(sock,msg,sizeof(msg),0);   //welcome message
    printf("%s",msg);
    if(msg[0]!='W'){        //if not welcome than chatroom full
        exit(0);
    }

    send(sock,name,strlen(msg)+1,0);     //send name to server on successfull connection

    pthread_create(&t,NULL,&receiver,&sock);

    while(1){
        memset(msg,0,sizeof(msg));
        memset(input,0,sizeof(input));
        fgets(input,sizeof(input),stdin);
        while(input[0]=='\n'){         //dont want to send empty input
            fgets(input,sizeof(input),stdin);
        }
        strcpy(msg,name);
        strcat(msg,": ");
        strcat(msg,input);
        send(sock,&msg,strlen(msg),0);
    }

}
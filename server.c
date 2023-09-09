#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h> 
// #include<sys/wait.h>

#define MAX_CLIENTS 3
#define MAX_LEN 100

#define idx(x) x-4
#define idx_to_sock(x) x+4
#define err(x,val) if(x<0){printf("%s error!\n",val);exit(1);};

struct send_msg{
    int snd_Sock;
    char msg[2*MAX_LEN];
};


int connected_clients=0;
char names[MAX_CLIENTS+1][MAX_LEN];         //sizeof(names[0])=MAX_LEN
bool valid[MAX_CLIENTS+1];
bool input_mode =false;


void send_all(void* arg){
    int sock;
    struct send_msg m = *(struct send_msg*)arg;

    for(int i =0;i<MAX_CLIENTS+1;i++){
        if(valid[i]){
            sock = idx_to_sock(i);
            if(sock==m.snd_Sock)continue;
            send(sock,m.msg,strlen(m.msg),0);
        }
    }
}

void* receiver(void* arg){
    int comm= *(int*)arg;
    char msg[MAX_LEN];
    struct send_msg m;
    // pthread_t t;

    valid[idx(comm)]=true;


    memset(msg,0,sizeof(msg));
    recv(comm,msg,sizeof(msg),0);  //recv name of client

    strcpy(names[idx(comm)],msg);
    printf("\n%s joined!\n\n",msg);

    while(1){
        sleep(1);
        memset(msg,0,sizeof(msg));
        int n = recv(comm,msg,MAX_LEN,0);
        while(input_mode);      //wait if input mode
        if(n==0){    //check for client exit
            printf("\n%s Left!\n\n",names[idx(comm)]);
            break;
        }
        printf("%s",msg);
        m.snd_Sock=comm;
        strcpy(m.msg,msg);
        send_all(&m);
    }
    connected_clients--;
    valid[idx(comm)]=false;
    close(comm);
}

void user_list(){
    int choice;
    printf("Press:\n");
    for(int i =0;i<=MAX_CLIENTS;i++){
        if(valid[i]){
            printf("%d: %s\n",i,names[i]);
        }
    }
    printf("-1: to return\n");
    scanf("%d",&choice);
    if(choice==-1){
        return;
    }
    if(choice<=MAX_CLIENTS&&choice>=0){
        if(!valid[choice]){
            printf("Invalid choice!\n");
            return;
        }
        int sock = idx_to_sock(choice);
        valid[choice]=false;
        send(sock,"You were removed form chatroom!\n",34,0);
        close(sock);
    }else{
        printf("Invalid choice!\n");
        return;
    }

}

void send_msg(){
    int choice;
    char ip[MAX_LEN];
    char msg[2*MAX_LEN];
    strcpy(msg,"-#-SERVER-#-: \0");
    printf("Press:\n");
    for(int i =0;i<=MAX_CLIENTS;i++){
        if(valid[i]){
            printf("%d: %s\n",i,names[i]);
        }
    }
    printf("-1: to return\n");
    scanf("%d",&choice);
    if(choice==-1){
        return;
    }
    if(choice<=MAX_CLIENTS&&choice>=0){
        if(!valid[choice]){
            printf("Invalid choice!\n");
            return;
        }
        int sock = idx_to_sock(choice);
        memset(ip,0,sizeof(ip));
        stdin->_IO_read_ptr=stdin->_IO_read_end;
        printf("Enter the message: ");
        fgets(ip,sizeof(ip),stdin);
        strcat(msg,ip);
        send(sock,msg,strlen(msg),0);
    }else{
        printf("Invalid choice!\n");
        return;
    }

}

void* input_routine(void* threads){
    char ip[MAX_LEN];
    int choice;
    while(1){
        
        memset(ip,0,sizeof(ip));

        fgets(ip,sizeof(ip),stdin);
        if(!strcmp(ip,"exit\n")){
            printf("\nChatroom dismissed!\n");
            exit(0);
        }
        if(!strcmp(ip,"input_mode\n")){
            input_mode=true;
            printf("\n\n\n-------------------------------------------------------------------------\n\n\n");
            printf("Press:\n1. See or remove users\n");
            printf("2. Send a message to a specific user\n");
            scanf("%d",&choice);
            if(choice==1){
                user_list();
            }else if(choice==2){
                send_msg();
            }else{
                printf("Invalid choice!\n");
            }
            printf("\n\n\n-------------------------------------------------------------------------\n\n\n");
            input_mode=false;
        }
    }
}


int main(){
    int sock;
    struct sockaddr_in binfo;
    pthread_t t[MAX_CLIENTS+1];
    pthread_t ip_routine;

    for(int i =0;i<MAX_CLIENTS+1;i++){
        valid[i]=false;
    }

    sock = socket(AF_INET,SOCK_STREAM,0);   //socket creation
    err(sock,"Socket creation");

    binfo.sin_family = AF_INET;             //bind info population
    binfo.sin_addr.s_addr = INADDR_ANY;
    binfo.sin_port = htons(22000);

    err(bind(sock,(struct sockaddr*)&binfo,sizeof(binfo)),"bind");      //binding

    err(listen(sock,10),"listen");          //listening

    printf("\nWelcome to the secure chatroom server!\n");
    printf("Anytime write 'input_mode' or 'exit' without quotes to enter input mode or exit server respectively\n\n");

    pthread_create(&ip_routine,NULL,&input_routine,NULL);

    printf("Ready to connect\n");

    while(1){
        
        int comm = accept(sock,NULL,NULL);  //accepting
        err(comm,"accept");
        
        if(connected_clients==MAX_CLIENTS){
            send(comm,"CHATROOM IS CURRENTLY FULL, TRY AGAIN LATTER!\n",48,0);  //chatroom full warning
            close(comm);
            continue;
        }else{
            send(comm,"Welcome to the ChatRoom!\n",27,0);       //welcome message to client
        }

        // printf("comm: %d\n\n",comm);

        connected_clients++;
        pthread_create(&t[idx(comm)],NULL,&receiver,&comm);

    }
    
    close(sock);
    return 0;
}
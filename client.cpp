#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<iostream>
#include<arpa/inet.h>

using namespace std;

int main(int argc,char* argv[]){
    if(argc<3){
        cerr<<"Ussage :"<<argv[0]<<" <ipAddress> <port>"<<endl;
    }
    char* ipAddr = argv[1];
    int port = atoi(argv[2]);
    int sockFd = socket(AF_INET,SOCK_STREAM,0);
    if(sockFd<0){
        perror("socket creation failed");
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_addr.s_addr=inet_addr(ipAddr);
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    
    int connectStatus = connect(sockFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(connectStatus<0){
        perror("Error in connecting");
        return 1;
    }
    cout<<"Connected to server"<<endl;
    char buffer[1024];
    while(true){
        cout<<"Enter message to server: ";
        cin>>buffer;
        send(sockFd,buffer,sizeof(buffer),0);

        int n = recv(sockFd,buffer,sizeof(buffer)-1,0);
        if(n<=0){
            cout<<"Server is down!!\n";
            break;
        }
        buffer[n]='\0';
        cout<<"Server replied: "<<buffer<<endl;   
    }
    
    return 0;
}
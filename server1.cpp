#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<iostream>
#include<stack>
#include    "calculator.cpp"
#include<string>

using namespace std;


Calculator calci;
char buffer[1024] = {0};

int main(int argc, char *argv[]){
    // argc = argument count (number of arguments)
    // argv = argument vector (array of strings)
    if(argc<2){
        cerr<<"Ussage: "<<argv[0]<<" <port>"<<endl;
        return 1;
    }

    int port = atoi(argv[1]);
    
    while(true){
        int sockFd = socket(AF_INET,SOCK_STREAM,0);
        if(sockFd<0){
            cerr<<"socket creation failed"<<endl;
            return 1;
        }
        sockaddr_in serverAddr;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_family=AF_INET;
        serverAddr.sin_addr.s_addr= INADDR_ANY;

        if (::bind(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("bind");
            return 1;
        }
        int clientActive=0;
            if(listen(sockFd,5)<0){
            perror("listen failed");
            return 1;
        }
        cout<<"Server is listening on port: "<<port<<endl;
        
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int clientSock = accept(sockFd,(struct sockaddr*)&clientAddr,&clientLen);

        if(clientSock<0){
            perror("Accept failed");
            return -1;
        }
        cout<<"Connected with client socket number: "<<clientAddr.sin_port<<endl;
        close(sockFd);
        
        while(true){
            int n=recv(clientSock,buffer,sizeof(buffer)-1,0);
            if(n<=0){
                cout<<"Client at port: "<<ntohs(clientAddr.sin_port)<<" disconnected!!\n";
                break;
            }
            buffer[n] = '\0'; 
            cout<<"Client socket "<<ntohs(clientAddr.sin_port)<<" sent message: "<<buffer<<endl;

            int ans = calci.calculate(buffer);
            cout<<"Sending reply: "<<ans<<endl;
            string sBuffer=to_string(ans);
            send(clientSock,sBuffer.c_str(),sBuffer.size(),MSG_NOSIGNAL);
        }
        close(clientSock);
    }
    return 0;
}

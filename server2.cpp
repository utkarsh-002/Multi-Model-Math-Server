#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<sys/wait.h>
#include "calculator.h"

using namespace std;

void sigchld_handler(int){
    while(waitpid(-1,nullptr,WNOHANG)>0);
}

void serveClient(int clientFd,int port){
    string  mssg,input="";
    char buffer[1024]={0};
    while(true){
        int n = recv(clientFd,buffer,sizeof(buffer)-1,0);
        if(n<=0){
            cout<<"client at port: "<<port<<" disconnected!!"<<endl;
            break;
        }
        buffer[n]=  '\0';
        input+=buffer;
        size_t pos;
        while((pos=input.find('\n'))!=string::npos){
            string expr = input.substr(0,pos);
            input.erase(0,pos+1);
            if(expr.empty())
                continue;
            cout<<"Client at port: "<<port<<" message:"<<expr<<endl;
            int ans=Calculator::calculate(expr);
            cout<<"Sending reply: "<<ans<<endl;
            mssg = to_string(ans) +"\n";
            send(clientFd,mssg.c_str(),mssg.size(),MSG_NOSIGNAL);
        }
    }
    close(clientFd);
}

int main(int argc,char* argv[]){
    if(argc<2){
        cout<<"Ussage: "<<argv[0]<<" <port>";
        return -1;
    }
    int port = atoi(argv[1]);
    int listenFd = socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(::bind(listenFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
        perror("bind");
        return -1;
    }
    
    if(listen(listenFd,5)<0){
        perror("listen failed!!");
        return -1;
    }
    cout<<"Server listening on port: "<<port<<endl;

    signal(SIGCHLD,sigchld_handler);

    while(true){
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int clientFd = accept(listenFd,(struct sockaddr*)&clientAddr,&clientLen);
        if(clientFd<0){
            perror("Accept failed");
            return -1;
        }
        cout<<"Connected to client on port: "<<ntohs(clientAddr.sin_port)<<endl;

        pid_t pid = fork();

        if(pid<0){
            perror("fork failed");
            return -1;
        }
        if(pid==0){
            close(listenFd);
            serveClient(clientFd,clientAddr.sin_port);
            exit(0);
        }else{
            close(clientFd);
        }
    }
    close(listenFd);
    return 0;
}
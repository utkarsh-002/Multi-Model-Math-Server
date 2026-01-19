#include<iostream>
#include <cstring>      // for memset, strerror
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_ntop
#include <unistd.h>     // close, read, write
#include <fcntl.h>      // fcntl, F_SETFL, O_NONBLOCK
#include <sys/select.h> // select, fd_set, FD_ZERO, etc.
#include "calculator.h"

using namespace std;
vector<int> all_connections;
unordered_map<int,int> portMap;
fd_set allset,rset;
int nready,maxFd;

int create_tcp_server(int port){

    int listenFd = socket(AF_INET,SOCK_STREAM,0);
    if(listenFd==-1){
        perror("Could not create socket");
        return -1;
    }

    int flags = fcntl(listenFd,F_SETFL,0);
    if(fcntl(listenFd,F_SETFL,flags | O_NONBLOCK)==-1){
        close(listenFd);
        return -1;
    }

    const int opt=1;
    if(setsockopt(listenFd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0){
        perror("setsocket failed");
        close(listenFd);
        return -1;
    }
    sockaddr_in serverAddr{};
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    serverAddr.sin_addr.s_addr=INADDR_ANY;
    
    if(::bind(listenFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
        perror("Bind failed");
        return -1;
    }

    if(listen(listenFd,5)<0){
        perror("listen failed");
        return -1;
    }
    cout<<"Server started listening on port:"<<port<<endl;
    return listenFd;

}

int accept_client(int listenFd){
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(sockaddr);

    int clientFd=accept(listenFd,(struct sockaddr*)&clientAddr,&clientLen);
    if(clientFd!=-1){
        int flags = fcntl(clientFd,F_SETFL,0);
        if(flags==-1) return -1;
        
        if(fcntl(clientFd,F_SETFL,flags | O_NONBLOCK )==-1){
            close(clientFd);
            return -1;
        }
        portMap[clientFd]=ntohs(clientAddr.sin_port);
        return clientFd;
    }else{
        if(errno==EAGAIN || errno==EWOULDBLOCK){
            return -1;
        }
        else{
            perror("Could not connect to client");
            return -1;
        }
    }
}

void serveClient(int clientFd){
    string mssg,input="";
    char buffer[1024]={0};
    while(true){
        int n = read(clientFd,buffer,sizeof(buffer)-1);
        if(n<=0){
            if(errno==EWOULDBLOCK || errno==EAGAIN){
                break;
            }else{
                cout<<"Client disconnected at port:"<<portMap[clientFd]<<endl;
                close(clientFd);
                break;
            }
        }
        buffer[n]='\0';
        input+=buffer;
        size_t pos;
        while((pos=input.find('\n'))!=string::npos){
            string expr = input.substr(0,pos);
            input.erase(0,pos+1);
            if(expr.empty())
                continue;
            cout<<"Client at port: "<<portMap[clientFd]<<" sending mesaage: "<<expr<<endl;
            int ans = Calculator::calculate(expr);
            mssg = to_string(ans)+'\n';
            cout<<"Sending reply: "<<ans<<endl;
            send(clientFd,mssg.c_str(),mssg.size(),MSG_NOSIGNAL);
        }
    }
}

int main(int argc,char* argv[]){
    if(argc<2){
        cout<<"Usage: "<<argv[0]<<" <port>"<<endl;
        return 1;
    }

    int port=atoi(argv[1]);

    int serverFd = create_tcp_server(port);
    if(serverFd==-1){
        cout<<"CRITICAL: could not start server aborting!!"<<endl;
        return 1;
    }

    FD_ZERO(&allset);
    FD_SET(serverFd,&allset);
    maxFd = serverFd;

    while(true){
        rset=allset;
        nready = select(maxFd+1,&rset,NULL,NULL,NULL);

        if(FD_ISSET(serverFd,&rset)){
            if(all_connections.size()==FD_SETSIZE){
                cout<<"Too many clinet!!";
                return -1;
            }

            int clientFd = accept_client(serverFd);
            if(clientFd!=-1){
                FD_SET(clientFd,&allset);
                all_connections.push_back(clientFd);
                maxFd = max(maxFd,clientFd);
            }
            nready--;
            if(nready==0)
                continue;
        }else{
            for(int i=0;i<all_connections.size();i++){
                if(FD_ISSET(all_connections[i],&rset)){
                    nready--;
                    serveClient(all_connections[i]);
                }
                if(nready==0)
                    break;
            }
        }
    }

    close(serverFd);
    return 0;
}
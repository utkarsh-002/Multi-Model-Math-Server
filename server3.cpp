#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<iostream>
#include<stack>
#include "calculator.h"
#include<string>
#include<thread>

using namespace std;

vector<thread> threads;
vector<thread::id> threadIds;

void serveClient(int clientFd,int port){
    string mssg,input="";
    int n;
    char buffer[1024]={0};
    while(true){
        n = recv(clientFd,buffer,sizeof(buffer)-1,0);
        if(n<=0){
            cout<<"Client at port:"<<port<<" disconnected!!"<<endl;
            break;
        }
        buffer[n]='\0';
        input+=buffer;
        size_t pos;
        while((pos=input.find('\n'))!=string::npos){
            string expr = input.substr(0,pos);
            input.erase(0,pos+1);
            if(expr.empty())
                continue;
            cout<<"Recievec message from client at port("<<port<<"):"<<expr<<endl;
            int ans = Calculator::calculate(expr);
            cout<<"Sending reply: "<<ans<<endl;
            mssg = to_string(ans)+"\n";
            send(clientFd,mssg.c_str(),mssg.size(),MSG_NOSIGNAL);
        }
    }
    close(clientFd);
}

int main(int argc,char* argv[]){
    if(argc<2){
        cout<<"Ussage "<<argv[0]<<" <port>"<<endl;
        return -1;
    }
    int port = atoi(argv[1]);

    int listenFd = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in serverAddr{};
    serverAddr.sin_addr.s_addr=INADDR_ANY;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port = htons(port);

    if(::bind(listenFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
        perror("bind failed");
        return -1;
    }

    if(listen(listenFd,5)<0){
        perror("listen failed");
        return -1;
    }

    cout<<"Server is listening on port:"<<port<<endl;

    int clientFd;
    sockaddr_in clientAddr;
    socklen_t clientLen;
    while(true){
        clientFd = accept(listenFd,(struct sockaddr*)&clientAddr,&clientLen);
        if(clientFd<0){
            perror("failed to connect");
            return 1;
        }
        cout<<"Connected to client at port "<<ntohs(clientAddr.sin_port)<<endl;
        thread t(serveClient,clientFd,ntohs(clientAddr.sin_port));
        threads.push_back(std::move(t));
    }
    close(listenFd);
    for(int i=0;i<threads.size();i++)
        threads[i].join();
    return 0;
}
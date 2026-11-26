#include <winsock2.h>
#include <stdio.h>
#include <Ws2tcpip.h> 
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1024

int main () {
    // inicializar 
    sockaddr_in service;
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    
    if (iResult != 0) {
        wprintf(L"Error al iniciar WSAStartup()\n");
        return 1;
    }
    
    SOCKET socketfd = socket(AF_INET, SOCK_STREAM, 0);
    
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(8080);
    
    //bind
    int bind_result = bind(socketfd,(SOCKADDR *) &service, sizeof (service));
    printf("Bind result is: %d\n", bind_result);
    
    if (bind_result == SOCKET_ERROR) {
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(socketfd);
        WSACleanup();
        return 1;
    } else {
        wprintf(L"Bind success\n");
    }
    
    //listen
    if (listen(socketfd, 1) == SOCKET_ERROR) {
        wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
        closesocket(socketfd);
        WSACleanup();
        return 1;
    } else {
        wprintf(L"Listening on socket...\n");
    }
    
    //accept
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    SOCKET AcceptSocket;
    AcceptSocket = accept(socketfd, (struct sockaddr *)&their_addr,NULL);
    
    if (AcceptSocket == INVALID_SOCKET) {
        wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
        closesocket(socketfd);
        WSACleanup();
        return 1;
    } else { 
        wprintf(L"Client connected.\n");
    }
    
    //recv
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    int num_bytes = recv(AcceptSocket, recvbuf, recvbuflen, 0);
    
    /*for (int i = 0; i < num_bytes; ++i) {
        putchar(recvbuf[i]);
    }*/
    
    if (strncmp(recvbuf, "GET", 3) == 0) {
        printf("Received HTTP GET request\n");
        const char buffer[] = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        //"<html><body><h1>Monda</h1></body></html>\r\n"
        ;
        
        send(AcceptSocket, buffer, sizeof(buffer) - 1, 0);
        
        FILE *index_file = fopen("index.html", "r");
        char c;
        while ( (c = getc(index_file)) != EOF ) {
            iResult = send(AcceptSocket, &c, 1, 0);
        }
        
        
        iResult = shutdown(AcceptSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(AcceptSocket);
            WSACleanup();
            return 1;
        }
    } else {
        printf("Received Non-GET request. Ignoring ...\n");
        exit(-1);
    }
    
    //send    
    /*char sendbuf[] = "this is a test";
    
    iResult = send( AcceptSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"send failed with error: %d\n", WSAGetLastError());
        closesocket(AcceptSocket);
        WSACleanup();
        return 1;
    }
    
    printf("Bytes Sent: %ld\n", iResult);
    
    while (1) {
        wprintf(L"Works\n");
        //printf("%d\n", service.sin_addr.s_addr);
        Sleep(1000);
        
    }*/
    iResult = closesocket(AcceptSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"close failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    WSACleanup();
    return 0;
    
}

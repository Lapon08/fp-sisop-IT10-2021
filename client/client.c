#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 4443

int main(int argc, char const *argv[])
{
    char kirim[1024] = {0};
    char terima[1024] = {0};
    if (!getuid() == 0)
    {
        if (argc != 5)
        {
            printf("<USAGE>\n./[program_client_database] -u [username] -p [password]\n");
            return 1;
        }
        if ((strcmp(argv[1], "-u") != 0 || strcmp(argv[3], "-p") != 0))
        {
            printf("<USAGE>\n./[program_client_database] -u [username] -p [password]\n");
            return 1;
        }
    }

    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    /* code */

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    int otentikasi = 0;
    
    if (getuid() == 0)
    {
        sprintf(kirim, "root\troot");
    }else{
        sprintf(kirim, "%s\t%s", argv[2], argv[4]);
    }
    
    send(sock, kirim, strlen(kirim), 0);
    read(sock, terima, 1024);
    printf("%s", terima);
    if (strcmp(terima, "Login Successfull\n") == 0)
    {
        otentikasi = 1;
    }
    else if (strcmp(terima, "Login Failed\n") == 0)
    {
        return 0;
    }

    memset(kirim, 0, 1024);
    memset(terima, 0, 1024);

    while (otentikasi)
    {
        printf(">> ");
        gets(kirim);
        if (strcmp(kirim,"") ==0)
        {
            strcpy(kirim,"dummy");
        }
        
        send(sock, kirim, strlen(kirim), 0);
        read(sock, terima, 1024);
        printf("%s", terima);

        if (strcmp(terima, "Good Bye\n") == 0)
        {
            return 0;
        }
        memset(kirim, 0, 1024);
        memset(terima, 0, 1024);
    }

    return 0;
}
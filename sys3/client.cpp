#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int err = 0;
    int socketDescr = socket(AF_INET, SOCK_STREAM, 0);
    in_addr_t server_ip;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;

    err = inet_pton(AF_INET, "10.10.69.6", &server_ip);
    if (!err)
    {
        return -1;
    }

    serverAddr.sin_addr.s_addr = server_ip;
    serverAddr.sin_port = htons(5000);

    err = connect(socketDescr, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (err == -1)
    {
        return -1;
    }

    const char msg[] = "Hello world";
    int len = strlen(msg);
    err = send(socketDescr, &len, sizeof(int), 0); // Отправляем размер

    if (err == -1)
    {
        return -1;
    }

    err = send(socketDescr, msg, len, 0);
    if (err == -1)
    {
        return -1;
    }
    err = recv(socketDescr, &len, sizeof(len), 0);
    if (err == -1)
    {
        return -1;
    }
    char *buff = (char *)malloc(len);
    if (!buff)
    {
        return -1;
    }

    if (!buff)
    {
        return -1;
    }

    err = recv(socketDescr, buff, len, 0);

    printf("%s\n", buff);
    free(buff);
    close(socketDescr);
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
int main()
{
    struct sockaddr_in server_addr;
    
    int sock = 0, valread;

    char message[19] = "Hello from client";
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0)
    {
        printf("\nConnection failed\n");
        return -1;
    }

    send(sock, message, strlen(message), 0);
    printf("Hello message sent from client\n");
    valread = read(sock, buffer, 1024);
    if (valread < 1)
    {
        printf("\nRead error from client\n");
        return -1;
    }
    printf("%s\n", buffer);
    return 0;
}

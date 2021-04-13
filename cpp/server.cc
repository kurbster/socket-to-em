#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

int main()
{
    struct sockaddr_in address;

    int server_fd, new_socket, valread;
    int opt = 1;
    int addrlen = sizeof(address);
    
    char buffer[1024] = {0};
    char message[19] = "Hello from server!";

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Attach fsocket to port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT,
                &opt, sizeof(opt)))
    {
        perror("setsockopt error");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address,
                sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                    (socklen_t *) &addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024);
    if (valread < 1)
    {
        perror("read error");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buffer);
    send(new_socket, message, strlen(message), 0);
    printf("Hello message sent from server\n");
    return 0;
}

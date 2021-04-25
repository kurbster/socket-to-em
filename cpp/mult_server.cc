#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients = 30, activity, i, valread, sd, max_sd, opt = 1;

    struct sockaddr_in address;

    char buffer[1024];
    char message[42] = "Ravioli ravioli you gave me the formuoli\n";

    fd_set readfds;

    // init all client sockets to be not checked
    for (i = 0; i < max_clients; i++) { client_socket[i] = 0; }

    //create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR,
                (char *) &opt, sizeof(opt)) < 0)
    {
        perror("error setting socket options");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind socket to port 8080
    if (bind(master_socket, (struct sockaddr *) &address,
                sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(master_socket, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for the ravioli");

    while (true)
    {
        // clear the socket set
        FD_ZERO(&readfds);

        // Add the master socket
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add the child sockets to the set
        for (i = 0; i < max_clients; ++i)
        {
            sd = client_socket[i];

            // if the socket is valid add it
            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        // wait for activity on a socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
            printf("select error");

        // If something happened on the master socket
        // Then it's and incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                            (struct sockaddr *) &address,
                            (socklen_t *) &addrlen)) < 0)
            {
                perror("accept error");
                exit(EXIT_FAILURE);
            }
            
            printf("New connection, socket file descriptor is %d, ip: %s, port: %d\n",
                    new_socket, inet_ntoa(address.sin_addr),
                    ntohs(address.sin_port));
            
            // send connection greeting
            if (send(new_socket, message, strlen(message), 0)
                    != (ssize_t)strlen(message))
            {
                perror("send error");
                exit(EXIT_FAILURE);
            }

            puts("Welcome message sent successfully");

            // add the new socket to array of sockets
            for (i = 0; i < max_clients; ++i)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }
        
        // otherwise some IO operation on another socket
        for (i = 0; i < max_clients; ++i)
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds))
            {
                // check if it is closing and read incoming message
                if ((valread = read(sd, buffer, 1024)) <= 0)
                {
                    // some client disconnected
                    getpeername(sd, (struct sockaddr *) &address,
                            (socklen_t *) &addrlen);
                    printf("Host disconnected, ip %s, port %d\n",
                            inet_ntoa(address.sin_addr),
                            ntohs(address.sin_port));

                    // close the socket
                    close(sd);
                    client_socket[i] = 0;
                }

                // Echo back the message that came in
                else
                {
                    buffer[valread] = '\0';
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }
    return 0;
}

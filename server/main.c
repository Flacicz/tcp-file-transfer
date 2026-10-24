#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    const int address_family = AF_INET;
    const char ip_address[] = "127.0.0.1";
    const uint16_t port = 9000;

    struct sockaddr_in address = {0};

    address.sin_family = address_family;

    const uint16_t network_port = htons(port);
    address.sin_port = network_port;

    const int res = inet_pton(address_family, ip_address, &address.sin_addr);

    if (res == 0)
    {
        fprintf(stderr, "Incorrect IPv4 address: %s\n", ip_address);
        return 1;
    }
    if (res == -1)
    {
        perror("inet_pton");
        return 1;
    }

    const int listen_fd = socket(address_family, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        perror("socket");
        return 1;
    }

    int exit_code = 0;
    int connection_fd = -1;

    if (bind(listen_fd, (const struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind");
        exit_code = 1;
        goto cleanup;
    }

    if (listen(listen_fd, 5) == -1)
    {
        perror("listen");
        exit_code = 1;
        goto cleanup;
    }

    printf("Server listening on port 9000\n");

    do
    {
        connection_fd = accept(listen_fd, NULL, NULL);
    } while (connection_fd == -1 && errno == EINTR);

    if (connection_fd == -1)
    {
        perror("accept");
        exit_code = 1;
        goto cleanup;
    }

    printf("Server accept connection\n");

cleanup:
    if (connection_fd >= 0 && close(connection_fd) == -1)
    {
        perror("close connection socket");
        exit_code = 1;
    }

    if (close(listen_fd) == -1)
    {
        perror("close listening socket");
        exit_code = 1;
    }

    return exit_code;
}

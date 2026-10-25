#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
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

    const int connect_fd = socket(address_family, SOCK_STREAM, 0);
    if (connect_fd == -1)
    {
        perror("socket");
        return 1;
    }

    int exit_code = 0;

    if (connect(connect_fd, (const struct sockaddr*)&address, sizeof(address)) == -1)
    {
        perror("connect");
        exit_code = 1;
        goto cleanup;
    }

    printf("Client connect successfully\n");

    const char buffer[] = "HELLO";
    size_t send_count = 0;

    while (send_count < 5)
    {
        const ssize_t send_result =
            send(connect_fd, buffer + send_count, sizeof(buffer) - send_count - 1, MSG_NOSIGNAL);

        if (send_result == 0)
        {
            fprintf(stderr,
                    "send returned 0 before the message was fully sent (%zu of 5 bytes sent)\n",
                    send_count);
            exit_code = 1;
            goto cleanup;
        }

        if (send_result == -1 && errno == EINTR)
        {
            continue;
        }
        if (send_result == -1)
        {
            perror("send");
            exit_code = 1;
            goto cleanup;
        }

        send_count += send_result;
    }

    printf("Sent 5 message bytes to the socket\n");

cleanup:
    if (close(connect_fd) == -1)
    {
        perror("close");
        exit_code = 1;
    }

    return exit_code;
}

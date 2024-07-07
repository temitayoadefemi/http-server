#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "bst.h"

#define PORT 8080
#define BACKLOG 10

extern Node* root;  // BST root declared in bst.c

void handle_client(int client_socket) {
    char buffer[1024];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("read failed");
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';
    printf("Request: %s\n", buffer);

    char method[16], path[256], version[16];
    sscanf(buffer, "%s %s %s", method, path, version);

    // Lookup the path in the BST
    Node *found = search(root, path + 1);  // +1 to skip the leading '/'

    char response[1024];
    int response_length;
    if (found) {
        // Found the path in the BST, respond with the HTML content
        response_length = snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s", strlen(found->value), found->value);
    } else {
        // Path not found, respond with 404 Not Found
        const char *not_found_message = "<html><body><h1>404 Not Found</h1></body></html>";
        response_length = snprintf(response, sizeof(response),
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s", strlen(not_found_message), not_found_message);
    }

    write(client_socket, response, response_length);
    close(client_socket);
}

int main(int argc, char *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;

    socklen_t addr_len = sizeof(client_addr);
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("HTTP Server listening on port %d\n", PORT);

    root = insertNode(root, "hello", "<html><body><h1>Hello, World!</h1></body></html>");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        handle_client(new_socket);
    }

    close(server_fd);
    return 0;
}

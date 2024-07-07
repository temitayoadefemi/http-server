#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BACKLOG 10

void handle_client(int client_socket) {
    char buffer[1024];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("read failed");
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';
    printf("%s", buffer);

    char method[16], path[256], version[16];
    sscanf(buffer, "%s %s %s", method, path, version);

    const char *html_content = "<html><body><h1>Hello, World!</h1></body></html>";
    int content_length = strlen(html_content);  // Calculate the length of the HTML content

    char response[1024];
    int response_length = snprintf(response, sizeof(response),

    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %d\r\n"  // Use the calculated length
    "\r\n"
    "%s", content_length, html_content);



    write(client_socket, response, response_length);

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

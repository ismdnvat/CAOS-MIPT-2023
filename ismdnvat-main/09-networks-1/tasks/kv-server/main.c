#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#define PORT 7001
#define PATH_MAX_LEN 4096

typedef struct StorageItem {
    char key[PATH_MAX_LEN];
    char value[PATH_MAX_LEN];
    struct StorageItem* next;
} StorageItem;

typedef struct Storage {
    struct StorageItem* head;
} Storage;

int server_socket;

void handle_sigint(int signo) {
    printf("\nServer shutting down...\n");
    close(server_socket);
    exit(0);
}


StorageItem* find(Storage* storage, char* key) {
    StorageItem* current = storage->head;
    while (current != NULL) {
        if (strncmp(current->key, key, PATH_MAX_LEN) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void set(Storage* storage, char* key, char* value) {
    StorageItem* element = find(storage, key);
    if (element == NULL) {
        element = malloc(sizeof(StorageItem));
        snprintf(element->key, sizeof(element->key), "%s", key);
        element->next = storage->head;
        storage->head = element;
    }
    snprintf(element->value, sizeof(element->value), "%s", value);
}

char* get(Storage* storage, char* key) {
    StorageItem* element = find(storage, key);
    if (element == NULL) {
        return "";
    } else {
        return element->value;
    }
}

void handle_client(int client_socket, Storage* storage) {
    char buffer[PATH_MAX_LEN];
    ssize_t received_bytes;

    while ((received_bytes = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[received_bytes] = '\0'; // Null-terminate the received data

        if (strncmp(buffer, "get ", 4) == 0) {
            // Handle get command
            char key[PATH_MAX_LEN];
            sscanf(buffer, "get %s", key);
            char* value = get(storage, key);
            send(client_socket, value, strlen(value), 0);
        } else if (strncmp(buffer, "set ", 4) == 0) {
            // Handle set command
            char key[PATH_MAX_LEN];
            char value[PATH_MAX_LEN];
            sscanf(buffer, "set %s %[^\n]", key, value);
            set(storage, key, value);
        }
    }

    close(client_socket);
}

int main() {
    Storage* storage = malloc(sizeof(Storage));
    storage->head = NULL;

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Set up server address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind socket
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // Listen for incoming connections
    listen(server_socket, 5);

    printf("Server listening on port %d...\n", PORT);
    signal(SIGINT, handle_sigint);
    while (1) {
        // Accept a connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Handle the client in a separate function
        handle_client(client_socket, storage);
    }

    close(server_socket);

    return 0;
}


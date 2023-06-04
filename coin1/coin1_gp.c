#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define TRUE 1
#define HOST "128.61.240.205"
#define PORT 9007


int find_counterfeit_coins(int socket_fd, int number_of_coins, int trials) {
    int coins_to_weigh[number_of_coins];
    for (int i = 0; i < number_of_coins; i++) {
        coins_to_weigh[i] = i;
    }

    for (int i = 0; i < trials; i++) {
        int group_size = number_of_coins / 2;

        if (test_coins(socket_fd, coins_to_weigh, group_size)) {
            number_of_coins = group_size;
        } else {
            memmove(coins_to_weigh, coins_to_weigh + group_size, group_size * sizeof(int));
            number_of_coins -= group_size;
        }
    }

    return coins_to_weigh[0];
}

int test_coins(int socket_fd, int* coins_array, int array_length) {
    char coins_string[10000] = {0};
    char buffer[1024] = {0};

    for (int i = 0; i < array_length; i++) {
        char current_coin[10];
        sprintf(current_coin, "%d ", coins_array[i]);
        strcat(coins_string, current_coin);
    }

    strcat(coins_string, "\n");

    send(socket_fd, coins_string, strlen(coins_string), 0);
    read(socket_fd, buffer, 1024);

    int total_weight = atoi(buffer);
    return (total_weight % 2 == 1);
}

int main(void) {
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[2048] = { 0 };
    int number_of_coins = 0;
    int trials = 0;
    char counterfeit_coin[10] = { 0 };

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, HOST, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    valread = read(client_fd, buffer, 2048);
    printf("%s\n", buffer);
    bzero(buffer, sizeof(buffer));

    while (TRUE) {
        valread = read(client_fd, buffer, 2048);
        char* token = strtok(buffer, " ");
        number_of_coins = atoi(token + 2);
        token = strtok(NULL, " ");
        trials = atoi(token + 2);

        int counterfeit_coin = find_counterfeit_coins(client_fd, number_of_coins, trials);
        sprintf(buffer, "%d\n", counterfeit_coin);
        send(client_fd, buffer, strlen(buffer), 0);

        bzero(buffer, sizeof(buffer));
        valread = read(client_fd, buffer, 2048);
        bzero(buffer, sizeof(buffer));
    }

    close(client_fd);
    return 0;
}

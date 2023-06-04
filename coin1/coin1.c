#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define TRUE 1
#define HOST "pwnable.kr"
#define PORT 9007


int get_number_of_coins(char* message) {
	char number_of_coins[10] = {0};
	size_t space_index = strcspn(message, " ");
	size_t start = 2;
	strncpy(number_of_coins, message + start, (space_index - start));
	return atoi(number_of_coins);
}

int get_trials(char* message) {
	char trials[10] = {0};
	size_t space_index = strcspn(message, " ");
	strncpy(trials, message + space_index + 3, strlen(message) - space_index - 3);
	return atoi(trials);
}

// int test_coins(int socket_fd, int* coins_array, int array_length) {
// 	char coins_string[10000] = {0};
// 	char current_coin[10] = {0};
// 	char buffer[1024] = {0};
// 	int total_weight = 0;

// 	for (int i = 0; i < array_length; i++)
// 	{	
// 		sprintf(current_coin, "%d ", coins_array[i]);
// 		strcat(coins_string, current_coin);
// 	}

// 	strcat(coins_string, "\n");

// 	send(socket_fd, coins_string, strlen(coins_string), 0);
// 	read(socket_fd, buffer, 1024);
// 	buffer[strcspn(buffer, "\n")] = 0;
// 	total_weight = atoi(buffer);
// 	bzero(buffer, sizeof(buffer));

// 	return (total_weight % 2);
// }

// int find_counterfeit_coins(int socket_fd, int number_of_coins, int trials) {
// 	int coins_to_weigh[1000] = {0};
// 	for (int i = 0; i < number_of_coins; i++) {
// 		coins_to_weigh[i] = i;
// 	}
    
// 	for (int j = 0; j < trials; j++) {
// 		if (number_of_coins == 1)
// 		{
// 			continue;
// 		}

// 		if (test_coins(socket_fd, coins_to_weigh, number_of_coins/2)) {
// 			number_of_coins = number_of_coins / 2;

// 		} else {
// 			number_of_coins = number_of_coins / 2;
// 			for (int i = 0; i < number_of_coins; i++) {
// 				coins_to_weigh[i] = coins_to_weigh[i + number_of_coins];
// 			}
// 		}
// 	}

// 	return coins_to_weigh[0];
// }


int test_coins(int socket_fd, int* coins_array, int array_length) {
    char coins_string[10000] = {0};
    char current_coin[10] = {0};
    char buffer[1024] = {0};
    int total_weight = 0;

    for (int i = 0; i < array_length; i++) {	
        sprintf(current_coin, "%d ", coins_array[i]);
        strcat(coins_string, current_coin);
    }

    strcat(coins_string, "\n");

    send(socket_fd, coins_string, strlen(coins_string), 0);
    read(socket_fd, buffer, 1024);
    buffer[strcspn(buffer, "\n")] = 0;
    total_weight = atoi(buffer);
    bzero(buffer, sizeof(buffer));

    return (total_weight % 2);
}

int find_counterfeit_coins(int socket_fd, int number_of_coins, int trials) {
    int low = 0;
    int high = number_of_coins - 1;

    while (low < high && trials > 0) {
        int mid = (low + high) / 2;
        int group_size = (high - low + 1) / 2;

        // Weigh the first half of the group against the second half
        int coins_to_weigh[2 * group_size];
        for (int i = 0; i < group_size; i++) {
            coins_to_weigh[i] = low + i;
            coins_to_weigh[group_size + i] = mid + 1 + i;
        }

        // Simulate the test_coins function by calculating the total weight parity
        int total_weight_parity = test_coins(socket_fd, coins_to_weigh, 2 * group_size);

        if (total_weight_parity == 0) {
            // Both halves have the same weight parity, counterfeit coin is in the remaining higher range
            low = mid + 1;
        } else {
            // First half and second half have different weight parity,
            // counterfeit coin is in the half with a different weight parity
            high = mid;
        }

        trials--;
    }

    return low;
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
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "128.61.240.205", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
  
    valread = read(client_fd, buffer, 2048);
    printf("%s\n", buffer);
    bzero(buffer, sizeof(buffer));

    while (TRUE) {

	    valread = read(client_fd, buffer, 2048);
	    
	    number_of_coins = get_number_of_coins(buffer);
	    trials = get_trials(buffer);

	    bzero(buffer, sizeof(buffer));
	    sprintf(counterfeit_coin, "%d\n", find_counterfeit_coins(client_fd, number_of_coins, trials));	    
	    send(client_fd, counterfeit_coin, strlen(counterfeit_coin), 0);

	   	valread = read(client_fd, buffer, 2048);
		bzero(buffer, sizeof(buffer));
		send(client_fd, counterfeit_coin, strlen(counterfeit_coin), 0);

	   	valread = read(client_fd, buffer, 2048);
		bzero(buffer, sizeof(buffer));


	}

	
    // closing the connected socket
    close(client_fd);
    return 0;
}

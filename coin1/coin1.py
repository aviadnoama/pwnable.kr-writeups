import socket

HOST = "pwnable.kr"  # The server's hostname or IP address
PORT = 9007  # The port used by the server

# def find_counterfeit_coins(s, first_coin, last_coin, max_coins, trials_left):
#     coins_to_weigh = ' '.join([str(i) for i in range(first_coin, last_coin)]) + '\n'
#     print(coins_to_weigh)
#     s.sendall(bytes(coins_to_weigh, encoding='utf-8'))        
#     combined_weigh = str(s.recv(1024).strip())[2:][:-1]
#     print(combined_weigh)


def find_counterfeit_coins(s, coin_arr, low, high):
 
    # Check base case
    if high >= low:
 
        mid = (high + low) // 2
 
        # If element is present at the middle itself
        if coin_arr[mid] == x:
            return mid
 
        # If element is smaller than mid, then it can only
        # be present in left subcoin_array
        elif coin_arr[mid] > x:
            return binary_search(coin_arr, low, mid - 1, x)
 
        # Else the element can only be present in right subcoin_array
        else:
            return binary_search(coin_arr, mid + 1, high, x)
 
    else:
        # Element is not present in the array
        return -1

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    welcome_message = s.recv(2048)
    # print(str(welcome_message))

    coins, trials = [i.strip()[2:] for i in str(s.recv(1024).strip())[2:][:-1].split(' ')]
    print(coins)
    print(trials)
    coins_array = range(first_coin, last_coin)
    find_counterfeit_coins(s, 0, int(int(coins)/2), int(coins), int(trials))


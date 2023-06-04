### A message from me ###
# I solved it not too long after trying, but then the speed was too slow
# my "great" idea was writing this logic in C because OF COURSE C will be faster right?
# After WAY to long of having small anoying bugs in my C code, after seesing that
# even my broken, NOT working C code is not as fast as it should be
# I noticed the pwnable message of "try running it from localy using SSH"
# And then I just ran my python and it worked.
# I hate my life.
# But hey. atleast it's pwned :)




import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 9007  # The port used by the server


def find_counterfeit_coins(s, number_of_coins, trials):
    coins_to_weigh = list(range(int(number_of_coins))) 
    for i in range(trials):
        if test_coins(s, coins_to_weigh[:int(len(coins_to_weigh)/2)]):
            coins_to_weigh = coins_to_weigh[:int(len(coins_to_weigh)/2)]
        else:
            coins_to_weigh = coins_to_weigh[int(len(coins_to_weigh)/2):]

    return(coins_to_weigh[0])


def test_coins(s, coins_list):
    coins_string = ' '.join([str(i) for i in coins_list]) + '\n'
    s.sendall(bytes(coins_string, encoding='utf-8'))
    total_weigh = s.recv(1024).decode('utf-8')
    return (int(total_weigh) % 2 == 1)



with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    welcome_message = s.recv(2048)

    while True:
        message = s.recv(1024)
        print(message)
        number_of_coins, trials = [i.strip()[2:] for i in str(message.strip())[2:][:-1].split(' ')]
        counterfeit_coin = find_counterfeit_coins(s, int(number_of_coins), int(trials))
        s.sendall(bytes(str(counterfeit_coin) + '\n', encoding='utf-8'))
        print(s.recv(1024))
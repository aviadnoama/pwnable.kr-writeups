python3 -c 'import sys,time; l1 = b"aaaa\n"; l2 = b"bbbb\n"; time.sleep(1); sys.stdout.buffer.write(l1); sys.stdout.buffer.flush(); time.sleep(1); sys.stdout.buffer.write(l2); sys.stdout.buffer.flush(); time.sleep(1)'
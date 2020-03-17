# Video Transfer

## Server
1. Open `server.cc` and change line 20 containing `#define MY_IP "192.168.0.190"`, to the IP where the server is going to run.
2. Do `cmake` and `make servidor`
3. Then, run `./servidor`.

## Client
1. Open `ip_server.txt` and write down the IP where C++ server is going to run.
2. Run `python cliente.py`.
3. Follow GUI instructions to receive the streaming.


from socket import *
serverPort = 12000
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(1)
print('The server is ready to receive')
while True:
    connectionSocket, addr = serverSocket.accept()
    try :
        filename = connectionSocket.recv(2048).split()[1].decode()
        print("filename: {}".format(filename))
        with open(filename, "r") as f:
            outputdata = f.read()

        response = ("HTTP/1.1 200 OK\n"
                "Server: Python\n"
                "Content-Type: text/html; charset=utf-8\r\n\n")
        connectionSocket.send(response.encode())
        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())
        connectionSocket.send("\r\n".encode())
        connectionSocket.close()
    except IOError:
        # Send response message for file not found
        response = ("HTTP/1.1 404 Not Found\n"
                    "Server: Python 3.7.2\n"
                    "Content-Type: text/html; charset=utf-8\r\n\n")
        connectionSocket.send(response.encode())
        connectionSocket.send("\r\n".encode())
        connectionSocket.close()

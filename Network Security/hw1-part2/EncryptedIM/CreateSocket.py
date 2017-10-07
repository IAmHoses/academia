""" 
	create_server_socket()
		instantiates a socket that binds to port 9999
		and listens for a single connetion request

	create_client_socket(string)
		creates a socket that will send a connection
		request to the server socket bound on port 9999
"""

import socket

def create_server_socket():
	# instantiate internet stream socket, bind to desired port, and listen
	serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)	
	serverSocket.bind(('', 9999))
	serverSocket.listen(1)

	#print "Server socket created at " + str(serverSocket.getsockname())
	return serverSocket


def create_client_socket(hostname):
	# create socket, send connection request
	clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	clientSocket.connect((hostname, 9999))

	#print "Client socket created at " + str(clientSocket.getsockname())
	return clientSocket

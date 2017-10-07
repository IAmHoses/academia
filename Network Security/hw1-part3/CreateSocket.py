""" 
	create_server_socket()
		instantiates a socket that binds to port 9999
		and listens for a single connetion request

	create_client_socket(string)
		creates a socket that will send a connection
		request to the server socket bound on port 9999
"""

import socket
import sys

def create_server_socket(first_communication = False):
	
	try:
		# instantiate internet stream socket, bind to desired port, and listen
		serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)	
		serverSocket.bind(('', 9999))
		serverSocket.listen(1)
		
		if first_communication == True:
			print "Waiting on connection..."

		# attempt to establish connection
		clientSocket, address = serverSocket.accept()
		if first_communication == True:
			print "%s has connected." % str(address)
		return serverSocket, clientSocket

	except:
		print "Unable to establish connection to %s, program terminated" % str(serverSocket.getpeername())
		sys.exit()

def create_client_socket(hostname, first_communication = False):
	try:
		# create socket, send connection request
		clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		clientSocket.connect((hostname, 9999))

		if first_communication == True:
			print "Connection established at %s" % hostname
		return clientSocket

	except:
		print "Unable to establish connection to %s, program terminated" % hostname
		sys.exit()

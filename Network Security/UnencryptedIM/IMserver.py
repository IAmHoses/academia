"""
	IM_server()
		allows a user to create a server socket, establish a 
		connection with a client, and exchange messages.
		Press CTRL + C to terminate sessions
"""

import CreateSocket, select, sys

def IM_server():
	
	# create server socket
	serverSocket = CreateSocket.create_server_socket()
	print "Waiting on connection..."

	try:
		# attempt to establish connection
		clientSocket, address = serverSocket.accept()

	except:
		print "Unable to establish connection to" + str(serverSocket.getpeername)
		return

	print str(address) + " has connected to the server socket."
	client_sockets = [clientSocket]
	connected = True

	# write from standard input and attempt to detect received data
	while connected:
		# use select to implement non-blocking sockets
		readList = [sys.stdin, serverSocket] + client_sockets
		readReady, writeReady, exceptions = select.select(readList, [], [], 0)

		# two caess: (1) standard input or (2)received message
		for socket in readReady:
			if socket == sys.stdin:		# read information and send to client
				serverMessage = sys.stdin.readline()
				clientSocket.sendall(serverMessage)

			else:	# read the receieved message from client
				message = clientSocket.recv(4092)
				
				if message != "":		# then there's actually text in the buffer, print to screen
					sys.stdout.write('[' + str(clientSocket.getpeername()) + "]: " + message); sys.stdout.flush()

				else:	# there's really nothing, indicates connection to client closed
					client_sockets.remove(clientSocket)
					connected = False
					print "Client " + str(clientSocket.getpeername()) + " has disconnected."
					break
	#close socket
	serverSocket.close()
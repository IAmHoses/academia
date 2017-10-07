"""
	IM_client(string)
		allows a user to create a client socket and send a
		connection request to a server socket. If the connection
		is established, then messages can be exchanged until
		CTRL + C is pressed.
"""

import CreateSocket, select, sys

def IM_client(hostname):
	
	try:
		# establish connection to server
		clientSocket = CreateSocket.create_client_socket(hostname)

	except:
		print "Unable to establish connection to " + hostname

	print "Connection established at " + hostname
	connected = True

	# write from standard input and attempt to detect received data
	while connected:
		# use select to implement non-blocking sockets
		readList = [sys.stdin, clientSocket]
		readReady, writeReady, exceptions = select.select(readList, [], [], 0)

		# two caess: (1) standard input or (2)received message
		for socket in readReady:
			if socket == clientSocket:	# then read receieved message from client
				serverMessage = clientSocket.recv(4098)

				if serverMessage != "":		# then there's data to be read from socket, print to screen
					sys.stdout.write('[' + hostname + "]: " + serverMessage); sys.stdout.flush()

				else:	# there's no data in buffer, connection to server closed
					connected = False
					print "Connection to " + hostname + " terminated."
					break

			else:	# user typed a new message into standard input
				clientMessage = sys.stdin.readline()
				clientSocket.sendall(clientMessage)

	# close sockets
	clientSocket.close()
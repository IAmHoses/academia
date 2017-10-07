"""
	IM_server()
		allows a user to create a server socket, establish a 
		connection with a client, and exchange messages.
		Press CTRL + C to terminate sessions
"""

import CreateSocket, select, sys
import Encryption, Authentication, base64


def IM_server(conf_key, auth_key):
	
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
			if socket == sys.stdin:		# read information, encrypt and HMAC, then send to client
				serverMessage = sys.stdin.readline()
				encMessage = Encryption.encrypt(conf_key, serverMessage)
				clientSocket.sendall(base64.b64encode(Authentication.make_HMAC(auth_key, encMessage)))

			else:	# read the receieved message from client
				raw_message = clientSocket.recv(4092)

				if raw_message != "":	# new message! Decode, authenticate, decrypt, and print to screen
					try:
						decoded_message = base64.b64decode(raw_message)
						recv_ciphertext = Authentication.check_HMAC(auth_key, decoded_message)
					except AssertionError, e:
						print str(e)
						sys.exit()

					# authentication check passed, decrypt and print
					plaintext = Encryption.decrypt(conf_key, recv_ciphertext)
					sys.stdout.write('[' + str(clientSocket.getpeername()) + "]: " + plaintext); sys.stdout.flush()

				else:	# there's really nothing, indicates connection to client closed
					client_sockets.remove(clientSocket)
					connected = False
					print "Client " + str(clientSocket.getpeername()) + " has disconnected."
					break
	#close socket
	serverSocket.close()
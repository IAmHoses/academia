"""
	IM_server()
		allows a user to create a server socket, establish a 
		connection with a client, and exchange messages.
		Press CTRL + C to terminate sessions
"""

import CreateSocket, select, sys
import DiffieHellman
import Encryption, Authentication, base64


def IM_server(auth_key):
	try:
		# generate ephemeral confidentiality key
		conf_key = DiffieHellman.diffie_hellman_server()
	except:
		print "Unable to generate session key, program terminated"
		sys.exit()

	# create server socket
	serverSocket, clientSocket = CreateSocket.create_server_socket()
	connected = True
	client_sockets = [clientSocket]
	
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
					print(plaintext)

				else:	# there's really nothing, indicates connection to client closed
					client_sockets.remove(clientSocket)
					connected = False
					print "Client " + str(clientSocket.getpeername()) + " has disconnected."
					break
	#close socket
	serverSocket.close()
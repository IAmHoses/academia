"""
	IM_client(string)
		allows a user to create a client socket and send a
		connection request to a server socket. If the connection
		is established, then messages can be exchanged until
		CTRL + C is pressed.
"""

import CreateSocket, select, sys
import Encryption, Authentication, base64


def IM_client(hostname, conf_key, auth_key):
	
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

				if serverMessage != "":		# new message! Decode, authenticate, decrypt, and print to screen
					try:
						decoded_message = base64.b64decode(serverMessage)
						recv_ciphertext = Authentication.check_HMAC(auth_key, decoded_message)
					except AssertionError, e:
						print str(e)
						sys.exit()

					# authentication check passed, decrypt and print
					plaintext = Encryption.decrypt(conf_key, recv_ciphertext)
					sys.stdout.write('[' + hostname + "]: " + plaintext); sys.stdout.flush()

				else:	# there's no data in buffer, connection to server closed
					connected = False
					print "Connection to " + hostname + " terminated."
					break

			else:	# user typed a new message into standard input
				clientMessage = sys.stdin.readline()
				encMessage = Encryption.encrypt(conf_key, clientMessage)
				clientSocket.sendall(base64.b64encode(Authentication.make_HMAC(auth_key, encMessage)))

	# close sockets
	clientSocket.close()	
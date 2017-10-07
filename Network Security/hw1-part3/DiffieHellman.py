import CreateSocket
import os

""" 
	diffie_hellman_server()
		server-side implementation of the Diffie-Hellman exchange.
		The base and prime used in computation are hardcoded, and
		one-time random secrets are generated before a connection
		is established to the other party. After a successful
		exchange, an ephemeral confidentiality key is returned to 
		the user for their session with an individual client

	create_client_socket(string)
		client-side implementation of the Diffie-Hellman exchange,
		returns an ephemeral key for session with server.
"""

prime = 0x00cc81ea8157352a9e9a318aac4e33ffba80fc8da3373fb44895109e4c3ff6cedcc55c02228fccbd551a504feb4346d2aef47053311ceaba95f6c540b967b9409e9f0502e598cfc71327c5a455e2e807bede1e0b7d23fbea054b951ca964eaecae7ba842ba1fc6818c453bf19eb9c5c86e723e69a210d4b72561cab97b3fb3060b

def diffie_hellman_server(g = 2):
	my_secret = long(os.urandom(64).encode('hex'), 16)

	serverSocket, clientSocket = CreateSocket.create_server_socket(True)
	waiting = True

	while waiting:
		incomplete_key = clientSocket.recv(1024)
	
		if incomplete_key == '':
			break
		else: 
			clientSocket.send(str(pow(long(g), my_secret, prime)))
			waiting = False

	serverSocket.close()

	eph_key = pow(long(incomplete_key), my_secret, prime)
	return eph_key

def diffie_hellman_client(hostname, g = 2):
	my_secret = long(os.urandom(64).encode('hex'), 16)

	clientSocket = CreateSocket.create_client_socket(hostname, True)
	clientSocket.send(str(pow(long(g), my_secret, prime)))
	incomplete_key = clientSocket.recv(1024)
	clientSocket.close()

	eph_key = pow(long(incomplete_key), my_secret, prime)
	return eph_key
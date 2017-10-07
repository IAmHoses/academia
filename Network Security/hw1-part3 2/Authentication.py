"""
	make_HMAC(authKey, encMessage)
		Constructs a Hash-based Message Authentication Code, taking an
		authentication key and an encrypted message as its arguments.
		The method then returns an encoded concatenation of the HMAC and
		encrypted message, allowing the user to send it immediately

	check_HMAC(authKey, recvMessage)
		Checks the authenticity of a received message by first decoding it, and then
		comparing its provided HMAC with HMAC(authkey, recvMessage - recvMessage.HMAC).
		If the message is succesfully authenticated, the function returns the encrypted 
		message following the read hash. Else, an exception will be thrown
"""

from Crypto.Hash import HMAC	
from Crypto.Hash import SHA256	# used to force keys to be 256 bits long

def make_HMAC(auth_key, encMessage):
	# normalize key to a 256-bit value
	hasher = SHA256.new(bytearray(auth_key, 'utf-8'))
	auth_key = hasher.digest()

	# hash encrypted messsage, then return concatenation of hash and encrypted messsage
	authenticator = HMAC.new(auth_key, encMessage, SHA256)
	return authenticator.hexdigest() + encMessage

def check_HMAC(auth_key, recvMessage):
	# normalize key to a 256-bit value
	hasher = SHA256.new(bytearray(auth_key, 'utf-8'))
	auth_key = hasher.digest()

	# extract HMAC and check authenticity of received message
	recv_HMAC = recvMessage[:2*SHA256.digest_size]
	ciphertext = recvMessage[2*SHA256.digest_size:]

	# check authenticity of received message
	authenticator = HMAC.new(auth_key, ciphertext, SHA256)
	assert recv_HMAC == authenticator.hexdigest(), "Authentication check failed on incoming message, program terminated."
	return ciphertext
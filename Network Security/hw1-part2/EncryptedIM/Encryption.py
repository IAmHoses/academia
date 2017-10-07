"""
	encrypt()
		Implements a E(k,m) cryptographic protocol for a user sending a message.
		The method will encrypt the message m with confidentiality key k using
		AES-256 in CBC mode and a randomly generated initialization vector (IV).
		After encrypting the message, encrypt() will return a concatenation of 
		the ephemeral IV and encrypted message.
		
	decrypt()
		Implements a D(k,m) cryptographic protocol for a user receiving a message.
		The method will extract its provided IV and decrypt the message using both
		the IV and a confidentiality key shared with the sender. If the message is
		not properly decrypted, an exception will be thrown.
		
	Note: These protocols, when used symmetrically, only provide confidentiality. For 
	authenticity, see Authentication.py. More information about message structure can
	be found in the protocol documentation provided with these files.
	
"""

from Crypto.Hash import SHA256	# forces keys to be 256 bits (32 bytes) long
import os						# /dev/urandom
from Crypto.Cipher import AES	# AES-256 in CBC mode!

def encrypt(conf_key, plaintext):
	# normalize key to a 256-bit value
	hasher = SHA256.new(bytearray(conf_key, 'utf-8'))
	conf_key = hasher.digest()

	# randomly generate initialization vector
	iv = os.urandom(AES.block_size)

	# padding scheme PKCS#7
	padding_bytes = AES.block_size - (len(plaintext) % AES.block_size)
	plaintext += chr(padding_bytes)*padding_bytes

	# wrap up with text string encoding and return
	cipher = AES.new(conf_key, AES.MODE_CBC, iv)
	return iv + cipher.encrypt(plaintext)


def decrypt(conf_key, ciphertext):
	# normalize key to a 256-bit value
	hasher = SHA256.new(bytearray(conf_key, 'utf-8'))
	conf_key = hasher.digest()

	# extract initialization vector
	iv = ciphertext[:AES.block_size]

	# decrypt message
	cipher = AES.new(conf_key, AES.MODE_CBC, iv)
	plaintext = cipher.decrypt(ciphertext[AES.block_size:])
	return plaintext

	# unpadding scheme PKCS#7, then return
	delimiter = len(plaintext) - int(plaintext[-1])
	print plaintext[:delimiter]

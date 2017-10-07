import argparse
import socket
import time
import math
import sys

"""
	PortScan is a networking tool that scans a specified target's ports in order. The program accepts 
	the target as a command line argument and passes it into the function scan_ports(target), which 
	will attempt to establish a connection with every TCP port on the target. After scanning is 
	completed, a report is produced that contains the following information:

		(1) each open port's number and running service
		(2) number of ports found to be open and time taken to scan ports
		(3) program scan rate

	command-line usage: python PortScan.py -t target
"""

def scan_ports(target):
	start_time = time.time()

	report = "-----------------\nOpen Ports Found\n-----------------\n"
	num_open_ports = 0

	# we will not scan port 0 since it is used to dynamically assign ports for applications
	for port in range(1, pow(2, 16)):
		try:
			sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			#settime out?
			conn_error = sock.connect_ex((target, port))

			# connection accepted -> (1) increment num_open_ports, (2) push port number and service into 
			if conn_error == 0:
				try:
					report += "%s running on port %s\n" % (socket.getservbyport(port), str(port))
				except:
					report += "undefined service running on port %s\n" % str(port)

				num_open_ports += 1

			# close socket and continue, covers refused connection case
			sock.close()

		# unexpected error while creating socket or establishing connnection
		except socket.gaierror as gaiE:
			print gaiE
			sys.exit(1)

		except socket.error as sE:
			print sE
			sys.exit(1)

	scan_time = time.time() - start_time
	
	# finish report and print
	report += "\n%s open ports found in %s seconds\n" % (str(num_open_ports), str(round(scan_time, 2)))
	report += "%s ports scanned per second\n" % (str(math.floor( pow(2, 16) / scan_time ))[:-2])
	print report


def main():
	parser = argparse.ArgumentParser(description = 'Probes all TCP ports on a targeted host, attempting to \
		establish a connection at each port. Reports all ports that accept a connection')

	parser.add_argument('-t', '--target', type = str, help = 'hostname of machine to be scanned')

	args = parser.parse_args()

	scan_ports(args.target)

if __name__ == '__main__':
	main()

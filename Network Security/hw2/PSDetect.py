import argparse
import netifaces

from scapy.all import *
import time
import sys

class PSDetect():
    def __init__(self, interface):
        self.interface = interface
        self.my_ip = self.find_my_ip(interface)

        self.packet_count = 0
        self.source_ip = ''
        self.sniffed = []

        self.detected_ips = {}
        self.timer = 180


    def find_my_ip(self, interface):
        return netifaces.ifaddresses('eth1')[2][0]['addr']

    def custom_action(self, packet):
        if packet[0][1].src != self.my_ip:
            if self.packet_count == 0:
                self.suspect_ip = packet[0][1].src

                if self.suspect_ip not in self.detected_ips:
                    self.detected_ips[self.suspect_ip] = False

            if packet[0][1].src == self.suspect_ip:
                self.sniffed.append(packet[0][2].dport)
                
            self.packet_count += 1


    def sniff_packets(self):
        #print 'Sniffing for incoming packets...'

        sniff(iface = self.interface, filter = 'tcp', count = 30, timeout = 5, \
            prn = self.custom_action, store = False)

        """
        print 'Sniffing complete!\n'

        for packet, info in enumerate(self.sniffed):
            print 'Packet #%s: %s' % (packet + 1, info)
        """


    def verify_detection(self):
        if self.packet_count and len(self.sniffed) == 15:
            detected = True

            for i in range(1, 15):
                if self.sniffed[i] != self.sniffed[i - 1] + 1:
                    detected = False

            if detected == True and self.detected_ips[self.suspect_ip] == False:
                print('Scanner detected. The scanner originated from host %s' % self.suspect_ip)
                self.detected_ips[self.suspect_ip] = True

        self.packet_count = 0
        self.suspect_ip = ''
        self.sniffed = []


    def detect(self):
        print 'Sniffing for suspicious activity...'

        while True:
            t0 = time.time()

            self.sniff_packets()
            self.verify_detection()

            self.timer -= time.time() - t0

            if self.timer <= 0:
                self.detected_ips.clear()
                self.timer = 180


def main():
    parser = argparse.ArgumentParser(description = 'Sniffs for packets on local machine ethernet frame and detects \
        whether an adversary is attempting to scan for open ports on the machine')

    parser.add_argument('-i', '--interface', type = str, help = 'ethernet frame to listen to')

    args = parser.parse_args()

    try:
        detector = PSDetect(args.interface)
        detector.detect()

    except KeyboardInterrupt:
        sys.exit(0)


if __name__ == '__main__':
    main()
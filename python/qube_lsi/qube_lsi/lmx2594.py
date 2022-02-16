
import qube_lsi.udpsendrecv

class LMX2594:

    def __init__(self, addr, path, chip):
        self.handle = qube_lsi.udpsendrecv.UDPSendRecv(addr, path)
        self.chip = chip

    def read_freq_100M(self):
        return self.handle.read_lmx2594(self.chip, 0x24)

    def write_freq_100M(self, value):
        return self.handle.write_lmx2594(self.chip, 0x24, value)

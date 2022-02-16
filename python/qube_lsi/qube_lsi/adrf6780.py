
import qube_lsi.udpsendrecv

class ADRF6780:

    def __init__(self, addr, path, chip):
        self.handle = qube_lsi.udpsendrecv.UDPSendRecv(addr, path)
        self.chip = chip

    def read_mode(self):
        return self.handle.read_adrf6780(self.chip, 0x5)

    def set_usb(self):
        return self.handle.write_adrf6780(self.chip, 0x5, 0x0000)

    def set_lsb(self):
        return self.handle.write_adrf6780(self.chip, 0x5, 0x0400)

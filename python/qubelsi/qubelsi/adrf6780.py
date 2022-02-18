import qubelsi.udpsendrecv

class ADRF6780:

    def __init__(self, addr, path, chip):
        self.handle = qubelsi.udpsendrecv.UDPSendRecv(addr, path)
        self.chip = chip

    def read_mode(self):
        return self.handle.read_adrf6780(self.chip, 0x5)

    def set_usb(self):
        return self.handle.write_adrf6780(self.chip, 0x5, 0x0000)

    def set_lsb(self):
        return self.handle.write_adrf6780(self.chip, 0x5, 0x0400)

    def write_value(self, addr, value):
        return self.handle.write_adrf6780(self.chip, addr, value)

    def read_value(self, addr):
        return self.handle.read_adrf6780(self.chip, addr)

    def do_init(self, message_out=False, usb_mode=False):
        self.write_value(0x0, 0x4000)
        self.write_value(0x0, 0x0000)
        self.write_value(0x3, 0x0167)
        if usb_mode:
            self.write_value(0x5, 0x0000)
        else:            
            self.write_value(0x5, 0x0400)

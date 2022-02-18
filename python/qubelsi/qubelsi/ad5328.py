
import qubelsi.udpsendrecv

class AD5328:

    def __init__(self, addr, path):
        self.handle = qubelsi.udpsendrecv.UDPSendRecv(addr, path)

    def write_values(self, values):
        for i,v in enumerate(value[:8]):
            self.handle.write_ad5328(i, v)
        return values
            
    def write_value(self, addr, value):
        return self.handle.write_ad5328(addr, value)

    def do_init(self, message_out=False):
        self.write_value(0x8, 0x00C) # range 0--3.3V (buffered,Vref)
        self.write_value(0x0, 0x800) # output-A 1.65V
        self.write_value(0x1, 0x800) # output-B 1.65V
        self.write_value(0x2, 0x800) # output-C 1.65V
        self.write_value(0x3, 0x800) # output-D 1.65V
        self.write_value(0x4, 0x800) # output-E 1.65V
        self.write_value(0x5, 0x800) # output-F 1.65V
        self.write_value(0x6, 0x800) # output-G 1.65V
        self.write_value(0x7, 0x800) # output-H 1.65V
        self.write_value(0xA, 0x002) # /LDAC single update(必要なのでは？？？ by miyo)
        return 0

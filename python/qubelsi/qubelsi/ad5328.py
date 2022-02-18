
import qubelsi.udpsendrecv

class AD5328:

    def __init__(self, addr, path):
        self.handle = qubelsi.udpsendrecv.UDPSendRecv(addr, path)

    def write_values(self, values):
        for i,v in enumerate(value[:8]):
            self.handle.write_adrf6780(i, v)
        return values
            
    def write_value(self, index, value):
        return self.handle.write_adrf6780(index, value)

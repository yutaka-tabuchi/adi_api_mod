import qube_lsi.udpsendrecv

class GPIO:

    def __init__(self, addr, path):
        self.handle = qube_lsi.udpsendrecv.UDPSendRecv(addr, path)

    def write_value(self, value):
        return self.handle.write_gpio(value)

    def read_value(self):
        return self.handle.read_gpio()

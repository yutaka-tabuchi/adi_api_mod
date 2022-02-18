import qubelsi.udpsendrecv
import qubelsi.ad9082
import qubelsi.lmx2594
import qubelsi.adrf6780
import qubelsi.ad5328
import qubelsi.gpio

class Qube:
    
    def __init__(self, addr, path):
        self.ad9082 = [qubelsi.ad9082.AD9082(addr, path, i) for i in range(2)]
        self.lmx2594 = [qubelsi.lmx2594.LMX2594(addr, path, i) for i in range(8)]
        self.lmx2594_ad9082 = [qubelsi.lmx2594.LMX2594(addr, path, i+8) for i in range(2)]
        self.adrf6780 = [qubelsi.adrf6780.ADRF6780(addr, path, i) for i in range(8)]
        self.ad5328 = qubelsi.ad5328.AD5328(addr, path)
        self.gpio = qubelsi.gpio.GPIO(addr, path)



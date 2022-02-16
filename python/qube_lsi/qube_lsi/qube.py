import qube_lsi.udpsendrecv
import qube_lsi.ad9082
import qube_lsi.lmx2594
import qube_lsi.adrf6780
import qube_lsi.ad5328
import qube_lsi.gpio

class Qube:
    
    def __init__(self, addr, path):
        self.ad9082 = [qube_lsi.ad9082.AD9082(addr, path, i) for i in range(2)]
        self.lmx2594 = [qube_lsi.lmx2594.LMX2594(addr, path, i) for i in range(8)]
        self.lmx2594_ad9082 = [qube_lsi.lmx2594.LMX2594(addr, path, i+8) for i in range(2)]
        self.adrf6780 = [qube_lsi.adrf6780.ADRF6780(addr, path, i) for i in range(8)]
        self.ad5328 = qube_lsi.ad5328.AD5328(addr, path)
        self.gpio = qube_lsi.gpio.GPIO(addr, path)



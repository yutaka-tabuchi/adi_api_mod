import os
import subprocess

import qubelsi.udpsendrecv
import qubelsi.ad9082
import qubelsi.lmx2594
import qubelsi.adrf6780
import qubelsi.ad5328
import qubelsi.gpio

class Qube:
    
    def __init__(self, addr, path):
        self.path = path
        self.ad9082 = [qubelsi.ad9082.AD9082(addr, path, i) for i in range(2)]
        self.lmx2594 = [qubelsi.lmx2594.LMX2594(addr, path, i) for i in range(8)]
        self.lmx2594_ad9082 = [qubelsi.lmx2594.LMX2594(addr, path, i+8) for i in range(2)]
        self.adrf6780 = [qubelsi.adrf6780.ADRF6780(addr, path, i) for i in range(8)]
        self.ad5328 = qubelsi.ad5328.AD5328(addr, path)
        self.gpio = qubelsi.gpio.GPIO(addr, path)
        self.bitfile = 'as is'
        self.rf_type = 'as is'

    def do_init(self, rf_type='A', bitfile=None, message_out=False):
        self.rf_type = rf_type
        # LMX2594 for ctrl/readout
        if message_out:
            print("init LMX2594 for ctrl/readout")
        for i, c in enumerate(self.lmx2594):
            if (rf_type == 'A') and (i == 0 or i == 1 or i == 6 or i == 7):
                c.do_init(readout_mode=True, message_out=message_out)
            else:
                c.do_init()
        # LMX2594 for ad9082
        if message_out:
            print("init LMX2594 for AD9082")
        for c in self.lmx2594_ad9082:
            c.do_init(ad9082_mode=True, message_out=message_out)
        # ADRF6780
        if message_out:
            print("init ADRF6780")
        for c in self.adrf6780:
            c.do_init(message_out=message_out)
        # AD5328
        if message_out:
            print("init AD5328")
        self.ad5328.do_init(message_out=message_out)
        # GPIO fo RF-switch
        if message_out:
            print("init GPIOs for RF-switches")
        self.gpio.do_init(message_out=message_out)
        # FPGA configuration
        if not bitfile is None:
            self.bitfile = bitfile
            if message_out:
                print("init FPGA")
            os.environ['BITFILE'] = bitfile
            commands = ["vivado", "-mode", "batch", "-source", "{}/config.tcl".format(self.path)]
            ret = subprocess.check_output(commands , encoding='utf-8')
            if message_out:
                print(ret)
        # AD9082
        if message_out:
            print("init AD9082")
        for c in self.ad9082:
            c.do_init(message_out=message_out)

        if message_out:
            print("QuBE Type={0}, BITFILE={1}".format(self.rf_type, self.bitfile))

import os
import subprocess

import qube_lsi.udpsendrecv

class AD9082:

    def __init__(self, addr, path, chip):
        self.addr = addr
        self.chip = str(chip)
        self.path = path
        self.handle = qube_lsi.udpsendrecv.UDPSendRecv(addr, path)

    def _setenv(self):
        os.environ['TARGET_ADDR'] = self.addr
        os.environ['AD9082_CHIP'] = self.chip
    
    def read_info(self):
        self._setenv()
        ret = subprocess.check_output("{}/print_ad9082_info".format(self.path), encoding='utf-8')
        return ret

    def set_nco(self, freq, ch, adc_mode=False, fine_mode=False):
        self._setenv()
        cmd = ["{}/set_nco".format(self.path),
               "--channel={}".format(ch),
               "--freq={}".format(freq),
        ]
        if adc_mode:
            cmd.append("--adc_mode")
        if fine_mode:
            cmd.append("--fine_mode")
        ret = subprocess.check_output(cmd, encoding='utf-8')
        return ret

    def write_value(self, addr, value):
        return self.handle.write_ad9082(self.chip, addr, value)

    def read_value(self, addr):
        return self.handle.read_ad9082(self.chip, addr)

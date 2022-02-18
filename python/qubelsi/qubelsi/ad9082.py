import os
import subprocess

import qubelsi.udpsendrecv

class AD9082:

    def __init__(self, addr, path, chip):
        self.addr = addr
        self.chip = str(chip)
        self.path = path
        self.handle = qubelsi.udpsendrecv.UDPSendRecv(addr, path)

    def _setenv(self):
        os.environ['TARGET_ADDR'] = self.addr
        os.environ['AD9082_CHIP'] = self.chip
    
    def read_info(self):
        self._setenv()
        ret = subprocess.check_output("{}/src/print_ad9082_info".format(self.path), encoding='utf-8')
        return ret

    def set_nco(self, freq, ch, adc_mode=False, fine_mode=False):
        self._setenv()
        cmd = ["{}/src/set_nco".format(self.path),
               "--channel={}".format(ch),
               "--freq={}".format(freq),
        ]
        if adc_mode:
            cmd.append("--adc_mode")
        if fine_mode:
            cmd.append("--fine_mode")
        ret = subprocess.check_output(cmd, encoding='utf-8')
        return ret

    def get_jesd_status(self):
        self._setenv()
        ret = subprocess.check_output("{}/src/get_jesd_status".format(self.path), encoding='utf-8')
        return [x for x in [x.split('=') for x in ret.split('\n')] if len(x) == 2]

    def write_value(self, addr, value):
        return self.handle.write_ad9082(self.chip, addr, value)

    def read_value(self, addr):
        return self.handle.read_ad9082(self.chip, addr)

    def do_init(self, message_out=False):
        self._setenv()
        ret = subprocess.check_output("{}/v1.0.6/src/hello".format(self.path), encoding='utf-8')
        if message_out:
            print(ret)

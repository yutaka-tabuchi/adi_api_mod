import os
import subprocess

class UDPSendRecv:

    def __init__(self, addr, path):
        self.addr = addr
        self.cmd = "{}/udpsendrecv".format(path)

    def _setenv(self):
        os.environ['TARGET_ADDR'] = self.addr

    def _sendrecv(self, commands):
        self._setenv()
        ret = subprocess.check_output(commands, encoding='utf-8')
        return int(ret.split(":")[1].strip(), 16)

    def read_ad9082(self, chip, addr):
        commands = [self.cmd]
        commands.append("ad9082")
        commands.append("r")
        commands.append(str(chip))
        commands.append("{:04x}".format(addr))
        commands.append("00") # dummy value
        return self._sendrecv(commands)
    
    def read_adrf6780(self, chip, addr):
        commands = [self.cmd]
        commands.append("adrf6780")
        commands.append("r")
        commands.append(str(chip))
        commands.append("{:04x}".format(addr))
        commands.append("00") # dummy value
        return self._sendrecv(commands)

    def read_lmx2594(self, chip, addr):
        commands = [self.cmd]
        commands.append("lmx2594")
        commands.append("r")
        commands.append(str(chip))
        commands.append("{:04x}".format(addr))
        commands.append("00") # dummy value
        return self._sendrecv(commands)
    
    def read_ad5328(self, addr):
        commands = [self.cmd]
        commands.append("ad5328")
        commands.append("r")
        commands.append("{:04x}".format(addr))
        commands.append("00") # dummy value
        return self._sendrecv(commands)
    
    def read_gpio(self):
        commands = [self.cmd]
        commands.append("gpio")
        commands.append("r")
        commands.append("00") # dummy value
        return self._sendrecv(commands)

    def write_ad9082(self, chip, addr, value):
        commands = [self.cmd]
        commands.append("ad9082")
        commands.append("w")
        commands.append(str(chip))
        commands.append("{:04x}".format(addr))
        commands.append("{:02x}".format(value))
        return self._sendrecv(commands)
    
    def write_adrf6780(self, chip, addr, value):
        commands = [self.cmd]
        commands.append("adrf6780")
        commands.append("w")
        commands.append(str(chip))
        commands.append("{:04x}".format(addr))
        commands.append("{:02x}".format(value))
        return self._sendrecv(commands)

    def write_lmx2594(self, chip, addr, value):
        commands = [self.cmd]
        commands.append("lmx2594")
        commands.append("w")
        commands.append(str(chip))
        commands.append("{:04x}".format(addr))
        commands.append("{:02x}".format(value))
        return self._sendrecv(commands)
    
    def write_ad5328(self, addr, value):
        commands = [self.cmd]
        commands.append("ad5328")
        commands.append("w")
        commands.append("{:04x}".format(addr))
        commands.append("{:02x}".format(value))
        return self._sendrecv(commands)
    
    def write_gpio(self, value):
        commands = [self.cmd]
        commands.append("gpio")
        commands.append("w")
        commands.append("{:02x}".format(value))
        return self._sendrecv(commands)

import qubelsi.udpsendrecv

class LMX2594:

    def __init__(self, addr, path, chip):
        self.handle = qubelsi.udpsendrecv.UDPSendRecv(addr, path)
        self.chip = chip

    def read_freq_100M(self):
        return self.handle.read_lmx2594(self.chip, 0x24)

    def write_freq_100M(self, value):
        return self.handle.write_lmx2594(self.chip, 0x24, value)

    def write_value(self, addr, value):
        return self.handle.write_lmx2594(self.chip, addr, value)

    def read_value(self, addr):
        return self.handle.read_lmx2594(self.chip, addr)

    def do_init(self, message_out=False, readout_mode=False, ad9082_mode=False):
        
        if ad9082_mode:
            self.write_value(0x00, 0x6612) # R6 [14]VCO_PHASE_SYNC=0
        else:
            self.write_value(0x00, 0x2612) # R6 [14]VCO_PHASE_SYNC=0, [9]OUT_MUTE=1
        self.write_value(0x00, 0x2610)
        self.read_value(0x00)
        
        self.write_value(0x4E, 0x0001)
        self.write_value(0x4D, 0x0000)
        self.write_value(0x4C, 0x000C)
        self.write_value(0x4B, 0x0840)
        self.write_value(0x4A, 0x0000) # R74 [15:2] SYSREF_PULSE_COUNT=0
        self.write_value(0x49, 0x003F)
        
        if ad9082_mode:
            self.write_value(0x48, 0x001F) # R72 [10:0] SYSREF_DIV
            self.write_value(0x47, 0x008D) # R71 [7:5]SYSREF_DIV_PRE=4='Divided by 4', [3]SYSREF_EN=1, [2]SYSREF_REPEAT=1
        else:
            self.write_value(0x48, 0x0000) # R72 [10:0] SYSREF_DIV
            self.write_value(0x47, 0x0081) # R71 [7:5]SYSREF_DIV_PRE=4='Divided by 4', [3]SYSREF_EN=0, [2]SYSREF_REPEAT=0
        
        self.write_value(0x46, 0xC350)
        self.write_value(0x45, 0x0000)
        self.write_value(0x44, 0x03E8)
        self.write_value(0x43, 0x0000)
        self.write_value(0x42, 0x01F4)
        self.write_value(0x41, 0x0000)
        self.write_value(0x40, 0x1388)
        self.write_value(0x3F, 0x0000)
        self.write_value(0x3E, 0x0322)
        self.write_value(0x3D, 0x00A8)
        self.write_value(0x3C, 0x03E8)
        self.write_value(0x3B, 0x0001)

        if ad9082_mode:
            self.write_value(0x3A, 0x0401) # R58 [15]IGNORE=0, [14]HYST=0, [13:12]INPIN_LVL=0=V
        else:
            self.write_value(0x3A, 0x8001) # R58 [15]IGNORE=0, [14]HYST=0, [13:12]INPIN_LVL=0=V
        
        self.write_value(0x39, 0x0020)
        self.write_value(0x38, 0x0000)
        self.write_value(0x37, 0x0000)
        self.write_value(0x36, 0x0000)
        self.write_value(0x35, 0x0000)
        self.write_value(0x34, 0x0820)
        self.write_value(0x33, 0x0080)
        self.write_value(0x32, 0x0000)
        self.write_value(0x31, 0x4180)
        self.write_value(0x30, 0x0300)
        self.write_value(0x2F, 0x0300)

        if ad9082_mode:
            self.write_value(0x2E, 0x07FE) # R46 [1:0] OUTB_MUX=2
        else:
            self.write_value(0x2E, 0x07FD) # R46 [1:0] OUTB_MUX=1(=VCO)
        
        if readout_mode:
            self.write_value(0x2D, 0xC8FF) # R45 [5:0] OUTB_PWR=1F
        else: # ctrl_mode
            self.write_value(0x2D, 0xC8DF) # R45
        
        if ad9082_mode:
            self.write_value(0x2C, 0x3220) # R44  3220
        elif readout_mode:
            self.write_value(0x2C, 0x3220) # R44  3220
        else: # ctrl_mode
            self.write_value(0x2C, 0x32A0) # R44
        
        self.write_value(0x2B, 0x0000)
        self.write_value(0x2A, 0x0000)
        self.write_value(0x29, 0x0000)
        self.write_value(0x28, 0x0000)
        self.write_value(0x27, 0x0001)
        self.write_value(0x26, 0x0000)
        self.write_value(0x25, 0x0204)

        if ad9082_mode:
            self.write_value(0x24, 0x001e)
        else:
            self.write_value(0x24, 0x0078)
            
        self.write_value(0x23, 0x0004)
        self.write_value(0x22, 0x0000)
        self.write_value(0x21, 0x1E21)
        self.write_value(0x20, 0x0393)
        self.write_value(0x1F, 0x43EC)
        self.write_value(0x1E, 0x318C)
        self.write_value(0x1D, 0x318C)
        self.write_value(0x1C, 0x0488)
        self.write_value(0x1B, 0x0002)
        self.write_value(0x1A, 0x0DB0)
        self.write_value(0x19, 0x0C2B)
        self.write_value(0x18, 0x071A)
        self.write_value(0x17, 0x007C)
        self.write_value(0x16, 0x0001)
        self.write_value(0x15, 0x0401)
        self.write_value(0x14, 0xC848)
        self.write_value(0x13, 0x27B7)
        self.write_value(0x12, 0x0064)
        self.write_value(0x11, 0x00FA)
        self.write_value(0x10, 0x0080)
        self.write_value(0x0F, 0x064F)
        self.write_value(0x0E, 0x1E10)
        self.write_value(0x0D, 0x4000)
        self.write_value(0x0C, 0x5001)
        self.write_value(0x0B, 0x0018)
        self.write_value(0x0A, 0x10D8)
        self.write_value(0x09, 0x0604)
        self.write_value(0x08, 0x2000)
        self.write_value(0x07, 0x00B2)
        self.write_value(0x06, 0xC802)
        self.write_value(0x05, 0x00C8)
        self.write_value(0x04, 0x1B43)
        self.write_value(0x03, 0x0642)
        self.write_value(0x02, 0x0500)
        self.write_value(0x01, 0x080B)

        if ad9082_mode:
            self.write_value(0x00, 0x6618)
            self.write_value(0x22, 0x0000)
            self.write_value(0x24, 0x001e)
            self.write_value(0x26, 0x0000)
            self.write_value(0x27, 0x0064)
            self.write_value(0x2A, 0x0000)
            self.write_value(0x2B, 0x0000)
            self.write_value(0x00, 0x6618)
        else:
            self.write_value(0x00, 0x2618)
        
        return self.read_value(0x00)

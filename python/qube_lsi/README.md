# API to manipulate LSIs in QuBE

## Setup

For example,

```
% cd ${TMPDIR}
% git clone https://github.com/qiqb-osaka/adi_api_mod # use HTTPS
% git clone git@github.com:qiqb-osaka/adi_api_mod.git # or use SSH
% cd ${PIPENVDIR}
% pipenv shell
% pipenv install ${TMPDIR}/python/qube_lsi
```

## Example

```
% cd ${PIPENVDIR}
% pipenv shell
% python
>>> import qube_lsi.qube
>>> q = qube_lsi.qube.Qube("10.5.0.14", "/home/miyo/adi_api_mod/src")
>>> q
<qube_lsi.qube.Qube object at 0x7f22e6d2d880>
>>> q.ad9082
[<qube_lsi.ad9082.AD9082 object at 0x7f22e6d2dbb0>, <qube_lsi.ad9082.AD9082 object at 0x7f22e6d2dc70>]
>>> print(q.ad9082[0].read_info())
CHIP_TYPE     = 0f
PROD_ID_LSB   = 82
PROD_ID_MSB   = 90
CHIP_GRADE    = 23
SPI_REVISION  = 01
VENDER_ID_LSB = 56
VENDER_ID_MSB = 04
CHIP_TYPE     = 0f
PROD_ID_LSB   = 82
PROD_ID_MSB   = 90
CHIP_GRADE    = 23
SPI_REVISION  = 01
VENDER_ID_LSB = 56
VENDER_ID_MSB = 04

>>> q.lmx2594
[<qube_lsi.lmx2594.LMX2594 object at 0x7f22e6d2dca0>, ..., <qube_lsi.lmx2594.LMX2594 object at 0x7f22e6c69040>]
>>> q.lmx2594_ad9082
[<qube_lsi.lmx2594.LMX2594 object at 0x7f22e6d2da90>, <qube_lsi.lmx2594.LMX2594 object at 0x7f22e6c691c0>]
>>> q.adrf6780
[<qube_lsi.adrf6780.ADRF6780 object at 0x7f22e6c69100>, ..., <qube_lsi.adrf6780.ADRF6780 object at 0x7f22e6c697c0>]
>>> q.ad5328
<qube_lsi.ad5328.AD5328 object at 0x7f22e6c69280>
>>> q.gpio
<qube_lsi.gpio.GPIO object at 0x7f22e6c69910>
>>> q.gpio.read_value()
16383
>>> 

```


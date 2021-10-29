# QuBE内LSI設定ツール

## Build

```
cd src
make
```

## Example

```
AD9082_CHIP=1 TARGET_ADDRESS=10.5.0.1 ./hello
```

## 設定スクリプト・プログラム

- 環境変数 `TARGET_ADDR` で，対象のQuBEユニットのIPアドレスを指定して使う

init.sh
:  AD5328，ADRF6780，LMX2594のデフォルト設定をするスクリプト

ad5328.sh
:  AD5328の出力電圧を設定するスクリプト

ad9082-init.sh
:  AD9082の初期化をするスクリプト．2つのAD9082に対してhelloを実行する

adrf6780.sh
:  ADRF6780の設定をするスクリプト

lmx2594_120_LO.sh
:  LO用(SYSREFを出力しない)のLMX2594の設定用スクリプト

lmx2594_120_LO_s.sh ID
:  指定したIDのLMX2594にパラメタを設定するスクリプト

lmx2594_120_a.sh
:  AD9082用のLMX2594のパラメタを設定するスクリプト

lmx2594_f_r.sh ID
:  指定したIDのLMX2594の周波数設定レジスタを読み出すスクリプト

lmx2594_f_w.sh ID VAL
:  指定したIDのLMX2594の周波数設定レジスタ N を VAL にセットするスクリプト

rfswitch.sh
:  RFスイッチのON/OFFをセットする

set_nco VAL
:  AD9082のNCOの周波数をVALにセットする．VALは周波数(たとえば2GHzなら 2000000000)
:  どちらのAD9082に値をセットしたいかは環境変数 `AD9082_CHIP` で指定

udpsendrecv
:  QuBEの個々のLSIに設定を流し込むプリミティブのプログラム．
:  単に `./updsendrecv` と実行するとヘルプが表示される．

hello
:  AD9082のパラメタをセットする．
:  どちらのAD9082に値をセットしたいかは環境変数 `AD9082_CHIP` で指定．
:  `ad9082-init.sh` で実行される本体

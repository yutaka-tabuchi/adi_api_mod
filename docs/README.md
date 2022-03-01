- QuBE version 0.1
- 2021年10月20日

# QuBEユニットとは

QuBEユニットは，8系統の出力と2系統の入力を搭載する量子コンピュータ制御用の信号処理装置です．
内蔵するFPGA内のメモリに書き込んだデータを10GHz帯(一部，アナログコンポーネントの構成により20GHz帯)マイクロ波として送信，あるいは受信したマイクロ波をFPGA内のメモリに格納することができます．

搭載するLSIのパラメタおよび，内蔵するRFアナログコンポーネントの構成により，対象とする量子素子に合わせた周波数帯域の信号の送受信が可能です．

# Quick Start Guide

## Setup environment

```
$ source /tools/Xilinx/Vivado/2020.1/settings64.sh
$ export TARGET_ADDR=10.5.0.4
```

`10.5.0.4` should be modified for the actual target unit.

## Setup parameters of LSIs

```
$ cd $HOME/adi_api_mod/src
$ ./init.sh
$ cd $HOME
```

## FPGA configuration

```
$ cd $HOME
$ export BITFILE=$HOME/work/\
$ alveo_u50_jesd204c_16lane_recv_reverse_ref250/\
$ prj/au50_jesd204c_16lane.runs/impl_1/my_top.bit
$ vivado -mode batch -source $HOME/config.tcl
```

Change the value of `$BITFILE` as you want to work on the FPGA.

## Setup parameters of AD9082s


```
$ cd $HOME/adi_api_mod/src
$ ./ad9082-init.sh
$ cd $HOME
```

各出力ポートから9GHzのマイクロ波が出力される様子をスペクトラムアナライザで確認することができます．
これは，AD9082で生成した3GHzとLMX2594で生成した12GHzをADRF6780でミキシング，下側の周波数を取り出したものです．

# OpenWSN on RIOT
This test demonstrates the [OpenWSN](https://github.com/openwsn-berkeley/openwsn-fw) full
stack (UDP, IPv6, RPL, 6TiSCH) running on RIOT. When flashed, it will initialize the stack
and provide the user with some minimal shell commands to:

- print the own IPv6 address `ifconfig`
- start a RPL root node `rplroot` (This is only available with `openwsn_serial`)
- change the UDP destination port `udp server start <port>`
- send a UDP packet `udp send <addr> <port> <data>`

Please note that this port is still in experimental status. For further information
about the port refer to the [pkg documentation](../../pkg/openwsn/doc.txt).

## Experimental setups

The following experiments act as a starting point for testing and debugging. Either
build and flash local nodes or incorporate the [FIT IoT-LAB](https://www.iot-lab.info/)
Testbed. Please check the ports [documentation](../../pkg/openwsn/doc.txt) for information
about supported hardware platforms. Because this port currently needs
[Openvisualizer](https://github.com/openwsn-berkeley/openvisualizer), you should
use `iotlab-m3-a8` nodes to be able to execute commands a root. The following
guide will describe the setup on iotlab but the same requirements apply locally.

If you want to disable channel hopping to speed up synchronization you can set
a fix channel:

    $ export CFLAGS=-DIEEE802154E_SINGLE_CHANNEL=17

### Launch an experiment

Assuming you have `iotlab-cli` installed, launch an experiment booking 3+
`iotlab-18-m3` nodes:

    $ iotlab-experiment submit -d 120 -l 3,archi=a8:at86rf231+site=saclay
    $ iotlab-experiment wait

    $ iotlab-experiment get --nodes

### Flash leaf node


Since multiple nodes where configured for the experiment `IOTLAB_NODE` needs
to be specified for every node

    $ IOTLAB_NODE=a8-101.saclay.iot-lab.info BOARD=iotlab-a8-m3 make -C tests/pkg_openwsn flash
    $ IOTLAB_NODE=a8-102.saclay.iot-lab.info BOARD=iotlab-a8-m3 make -C tests/pkg_openwsn flash

You can also start a terminal for these nodes

    $ IOTLAB_NODE=a8-102.saclay.iot-lab.info BOARD=iotlab-a8-m3 make -C tests/pkg_openwsn term

    main(): This is RIOT! (Version: 2020.04-devel-1649-g96fa9-pr_openwsn)
    OpenWSN UDP test
    You are running RIOT on a(n) iotlab-a8-m3 board.
    This board features a(n) stm32f1 MCU.
    > ifconfig
    ifconfig
    inet6 fe80::684:f665:106b:1114
    hwaddr short: 11:14    long: 06:84:F6:65:10:6B:11:14
    panid: CA:FE

    IEEE802154E sync: 0

    6tsch joined: 0

    NO RPL parent

### Flash root node

Same as above but with `openwsn_serial`:

    $ USEMODULE=openwsn_serial IOTLAB_NODE=a8-100.saclay.iot-lab.info BOARD=iotlab-a8-m3 make -C tests/pkg_openwsn flash

The code that goes into root nodes and normal nodes is mostly the same except for
`stdio`. To be able to interface with `openvisualizer` the node will need a
dedicated uart. For most `BOARD`s its easier to simply disable `stdio` so this
application include `stdio_null` by default.

We now need to setup openvisualizer, you will need to `ssh` to the `ssh` frontend:

    $ ssh <login>@<site>.iot-lab.info
    $ cd A8
    $ mkdir openwsn
    $ cd openwsn
    $ git clone https://github.com/openwsn-berkeley/openvisualizer
    $ ssh root@node-a8-<number>
    $ cd openvisualizer

We will now run openvisualizer with:

    $ scons runweb
        scons: Reading SConscript files ...

        ___ _ _ _ ___ _ _ 
        | . | ___ ___ ._ _ | | | |/ __>| \ |
        | | || . \/ ._>| ' || | | |\__ \| |
        `___'| _/\___.|_|_||__/_/ <___/|_\_|
        |_| openwsn.org

        scons: done reading SConscript files.
        ...
        18:12:56 INFO create instance
        18:12:56 INFO create instance
        18:12:56 INFO create instance
        OpenVisualizer
        web interface started at  0.0.0.0: 8080
        enter 'quit' to exit
        Available ports:  /dev/ttyUSB1

        OpenVisualizer  (type "help" for commands)
        > 

You should see `/dev/ttyUSB1`, if you dont exit `Openvisualizer` by tipping
`quit` and restart the process (NOTE you MUST exit with `quit`).

Declare that node as root:

    $ root /dev/ttyUSB1

Your node will now start sending beacons and other nodes will synchronize, and
join. On leaf nodes yous should see the following:

    > [IEE20154E]: synchronized
    [neighbors]: new neighbor rssi: -66
    [cjoin]: send join request
    [cjoin]: success
    [icmpv6rpl]: found better parent
    [neighbors]: new neighbor rssi: -55
    ifconfig
    ifconfig
    inet6 bbbb::684:f665:106b:1114
    hwaddr short: 11:14    long: 06:84:F6:65:10:6B:11:14
    panid: CA:FE

    IEEE802154E sync: 1

    6tsch joined: 1

    RPL rank: 2816
    RPL parent: 2A:BA:F7:65:10:6B:11:14
    RPL DODAG ID: bbbb::2aba:f765:106b:1114

The root node should soon start receiving RPL DAOs:

    received RPL DAO from bbbb:0:0:0:ab8:fc65:106b:1114
    - parents:
    bbbb:0:0:0:2aba:f765:106b:1114
    - children:
    bbbb:0:0:0:684:f665:106b:1114

    received RPL DAO from bbbb:0:0:0:684:f665:106b:1114
    - parents:
    bbbb:0:0:0:2aba:f765:106b:1114
    - children:
    bbbb:0:0:0:ab8:fc65:106b:1114

    received RPL DAO from bbbb:0:0:0:684:f665:106b:1114
    - parents:
    bbbb:0:0:0:2aba:f765:106b:1114
    - children:
    bbbb:0:0:0:ab8:fc65:106b:1114

Once DAOs for all nodes start being received the network is setup and you
should be able to send packets between nodes or ping from the host.

### Send UDP packets

On one node setup a udp-server:

    ifconfig
    inet6 bbbb::684:f665:106b:1114
    hwaddr short: 11:14    long: 06:84:F6:65:10:6B:11:14
    panid: CA:FE

    IEEE802154E sync: 1

    6tsch joined: 1

    RPL rank: 2816
    RPL parent: 2A:BA:F7:65:10:6B:11:14
    RPL DODAG ID: bbbb::2aba:f765:106b:1114
    > udp server start 3000
    udp server start 3000
    Set UDP server port to 3000

On the other node send udp messages

    > udp send bbbb::684:f665:106b:1114 3000 hello
    udp send bbbb::684:f665:106b:1114 3000 hello
    Send 5 byte over UDP to [bbbb::684:f665:106b:1114]:3000
    > msg.l2_sendDoneError: 0
    Send success

The first node should receive the message

    > Received 12 bytes on port 3000
    00000000  A6  28  00  00  00  02  00  68  65  6C  6C  6F .(.....hello

### PING from host

    $ ssh <login>@<site>.iot-lab.info
    $ ssh root@node-a8-<number>

    $ root@node-a8-102:~# ping6 bbbb::684:f665:106b:1114
    PING bbbb::684:f665:106b:1114(bbbb::684:f665:106b:1114) 56 data bytes
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=4 ttl=64 time=6343 ms
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=6 ttl=64 time=6316 ms
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=15 ttl=64 time=5319 ms

Roundtrip and message loss could be high depending on the interval

    root@node-a8-102:~# ping6 bbbb::684:f665:106b:1114 -i 5
    PING bbbb::684:f665:106b:1114(bbbb::684:f665:106b:1114) 56 data bytes
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=1 ttl=64 time=3505 ms
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=2 ttl=64 time=4621 ms
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=3 ttl=64 time=3659 ms
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=4 ttl=64 time=4672 ms
    64 bytes from bbbb::684:f665:106b:1114: icmp_seq=5 ttl=64 time=3674 ms
    ^C
    --- bbbb::684:f665:106b:1114 ping statistics ---
    6 packets transmitted, 5 received, 16% packet loss, time 25003ms
    rtt min/avg/max/mdev = 3505.897/4026.642/4672.372/510.070 ms

### Troubleshooting

- openvisualizer doesn't exit gracefully if not exited with `quit`. If you
  ctrl+c, dangling process will be left you will have to manually kill it:

    $ ps aux | grep openvisualizer
        root      2727 34.4  6.9 198132 17648 pts/0    Sl+  18:12   5:21 python bin/openVisualizerWeb.py -a /home/root/A8/openwsn/openvisualizer/build/runui -H 0.0.0.0 -p 8080
        root      2790  0.0  0.4   2740  1224 pts/1    S+   18:28   0:00 gr

    $ kill -9 2727

- openvisualizer doesn't detect serial ports that are not `/dev/ttyUSB*`, to
  patch this:

  $ vi, openvisualizer/moteProbe/moteProbe.py
    ....
    elif os.name=='posix':
            if platform.system() == 'Darwin':
                portMask = ['/dev/tty.usbserial-*']
            else:
                #portMask = ['/dev/ttyUSB*']
                portMask = ['/dev/ttyUSB*', '/dev/ttyACM*']
            for mask in portMask :
                serialports += [(s,BAUDRATE_GINA) for s in glob.glob(mask)]
    ....

pkg_libcsp
==========


Usage
=====


```
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link add dev vcan1 type vcan
sudo ip link set vcan0 up
sudo ip link set vcan1 up
```

```
sudo modprobe can-gw
sudo cangw -A -s vcan1 -d vcan0 -e
sudo cangw -A -s vcan0 -d vcan1 -e
```

```
VCAN_IFNAME=vcan0 make -C tests/pkg_libcsp
```

```
VCAN_IFNAME=vcan1 make -C tests/pkg_libcsp
```

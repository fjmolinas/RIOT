## Decawave TWR_ALOHA Example

This example allows testing different two-way ranging algorithm between
two boards supporting a dw1000 device. This makes use of the uwb-core
pkg. This example is based on the twr-aloha example in
[uwb-apps](https://github.com/Decawave/uwb-apps/tree/master/apps/twr_aloha).

### Setup

1. Flash one node as the tag (Default configuration)

    $ make -C examples/twr_aloha/ flash term

2. Flash the second node as an anchor

    $ ROLE_DEFAULT=0x4 make -C examples/twr_aloha/ flash term

3. On the tag node begin ranging, you will see ranging estimations where
the "raz" field is "range" in meters.

```
 main(): This is RIOT! (Version: 2020.10-devel-1384-g5b7ad-wip/uwb-dw1000)
 pkg uwb-dw1000 + uwb-core test application
 {"utime": 49412,"exec": "/home/francisco/workspace/RIOT/examples/twr_aloha/control.c"}
 {"device_id"="deca0130","panid="DECA","addr"="1303","part_id"="cad11303","lot_id"="402c188"}
 {"utime": 49412,"msg": "frame_duration = 201 usec"}
 {"utime": 49412,"msg": "SHR_duration = 139 usec"}
 {"utime": 49412,"msg": "holdoff = 821 usec"}
 Node role: TAG
 {"utime": 120995,"c": 274,"uid": 4867,"ouid": 4660,"raz": [0.766359],"los": [1.000000]}
> range start
 range start
 Start ranging
 {"utime": 5214098,"c": 274,"uid": 4867,"ouid": 4660,"raz": [0.778875],"los": [1.000000]}
 {"utime": 5232368,"c": 274,"uid": 4867,"ouid": 4660,"raz": [0.777146],"los": [1.000000]}
 {"utime": 5250631,"c": 274,"uid": 4867,"ouid": 4660,"raz": [0.796009],"los": [1.000000]}
 {"utime": 5268894,"c": 274,"uid": 4867,"ouid": 4660,"raz": [0.756952],"los": [1.000000]}
 {"utime": 5287157,"c": 274,"uid": 4867,"ouid": 4660,"raz": [0.787994],"los": [1.000000]}
```

4. Trying different ranging algorithms

The method for ranging used is selected by modifying the mode variable
in the uwb_ev_cb function in main.c. By default, it will use one of the
modes available and setting the `mode` variable accordingly.

If multiple modes are enabled it will switch among them, to use only
one algorithm compile as follows:

    $ UWB_FIXED_TWR_ALGORITHM=UWB_DATA_CODE_SS_TWR make -C examples/twr_aloha/ flash term

The different algorithm options are:

- UWB_DATA_CODE_SS_TWR
- UWB_DATA_CODE_SS_TWR_EXT
- UWB_DATA_CODE_SS_TWR_ACK
- UWB_DATA_CODE_DS_TWR
- UWB_DATA_CODE_DS_TWR_EXT

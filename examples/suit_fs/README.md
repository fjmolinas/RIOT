# SUIT File System Update

## Requirements

1. Make sure an image for CoAP file server exists

```bash
make -C examples/gcoap_fs/ fatfs-image
```

1. Setup tap interfaces

```
sudo ${RIOTBASE}/dist/tools/tapsetup/tapsetup -c 2
sudo ip address add 2001:db8::1/64 dev tapbr0
```

## Test

`RIOT_TERM_START_DELAY=1 python examples/suit_fs/test.py`

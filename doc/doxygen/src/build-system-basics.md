Build System Basics                    {#build-system-basics}
============================

[TOC]

https://pad.inria.fr/p/np_dXL6aMg06esIaEMP

MAKEFILES                                                    {#introduction}
============



BOARDS                                                     {#introduction}
============



That is what I call a different "board" in RIOT.
In linux-arm, it would require a different device tree (or a really adapted device tree for the shields, like the beaglebone).

If you may need to have a device mapping, I call it a different board as we store this in the board/periph_conf
And if your board can use the default one, you must have checked the default works for your setup and not configure a different one.


MODULES AND FEATURES                            {#customize-build-system}
==========================

+ `RIOT_MAKEFILES_GLOBAL_PRE`: files parsed before the body of
  `$RIOTBASE/Makefile.include`
+ `RIOT_MAKEFILES_GLOBAL_POST`: files parsed after the body of
  `$RIOTBASE/Makefile.include`

The variables are a list of files that will be included by
`$RIOTBASE/Makefile.include`.
They will be handled as relative to the application directory if the path is
relative.


FEATURES
-----
-----


Providing a `FEATURE`
-----

`FEATURES_PROVIDED` are hardware (including toolchain) features that are available
for a board.

For a `FEATURE` to be provided by a `board` it must meet 3 criteria:

- Needs the "hardware" or needs BSP support (toolchain)
    - `stm32l152re` has an SPI periheral
- Need support in RIOT, needs an implementatio of an api to interact with the `hw`
    - `cpu/stm32l1/periph/spic.c` is implemented for `stm32l1`
- Exposed mapping and a configuration exposing this
    - `nucleo-l152re/include/periph_conf.h` has an SPI exposed on `PORT_X`

What is a `FEATURE`
-----

Whenever a `FEATURE` is used there should be at some level a HW requirements,
wether this is a `radio`, a `bus`, a specific core architecture. 

This is not a hard line, in some cases the line can be harder to establish than
others. There are complicated cases like `netif` since a network interface could
be fully implemented in software as a loop-back.

It's also important to note that a `FEATURE` does not mean there is a `MODULE`
with the same name. There could be many implementations for the same `FEATURE`.
The fact that many `FEATURES` translate directly into a `MODULE` is only by
convenience.

e.g.

    # all periph features correspond to a periph submodule
    USEMODULE += $(filter periph_%,$(FEATURES_USED))

In the end a `FEATURE` is a tool to specify invalid dependencies by specifying
`BOARD`.

MODULES
-----

Modules somehow refer to an implementation.

Variable Definitions                           {#variable definition}
=============================


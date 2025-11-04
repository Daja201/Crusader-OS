# Crusader-OS
Operating system based on nanobyte's yt tutorials "How to make an operating system". 

I used these packages on Debian:
  sudo apt install build-essential libgmp3-dev libmpc-dev libmpfr-dev texinfo mtools dosfstools libguestfs-tools qemu-system-x86
    (copied from nanobyteOS repository, may cointain more or less packages then needed)

I personally use qemu for emulation i686 processor:
  qemu-system-i386 -fda build/main_floppy.img

You need to make your own i686 cross compiler, or use some existing one like OpenWatcomV2 (im using some random precompiled from git but you can make your own)

v01:
  Basically plain copy from [NanobyteOS](https://github.com/nanobyte-dev/nanobyte_os) with some small changes

v02:
v03:
v04:
v05:
v06:

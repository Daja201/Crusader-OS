# Crusader-OS
Operating system based on nanobyte's yt tutorials "How to make an operating system". 

Use this on Debian:
sudo apt install build-essential libgmp3-dev libmpc-dev libmpfr-dev texinfo  mtools scons dosfstools libguestfs-tools qemu-system-x86

(copied from nanobyteOS repository, may cointain more or less packages then needed)

I personally use qemu for emulation:
qemu-system-i386 -fda build/main_floppy.img

You need to make your own i686 cross compiler, or use some existing one (im using some random precompiled from git but you can make your own)

ShadowsocksQt
================

[ss]: http://shadowsocks.org

[![Build Status](https://travis-ci.org/shadowsocks/libQtShadowsocks.svg?branch=master)](https://travis-ci.org/shadowsocks/libQtShadowsocks) <a href="https://copr.fedorainfracloud.org/coprs/librehat/shadowsocks/package/libQtShadowsocks/"><img src="https://copr.fedorainfracloud.org/coprs/librehat/shadowsocks/package/libQtShadowsocks/status_image/last_build.png" /></a>

Introduction
------------

`ShadowSocksQt` is a [shadowsocks][ss] modification, which has TUI/GUI Client & Server.

Written in C++ using Qt5 framework and [Botan](https://github.com/randombit/botan) library,
the required [Qt5](http://download.qt.io/official_releases/qt) version is 5.6 or later.

Building
--------

Dependencies
- [botan](https://github.com/randombit/botan)
  * $ pacman -S mingw-w64-i686-libbotan
  * $ pacman -S mingw-w64-x86_64-libbotan

- [bzar](http://zbar.sourceforge.net/)
  * $ sudo apt install libzbar-dev
  * $ pacman -S mingw-w64-i686-zbar
  * $ pacman -S mingw-w64-x86_64-zbar

- [libqrencode](https://fukuchi.org/works/qrencode)
  * $ sudo apt install libqrencode-dev
  * $ pacman -S mingw-w64-i686-qrencode
  * $ pacman -S mingw-w64-x86_64-qrencode

- [Qt]http://download.qt.io/official_releases/qt 5.6 or later
  * $ pacman -S mingw-w64-i686-qt5
  * $ pacman -S mingw-w64-i686-qt5-static
  * $ pacman -S mingw-w64-x86_64-qt5
  * $ pacman -S mingw-w64-x86_64-qt5-static

License
-------

![](https://www.gnu.org/graphics/gplv3-127x51.png)

You should also get your employer (if you work as a programmer) or
school, if any, to sign a "copyright disclaimer" for the program, if
necessary. For more information on this, and how to apply and follow
the GNU GPL, see <https://www.gnu.org/licenses/>.

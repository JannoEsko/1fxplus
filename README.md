Additional note before a proper readme:

For Linux distros where libbacktrace is not available as a package, build it yourself:
```
git clone https://github.com/ianlancetaylor/libbacktrace.git
cd libbacktrace
./configure
make
sudo make install
```

Adding a note here just so that when a real README is done, I won't forget it:
The VS project is linked with vcpkg folder C:\vcpkg - ensure you change that.
Also, it has a dependency on cURL: vcpkg install curl:x64-windows-static

Building on MSYS2/Linux, just ensure you have cURL libs installed.

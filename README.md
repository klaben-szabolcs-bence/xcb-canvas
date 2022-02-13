# xcb-canvas
Implements HTML Canvas on XCB

## Build
On Linux with `make libs` to only create the library file or `make example` to create the example program as well. \
Ubuntu: `sudo apt install build-essential libx11-xcb-dev pkg-config` to get required packages.

For testing, [cmocka](https://cmocka.org/) is used. Please make sure it is installed. You will probably have to compile the source code of it for yourself.


Windows: [Use VS Code with WSL2](https://docs.microsoft.com/en-us/windows/wsl/tutorials/wsl-vscode) and run the above mentioned lines for Linux.
If GUI programs don't show up, see [VcSRsv](https://sourceforge.net/projects/vcxsrv/) and this [SO answer](https://stackoverflow.com/a/61110604).

# APT manager

GUI for `apt update` command. Displays installed and available to update packages.

![apt-manager-gtk](https://github.com/Chukak/apt-manager-gtk/blob/main/apt-manager-gtk.png)


## Installation and Running

### Ubuntu 20.04/Debian 11

To install the `apt-manager-gtk` from the `.deb` package, download the release you need from [Releases](https://github.com/Chukak/apt-manager-gtk/releases).
```bash
dpkg -i apt-manager-gtk_<VERSION>_amd64.deb
```

To run the `apt-manager-gtk`:
```bash
sudo apt-manager-gtk # or apt-manager-gtk (update option not available)
```

<b>Note</b>: If you want to install `apt-manager-gtk` on `Debian 10`, you need to manually install the `libapt-pkg6.0` package.

## Building from sources (Only Linux)

```bash
cmake -DCMAKE_BUILD_TYPE=Release . && make && sudo make install
```

### Debug Build

Pass this option before running application [see more](https://gitlab.gnome.org/GNOME/gtkmm/-/issues/65): 
```bash
ASAN_OPTIONS=new_delete_type_mismatch=0 ./apt-manager-gtk
```

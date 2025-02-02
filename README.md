# horologion

- Calculates local prayer times
- Tracks time until next prayer in log file
- Notifies user a minute ahead, and optionally displays corresponding prayer in terminal

The notification depends on [herbe](https://github.com/dudik/herbe).

The prayers are in ```~/.local/share/horologion``` and they are in Church Slavonic. Edit them as needed. The log is in the same folder. You could, for example, tail it in a status bar.

## Installation
```sh
make && sudo make install
```

To run at start-up, add the following to your ~/.xinitrc or equivalent:
```sh
horologion &
```

## Settings
The location (determines sunrise/sunset) and terminal can be changed in config.h

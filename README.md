# horologion

- Calculates local prayer times
- Tracks time until next prayer and prints it every minute as "prayer_name in hh:mm"
- Notifies user a minute ahead, and optionally displays corresponding prayer in terminal

If the -F flag is set, the countdown string is printed to a named pipe. By default ```/tmp/bar/fifo_horologion ```.

You should change the variables in config.h before compilation according to your use case. The terminal for displaying prayers and the pipe path can also be edited there.

The notification depends on [herbe](https://github.com/dudik/herbe). Default: left-click to dismiss, right-click to display prayer. They are by default in Church Slavonic: edit the files in ```~/.local/share/horologion``` as you wish.


To install
```sh
make && make install
```
To uninstall
```sh
make clean && sudo make uninstall
```

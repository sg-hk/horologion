# horologion

- Calculates local prayer times
- Tracks time until next prayer and prints it every minute
- Notifies user a minute ahead, and optionally displays corresponding prayer in terminal

You should change the variables in config.h before compilation according to your use case.

The notification depends on [herbe](https://github.com/dudik/herbe). Default: left-click to dismiss, right-click to display prayer. They are by default in Church Slavonic: edit the files in ```~/.local/share/horologion``` as you wish.

## Installation
```sh
make && make install
```

You can update your status bar with the stdout print (which is date, time, and the prayer countdown) as below:
```sh
mkfifo /tmp/horologion
ctimer > /tmp/horologion &
while read line; do
    xsetroot -name "$line"
done < /tmp/horologion
```

Add the first two lines to your ```~/.xinirc``` or equivalent to run at start-up.

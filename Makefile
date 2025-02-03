CC = cc
CFLAGS = -Wall -Wextra -lm
SRC = horologion.c
TARGET = horologion

BINDIR = /usr/local/bin
TARGETDIR = $(HOME)/.local/share/horologion
PRAYER_FILES = matins first third sixth ninth vespers compline nocturns

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

install: $(TARGET)
	mkdir -p $(BINDIR) $(TARGETDIR)
	cp $(TARGET) $(BINDIR)/
	for file in $(PRAYER_FILES); do \
		cp prayers/$$file $(TARGETDIR)/; \
	done

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -rf $(TARGETDIR)

clean:
	rm -f $(TARGET)

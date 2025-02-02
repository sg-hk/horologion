CC = cc
CFLAGS = -Wall -Wextra -O2 -lm
SRC = horologion.c
TARGET = horologion

# Where to install the horologion binary
BINDIR = /usr/local/bin

# Where to copy the prayer text files
USERDATADIR = $(HOME)/.local/share/horologion

PRAYER_DIR = prayers
PRAYER_FILES = matins first third sixth ninth vespers compline nocturns

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

install: $(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR) ..."
	sudo mkdir -p $(BINDIR)
	sudo install -m 755 $(TARGET) $(BINDIR)/

	@echo "Copying prayer files to $(USERDATADIR) ..."
	mkdir -p $(USERDATADIR)
	for file in $(PRAYER_FILES); do \
		install -m 644 $(PRAYER_DIR)/$$file $(USERDATADIR)/; \
	done

	@echo "Install complete."
	@echo "Remember to add '$(TARGET) &' to your ~/.xinitrc or WM autostart."

uninstall:
	@echo "Removing $(BINDIR)/$(TARGET) ..."
	sudo rm -f $(BINDIR)/$(TARGET)
	@echo "No prayer files removed from $(USERDATADIR) (do that manually if desired)."
	@echo "Uninstall complete."

clean:
	rm -f $(TARGET)

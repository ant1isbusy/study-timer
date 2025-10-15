#!/bin/bash

# Variables
APP_NAME="timer"
EXEC_PATH="/usr/local/bin/$APP_NAME"
DESKTOP_ENTRY="/usr/share/applications/$APP_NAME.desktop"
FONT_PATH="JetBrainsMono-Medium.ttf"

echo -e "\e[1m\e[33mInstallation script running... \e[0m"
echo "Compiling the timer executable"
gcc timer.c -o timer.out -lSDL2 -lSDL2_ttf
echo -e "File compiled!\n"

echo "Installing the font locally for the user"
mkdir -p ~/.local/share/fonts
cp "$FONT_PATH" ~/.local/share/fonts/

echo -e "Refreshing the font cache\n"
fc-cache -fv ~/.local/share/fonts/

echo -e "\nInstalling the executable to $EXEC_PATH"
sudo cp timer.out "$EXEC_PATH"
sudo chmod +x "$EXEC_PATH"

cat <<EOF | sudo tee "$DESKTOP_ENTRY" >/dev/null
[Desktop Entry]
Name=Timer
Comment=ant1sbusy study timer
Exec=$EXEC_PATH
Icon=executable
Terminal=false
Type=Application
Categories=Utility;
EOF

update-desktop-database ~/.local/share/applications/

echo -e "\e[1m\e[32mInstallation complete! You can now search for and run 'Timer' from your applications.\e[0m"

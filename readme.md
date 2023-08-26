# A little simple BBS for the ESP32

This project started as a joke and a dare after CCCamp2023, to write a simple board on a LoLin32 Lite i bought there.

Please do not use this in any production environment, this is just for fun!

To use this first change the board in platformio.ini to what you have, then create and upload the LittleFS image. 
After that, for now, insert the SSID and its password you want to connect to in main.cpp. Compile, upload and enjoy.

TODO:
- Some way to delete posts
- Dynamic selection of the AP to connect to
- Better HTML, the current one should work with any browser tough

Uses:
- Arduino and its webserver library
- Sqlite3 Arduino library for ESP32 (https://github.com/siara-cc/esp32_arduino_sqlite3_lib) 

Pull requests welcome. :)
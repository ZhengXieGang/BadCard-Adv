# BadCard
<img src="https://github.com/VoidNoi/BadCard/blob/main/BADCARD.png" height="400">
BadUSB for the Cardputer with ducky script support and extra functionality

Ducky Script processing and executing ported from [arducky](https://github.com/Creased/arducky)

## Cardputer ADV support
BadCard works on both the original Cardputer and the **Cardputer ADV** since v1.7.3 ([#19](https://github.com/VoidNoi/BadCard/issues/19)).

The ADV keyboard is not a GPIO matrix like on the original Cardputer: it is a 56-key matrix behind a **TCA8418 I2C expander** (SDA/G8, SCL/G9, INT/G11, addr `0x34`). M5Stack added the reader for it in the M5Cardputer library **v1.1.1**, which picks the right keyboard driver automatically per board. If your firmware compiles but **no keypress is ever recognized on a Cardputer ADV**, you are building against an older M5Cardputer library — update it:

* **Arduino IDE**: Library Manager → search "M5Cardputer" → update to **1.1.1** or newer (also update M5Unified/M5GFX when asked).
* **PlatformIO**: use the included `platformio.ini`, which pins `M5Cardputer@^1.1.1`.
* If you build with an old library anyway, the compiler now emits a `#warning` about it.

## How to build
With [arduino-cli](https://arduino.github.io/arduino-cli/) and the esp32 core installed:

```sh
arduino-cli core install esp32:esp32
arduino-cli lib install "M5Cardputer@1.1.1" NimBLE-Arduino@2.5.1
./build.sh
```

The flashable artifacts land in `firmware/` — `BadCard.ino.merged.bin` is a single full flash image (flash it at offset `0x0` with `esptool.py write_flash`).

## FEATURES
* Bluetooth connection
* File editing (Thanks to [@guidoaguiar](https://github.com/guidoaguiar))
* Horizontal file navigation
* Multiple Keyboard layouts (en_US, es_ES, de_DE, pt_PT, fr_FR, sv_SE, it_IT, hu_HU, da_DK, pt_BR, en_GB, nb_NO, ja_JP).
* You can navigate, create and delete folders

You can contribute to BadCard by creating your own keyboard layouts following this guide: [How to create a keyboard layout for BadCard](https://gist.github.com/VoidNoi/4087bab5d67458a52fe22a574a5f7394)

<strong>If you're having trouble executing your payloads on Windows 11 try adding a <code>DELAY 500</code> at the end of your payload https://github.com/VoidNoi/BadCard/issues/1#issuecomment-2000571655</strong>

## How to use
You can place your payloads inside the `/BadCard` folder on your SD card (It will be created automatically after installing BadCard)
<br/>
Your files should use the `.txt` extension.

OR

You can make your own payloads in your cardputer with the <strong>"NEW FILE"</strong> option 
<details>
  <summary>
    <strong>Available commands</strong>
  </summary>
  
     (1) Commands without payload:
     - ENTER
     - MENU <=> APP
     - DOWNARROW <=> DOWN
     - LEFTARROW <=> LEFT
     - RIGHTARROW <=> RIGHT
     - UPARROW <=> UP
     - BREAK <=> PAUSE
     - CAPSLOCK
     - DELETE
     - END
     - ESC <=> ESCAPE
     - HOME
     - INSERT
     - NUMLOCK
     - PAGEUP
     - PAGEDOWN
     - PRINTSCREEN
     - SCROLLLOCK
     - SPACE
     - TAB
     - REPLAY (global commands aren't implemented)
     
     (2) Commands with payload:
     - DEFAULT_DELAY <=> DEFAULTDELAY (global commands aren't implemented.)
     - DELAY (+int)
     - STRING (+string)
     - GUI <=> WINDOWS (+char)
     - SHIFT (+char or key)
     - ALT (+char or key)
     - CTRL <=> CONTROL (+char or key)
     - REM (+string)
  
</details>

## File creating/editing controls

* You can navigate your file pressing <code>fn</code> and the <code>up</code>, <code>down</code>, <code>left</code> or <code>right</code> keys (aka <code>;</code>, <code>.</code>, <code>,</code> or <code>/</code>)
* When you're finished creating your file press <code>fn</code> and <code>esc</code>, add the name of your file and press enter (you don't need to add the <code>.txt</code> extension)
* Delete folders by pressing the <code>G0</code> button and enter for the selected folder

## About BLE
* Doesn't support special keys (ñ, ß, etc) like USB does
* It may skip some characters, especially characters that require modifier keys (SHIFT, ALTGR)

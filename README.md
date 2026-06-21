# Codex

The Codex is an attempt at an open-source, distraction-free writing device inspired somewhat by the AlphaSmart Neo (at least the display was). The Codex pairs a 40x4 character LCD with USB HID keyboard support, and SD card storage. For a more in-depth summary/breakdown, you can visit the page I have on my website: [Codex Web Page](https://adamjackson.dev/codex.html).

---

## Project Status

Codex is under active development. Current state:

**Working:**
- [x] LCD driver
  - Originally used a derivation of the Arduino LiquidCrystal library (noted below under [Libraries Used](#libraries-used) and [Acknowledgements](#acknowledgments)). I have since written a custom one from the ground up specific for this project.
- [x] USB HID keyboard input via TinyUSB
- [x] Gap buffer with insert/delete/backspace
- [x] Character, word, line, frame, and document navigation
- [x] Frame buffer rendering (LCD stays in sync with cursor position)
- [x] SD card read/write (FatFs via [no-OS-FatFS-SD-SPI-RPi-Pico](https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico))
- [x] Document save/load
- [x] File selection on boot
- [x] Tab handling (inserted as two spaces)
- [x] Home/End at line, frame, and document scope
- [x] Card detect

**In progress / planned:**
- [ ] Keyboard shortcut finalization
- [ ] Save new document/give file name
- [ ] PWM backlight brightness control (potentiometer on one of the analog pins + transistor)
- [ ] Rev 2+ PCB: LiPo + boost converter, switchable external/battery power input, finalized sockets for ribbon cables/harnesses
- [ ] Final Enclosure (currently a wedge shape)
- [ ] Documentation: build guide, BOM, update procedure, etc.

**Maybes**
- [ ] Ability to change file after startup

**Explicitly out of scope (for now):**
- Text selection / clipboard
- Undo/redo
- Rich text or formatting of any kind

---

## Hardware

### Bill of Materials (high level)

| Component | Notes |
|---|---|
| Raspberry Pi Pico (RP2040) | Mounted via socket currently, likely castellated edge pads on the final PCB |
| 40x4 ST7066U character LCD | Dual controller (EN1/EN2), 4-bit mode |
| USB OTG converter | USB port for keyboard |
| SD card socket | SPI mode |
| Slide potentiometers (x2) | Contrast (to LCD V0) and brightness (to analog GPIO) |
| BC547 NPN transistor | PWM backlight switching |
| TP61090 boost converter | LiPo → 5V (Rev 2+) |
| MCP73871 charge controller | LiPo charge circuit (Rev 2+) |
| L7805 linear regulator | Bench/external power path (Rev0-Rev2, won't be on Rev3) |
| SPDT switch | Selects external 5V vs. LiPo/boost power source (Rev2 only), acts as power switch (Rev0 & Rev1) |
| LiPo cell | (Rev 2+) |

### PCB Revisions

| Rev | Purpose | Status |
|---|---|---|
| **Breadboard** | Initial validation: LCD, USB host, SD card | Done | 
| **Rev 0** | First PCB, puts the breadboard on a dedicated PCB, adds a power switch, adds filtering capacitors | Done |
| **Rev 1** | Fixes mistakes from Rev0. Sill uses socketed footprints for SD card and Pico. Adds breakout pins for contrast/brightness potentiometers, adds PWM backlight transistor. | Board Designed |
| **Rev 2** | Dedicated SD card slot, finalized sockets for IDC, potentiometers, and LED power. Pico footprint will allow sockets _or_ solder directly via castellated holes. 5v linear regulator will remain, with an optional switch for swapping to using a 5v direct power source (planned as a Powerboost 1000 module). | Planned |
| **Rev 3** | Removal of 5v linear regulator. Move to all SMD components for assembly by fab house. | Planned |
| **???** | Anything beyond Rev3 will only happen if I make dumb mistakes (and I probably will).

## Firmware

Built with the **Raspberry Pi Pico SDK**

### Libraries used

- Originally used a derivation of [LiquidCrystal440](https://github.com/jurs/liquidcrystal440), which is in turn a derivation of the [Original Arduino Library](https://github.com/arduino-libraries/LiquidCrystal).
  - Now uses a driver that is purpose-written for this device 
- **TinyUSB** - USB HID host
- **no-OS-FatFS-SD-SPI-RPi-Pico** - SD card / FAT filesystem (submodule)

### Recovery

If a firmware update fails or the device won't boot, hold **BOOTSEL** while connecting the Pico to a computer via USB. It will appear as a USB mass storage device. Then drag and drop a known-good `.uf2` to restore it. This is a hardware bootloader built into the RP2040 and cannot be bricked by firmware alone.

---

## Enclosure

Designed in FreeCAD. It has a sort of wedge factor with the 40x4 LCD forming most of the angled top face. Slide potentiometers for brightness and contrast are mounted on the front, with the the keyboard USB port and SD card slot to the right of those. FCStd files are located `3dmodels/` (I'm just committing the whole blob since I don't really know a better way to do it for freecad files in git).

![enclosure](https://adamjackson.dev/images/codex/codex-new.svg)

---

## Keyboard Compatibility

The Codex _should_ work with any standard USB HID keyboard, both wired or via a wireless USB dongle. The Codex does **not** support bluetooth keyboards.

### Recommended

For a compact keyboard, a 65% or 75% layout is recommended for dedicated Home/End/Page Up/Page Down/Arrow keys without needing Fn combinations. Otherwise, pretty much whatever you're most comfortable with. I use an Epomaker TH80SE, which is a 75% keyboard. However, that doesn't have a dedicated "End" key (it's FN+PgDown). Something like the [Epomaker x Aula F65](https://epomaker.com/products/epomaker-aula-f65) or [Epomaker x Aula F75](https://epomaker.com/products/epomaker-aula-f75) could maybe be good choices that include all the buttons.

---

## Keyboard Shortcuts (WIP)

| Shortcut | Primary Action | Secondary Action
|---|---|---|
| Arrow keys | Move cursor (character / line) | |
| Ctrl + ←/→ | Move by word | |
| Ctrl + ↑/↓ | Jump to start/end of frame | Shift view by one full frame if at frame boundaries |
| Page Up/Page Down | Shift view by one full frame | |
| Home / End | Start/end of current line | |
| Ctrl + Home/End | Jump to start/end of current frame | |
| Ctrl + Shift + Home/End | Jump to start/end of document | |
| Tab | Insert two spaces | |
| Ctrl + S | Save current document to SD card | |
| Ctrl + Shift + I | Open document info (filename, size, etc) | |

---

## License

The Codex uses a split-license approach:

- All non-third-party code: **GPL v3**
- Hardware (`pcb/`) and Enclosures (`3dmodels/`): **CERN-OHL-S v2**

All third-party libraries retain their original licensing:

- TinyUSB: **MIT**
- no-OS-FatFS-SD-SPI-RPi-Pico **Apache 2.0**

See the `LICENSE` file in each directory for full text.

---

## Acknowledgments

- [LiquidCrystal](https://github.com/arduino-libraries/LiquidCrystal) original LCD library for Arduino, modified by jurs to support 40x4 displays
- [LiquidCrystal440](https://github.com/jurs/liquidcrystal440) modification of the original LiquidCrystal library by jurs, and that this project uses with small modifications.
- [no-OS-FatFS-SD-SPI-RPi-Pico](https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico)
  by carlk3
- [TinyUSB](https://github.com/hathach/tinyusb) USB HID stack
- The AlphaSmart Neo, for the original inspiration (sort of)
- The [r/writerdeck](https://reddit.com/r/writerdeck) community

---

## Contributing

The Codex is a work in progress and a personal project, but issues, ideas, and pull requests are welcome. If you build one, I'd love to hear about it.

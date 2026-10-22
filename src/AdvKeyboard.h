/*
 * Polling TCA8418 keyboard reader for the Cardputer ADV.
 *
 * M5Cardputer 1.1.1 drives the expander from a pin interrupt and clears its
 * software flag after reading the status register. A key event that arrives
 * in that small window is acknowledged without ever being read; the INT line
 * then stays low and no further edge is generated, so every later keypress is
 * lost and the UI looks frozen. Polling the event FIFO removes that race.
 */
#pragma once
#include <algorithm>
#include <memory>
#include <utility/Keyboard/KeyboardReader/KeyboardReader.h>
#include <utility/Adafruit_TCA8418/Adafruit_TCA8418.h>

class AdvKeyboardReader : public KeyboardReader {
public:
  void begin() override {
    _tca8418 = new Adafruit_TCA8418();
    if (!_tca8418->begin()) {
      _ok = false;
      return;
    }
    _tca8418->matrix(7, 8);
    _tca8418->flush();
    _ok = true;
  }

  void update() override {
    if (!_ok) {
      return;
    }

    // Drain everything the expander has buffered. Only one event is read per
    // call by the stock reader, which is what lets the FIFO fall behind.
    for (uint8_t count = _tca8418->available(); count > 0; count--) {
      uint8_t event = _tca8418->getEvent();
      if (event == 0) {
        break;
      }
      addOrRemove(event);
    }
  }

private:
  struct KeyEvent {
    bool state = false;
    uint8_t row = 0;
    uint8_t col = 0;
  };

  static const uint8_t kRows = 7;
  static const uint8_t kCols = 8;

  // Raw event code is row * 10 + col + 1 (the expander always encodes 10
  // columns); remap it onto the 4 x 14 logical layout of the Cardputer.
  KeyEvent decode(uint8_t eventRaw) {
    KeyEvent key;
    uint8_t code = eventRaw & 0x7F;
    key.state = eventRaw & 0x80;

    if (code == 0) {
      return key;
    }
    code--;
    uint8_t rawRow = code / 10;
    uint8_t rawCol = code % 10;
    if (rawRow >= kRows || rawCol >= kCols) {
      key.col = 0xFF;  // out of range, ignored by addOrRemove()
      return key;
    }

    key.col = rawRow * 2;
    if (rawCol > 3) {
      key.col++;
    }
    key.row = (rawCol + 4) % 4;
    return key;
  }

  void addOrRemove(uint8_t eventRaw) {
    KeyEvent key = decode(eventRaw);
    if (key.col > 13) {
      return;
    }

    Point2D_t point;
    point.x = key.col;
    point.y = key.row;

    auto it = std::find(_key_list.begin(), _key_list.end(), point);
    if (key.state) {
      if (it == _key_list.end()) {
        _key_list.push_back(point);
      }
    } else if (it != _key_list.end()) {
      _key_list.erase(it);
    }
  }

  Adafruit_TCA8418* _tca8418 = nullptr;
  bool _ok = false;
};

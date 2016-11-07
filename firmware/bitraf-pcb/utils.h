#pragma once

template<int interval_ms>
class fixed_interval_timer {
  public:
    void reset() {
      next = millis() + interval_ms;
    }

    bool expired() {
      auto now = millis();
      bool e = now > next;

      if (!e) {
        return false;
      }

      do {
        next += interval_ms;
      } while (now > next);

      return true;
    }

  private:
    long next;
};


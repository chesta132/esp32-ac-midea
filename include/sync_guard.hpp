#pragma once

#include <Arduino.h>

// Tracks a checklist of pins expected during a sync, so IR commands
// can be suppressed until every expected pin has reported in (or timed out).
class SyncGuard {
 public:
  // Register a pin (e.g. 0 for V0, 1 for V1, ...) as "expected" for the
  // next sync cycle. Call once per pin when a sync starts.
  void begin(uint8_t pin_count) {
    pending_mask_ = (pin_count >= 32) ? 0xFFFFFFFF : ((1UL << pin_count) - 1);
    active_ = pending_mask_ != 0;
    start_time_ms_ = millis();
  }

  // Call from each BLYNK_WRITE(Vx) handler with x as the index,
  // regardless of whether it happened during a sync or not.
  // Ticking a pin that isn't pending, or ticking with sync inactive,
  // is a harmless no-op.
  void tick(uint8_t pin) {
    if (!active_) return;
    pending_mask_ &= ~(1UL << pin);
    if (pending_mask_ == 0) finishCallback();
  }

  // Call once per loop() to expire a sync that never finished
  // (e.g. a datastream missing the "sync with latest value" option).
  void checkTimeout(unsigned long timeout_ms) {
    if (active_ && (millis() - start_time_ms_ > timeout_ms)) {
      pending_mask_ = 0;
      finishCallback();
    }
  }

  // Set a callback to be called when the sync finishes (either normally or via
  // timeout).
  using Callback = std::function<void()>;
  void onFinished(std::function<void(Callback)> callback) {
    auto prev = callback_;
    callback_ = [=]() {
      if (callback) {
        callback(prev);
      }
    };
  }

  // True while any expected pin hasn't reported in yet.
  bool isSyncing() const { return active_; }

 private:
  uint32_t pending_mask_ = 0;
  bool active_ = false;
  unsigned long start_time_ms_ = 0;
  Callback callback_{nullptr};
  void finishCallback() {
    active_ = false;
    if (callback_) callback_();
  }
};
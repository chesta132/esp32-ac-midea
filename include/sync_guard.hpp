#pragma once

#include <Arduino.h>
#include <arduino-timer.h>

#include <functional>

// Tracks a checklist of pins expected during a sync, so IR commands
// can be suppressed until every expected pin has reported in (or timed out).
class SyncGuard {
 public:
  // Register a pin (e.g. 0 for V0, 1 for V1, ...) as "expected" for the
  // next sync cycle and arm a timeout of `timeout_ms`. Call once when a
  // sync starts (replaces the old begin(pin_count) + per-loop checkTimeout).
  void begin(uint8_t pin_count, unsigned long timeout_ms) {
    pending_mask_ = (pin_count >= 32) ? 0xFFFFFFFF : ((1UL << pin_count) - 1);
    active_ = pending_mask_ != 0;

    cancelTimeout();
    if (active_) {
      timeout_ms_ = timeout_ms;
      timeout_task_ = timer_.in(
          timeout_ms,
          [](void* arg) -> bool {
            static_cast<SyncGuard*>(arg)->onTimeout();
            return false;  // one shot
          },
          this);
    }
  }

  // Call from each BLYNK_WRITE(Vx) handler with x as the index,
  // regardless of whether it happened during a sync or not.
  // Ticking a pin that isn't pending, or ticking with sync inactive,
  // is a harmless no-op.
  void tick(uint8_t pin) {
    if (!active_) return;
    pending_mask_ &= ~(1UL << pin);
    if (pending_mask_ == 0) {
      cancelTimeout();
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

  void loop() { timer_.tick(); }

 private:
  void onTimeout() {
    pending_mask_ = 0;
    LOG_WARN("Sync timeout after {} ms.", timeout_ms_);
    timeout_task_ = nullptr;
    finishCallback();
  }

  void cancelTimeout() {
    if (timeout_task_) {
      timer_.cancel(timeout_task_);
      timeout_task_ = nullptr;
    }
  }

  void finishCallback() {
    active_ = false;
    if (callback_) callback_();
  }

  Timer<> timer_ = timer_create_default();
  Timer<>::Task timeout_task_ = nullptr;
  uint32_t pending_mask_ = 0;
  bool active_ = false;
  unsigned long timeout_ms_ = 0;
  Callback callback_{nullptr};
};
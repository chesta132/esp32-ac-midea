#pragma once

// time helper
// second to ms
constexpr unsigned long operator"" _s(unsigned long long sec) {
  return sec * 1000UL;
}
// minute to ms
constexpr unsigned long operator"" _m(unsigned long long min) {
  return min * 60UL * 1000UL;
}
// hour to ms
constexpr unsigned long operator"" _h(unsigned long long hour) {
  return hour * 60UL * 60UL * 1000UL;
}
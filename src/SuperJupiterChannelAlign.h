// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_CHANNEL_ALIGN_H
#define SUPERJUPITER_CHANNEL_ALIGN_H
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_CHANNEL_ALIGN_VERSION "0.1.0"
namespace superjupiter {
namespace channel_align {

namespace detail {
inline bool finite(double value) { return value == value && value >= -DBL_MAX && value <= DBL_MAX; }
inline double oneMinusExpNegative(double ratio) {
  // Stable at small elapsed/tau without requiring AVR's unavailable expm1.
  if (ratio < 0.01) {
    double term = ratio, total = ratio;
    for (unsigned n = 2; n <= 12; ++n) {
      term *= -ratio / n;
      total += term;
    }
    return total;
  }
  return 1 - ::exp(-ratio);
}
} // namespace detail

enum class Status {
  Ok,
  Interpolated,
  Held,
  Missing,
  InvalidChannel,
  InvalidInput,
  InvalidTime,
  TooWide,
  Stale,
  NumericError
};
struct Result {
  Status status;
  double value;
  uint32_t ageMs;
};
template <size_t Channels> class ChannelAlign {
  static_assert(Channels > 0 && Channels <= 256, "ChannelAlign requires 1..256 channels");
  struct Pair {
    double older, newer;
    uint32_t oldMs, newMs;
    uint8_t count;
    Pair() : older(0), newer(0), oldMs(0), newMs(0), count(0) {}
  };
  Pair channels_[Channels];
  uint32_t interpolation_, hold_;

public:
  ChannelAlign(uint32_t maxInterpolationMs, uint32_t maxHoldMs = 0)
      : interpolation_(maxInterpolationMs), hold_(maxHoldMs) {}
  Status push(size_t channel, double value, uint32_t atMs) {
    if (channel >= Channels)
      return Status::InvalidChannel;
    if (!detail::finite(value))
      return Status::InvalidInput;
    Pair &p = channels_[channel];
    const uint32_t dt = atMs - p.newMs;
    if (p.count && (!dt || dt >= 0x80000000UL))
      return Status::InvalidTime;
    p.older = p.newer;
    p.oldMs = p.newMs;
    p.newer = value;
    p.newMs = atMs;
    if (p.count < 2)
      ++p.count;
    return Status::Ok;
  }
  Result at(size_t channel, uint32_t targetMs) const {
    if (channel >= Channels)
      return Result{Status::InvalidChannel, 0, 0};
    const Pair &p = channels_[channel];
    if (!p.count)
      return Result{Status::Missing, 0, 0};
    if (targetMs == p.newMs)
      return Result{Status::Ok, p.newer, 0};
    if (p.count == 2 && targetMs == p.oldMs)
      return Result{Status::Ok, p.older, 0};
    const uint32_t age = targetMs - p.newMs;
    if (age < 0x80000000UL) {
      if (age <= hold_ && hold_ < 0x80000000UL)
        return Result{Status::Held, p.newer, age};
      return Result{Status::Stale, 0, age};
    }
    if (p.count < 2)
      return Result{Status::Missing, 0, 0};
    const uint32_t span = p.newMs - p.oldMs, dt = targetMs - p.oldMs;
    if (dt >= span)
      return Result{Status::Missing, 0, 0};
    if (span > interpolation_ || interpolation_ >= 0x80000000UL)
      return Result{Status::TooWide, 0, 0};
    const double fraction = static_cast<double>(dt) / span;
    const double value = (1 - fraction) * p.older + fraction * p.newer;
    return Result{detail::finite(value) ? Status::Interpolated : Status::NumericError,
                  detail::finite(value) ? value : 0, 0};
  }
};
} // namespace channel_align
} // namespace superjupiter
#endif

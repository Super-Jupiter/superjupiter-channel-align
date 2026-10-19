# SuperJupiter ChannelAlign

Explicit bounded interpolation and stale-value handling across sensor channels.

**Version 0.1.1: evolving preview. Hardware validation pending.**

Original MIT-licensed portable C++11 by [SuperJupiter](https://www.superjupiter.net).
No mandatory service, hidden clock reads, network calls or heap allocation in the core.

## What it does

Retains two finite samples per channel; channels must use the same uint32 millisecond clock.
Each channel advances by 1..2^31-1 ms. Query must remain within that channel's half-range clock horizon.
Exact samples succeed. Between retained samples interpolation is allowed only below maxInterpolationMs.
After the newest sample, optional hold-last succeeds up to maxHoldMs and reports Held. No extrapolation before history.
Different physical units are not combined. No implicit alignment or claim that interpolation reconstructs true readings.

## Install

PlatformIO exact release (after registry publication):

```ini
lib_deps = superjupiter/SuperJupiterChannelAlign@0.1.1
```

Arduino: use the versioned [GitHub release ZIP](https://github.com/Super-Jupiter/superjupiter-channel-align/releases/tag/v0.1.1).
Library Manager registration and index propagation are separate steps; check the official index before relying on IDE availability.

## Example

```cpp
#include <SuperJupiterChannelAlign.h>
using namespace superjupiter::channel_align;
int main() {
ChannelAlign<2> channels(1000,100);
channels.push(0,20,0);channels.push(0,22,1000);
Result aligned=channels.at(0,500);
(void)aligned;
return 0;
}

```

For Arduino, open `examples/Minimal`. It uses synthetic data only.
Results are deliberately local so applications can inspect outcomes before adding I/O.

## CMake

```cmake
add_subdirectory(path/to/superjupiter-channel-align)
target_link_libraries(your_app PRIVATE SuperJupiter::ChannelAlign)
```

For an installed package use `find_package(SuperJupiterChannelAlign 0.1 CONFIG REQUIRED)`.
Top-level CMake builds tests/examples; a subproject does not enable developer tools by default.

## Documentation and verification

- [API and boundaries](https://github.com/Super-Jupiter/superjupiter-channel-align/blob/v0.1.1/docs/API.md)
- [Tests](https://github.com/Super-Jupiter/superjupiter-channel-align/blob/v0.1.1/tests/test.cpp)
- [Validation status](https://github.com/Super-Jupiter/superjupiter-channel-align/blob/v0.1.1/docs/VALIDATION.md)
- [Shared conventions](https://github.com/Super-Jupiter/superjupiter-channel-align/blob/v0.1.1/docs/CONVENTIONS.md)
- [Migration policy](https://github.com/Super-Jupiter/superjupiter-channel-align/blob/v0.1.1/docs/MIGRATION.md)

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
python3 tools/check_release.py --tag v0.1.1
```

Keep raw observations and their provenance in your application. Derived results are not physical truth.
Do not use fast-math or finite-math-only compiler assumptions. AVR double may have float precision.
Software tests, board compilation and physical validation are reported separately.

## Fits the SuperJupiter measurement portfolio

Complements [SensorQuality](https://github.com/Super-Jupiter/superjupiter-sensor-quality) and
[SampleWindow](https://github.com/Super-Jupiter/superjupiter-sample-window) without a required dependency.
Each component can be used independently. Domain models, customer data and proprietary applications are excluded.

## Support and licence

MIT. Report reproducible bugs through repository issues; private security reports: info@superjupiter.net.
Include version, configuration, inputs, expected and observed behaviour. No credentials or private customer data.


## Compatibility review 0.1.1

This patch includes independent regression tests and a reviewed [interoperability contract](https://github.com/Super-Jupiter/superjupiter-channel-align/blob/v0.1.1/docs/INTEROPERABILITY.md). Hardware validation pending.

- Removed unused duplicated math helpers and unnecessary includes.
- Invalid time limits consistently rejected; interpolation uses stable convex blending.

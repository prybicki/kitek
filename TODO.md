Release target:
- PID has no still error
    - Chart of p, i, d, integral [in progress]
- Scanner in auto mode
    - Plot 2D
- Scanner in manual mode

- PoseEstimator [should be runnable both on base and device]
    - relativization of scanner data

- Save overall mileage
    - Show it in GUI

- Timeouts (speed!)

Pool:
- liftTick does not look like a safe thing, use amplified clock everywhere?
- Use event queue instead of polling
    - Remove tickable, useless
- Templatization
- It seems that code should be moved to header and templatized:
    - Long compilation time is not an issue, it's a helper to write mindfully.
- NVController should take Joy as parameter - but what about cases when no joy is used?
    - Ah, just optional and if.
- Link pigpio statically and do LTO, low priority

- realtime manager
- display overall mileage

Ports:
1024 - kitek.cpp -> 2048 (base.cpp) 2049 -> 4096 (gui.py)

Research:
- Implement led screen on stm32, figure out if it makes sense to have raspi/stm32 HAL (device templates)
    - Remove direct calls to gpioTick
- Implement std::chrono::clock using gpioTick (?)

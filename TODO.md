For now, PID is reasonably good within 0.3 m/s speed limit.
In future try to understand the way the integral behaves.
Additionally, figure out a way to treat both wheels as single system, not separate control loops.
Integral can be increased by turning the wheel manually or driving in half stall


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

- Total uptime in statistics
- Timeouts (speed!)

Pool:
- Differential voltage measurement (5V vs 8.4 battery -> 0, 3.4V range)
    - It can help with diagnosing bad 5V voltage (but what if there's sudden diff overvoltage?)
- Feature to stop the world (charts)
    - + Recording in the base?
- Fix the nasty joy bug (sticking to -1)
- liftTick does not look like a safe thing, use amplified clock everywhere?
- Use event queue instead of polling
    - Remove tickable, useless
    - Implement periodic behavior, like printing debugs
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

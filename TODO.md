Functional targets:

- PID has no still error
    - Chart of p, i, d?

- PoseEstimator
    - save overall mileage

- Timeouts (speed!)


Technical targets:
- Does this Tickable thing makes any sense?
- PID can be templatized with min/max -> float can't be simply template parameter
- NVController should take Joy as parameter

Optional:

- realtime manager
- display overall mileage

- Implement std::chrono::clock using gpioTick

Ports:
1024 - kitek.cpp -> 2048 (base.cpp) 2049 -> 4096 (gui.py)
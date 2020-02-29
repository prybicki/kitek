#include <Joy.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>
#include <fmt/format.h>

Joy::Joy(fs::path devPath)
{
    int status = 0;
    fd = open(devPath.c_str(), O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        throw std::system_error(errno, std::system_category(), "Joy/open");
    }

    status = libevdev_new_from_fd(fd, &dev);
    if (status < 0) {
        close(fd);
        throw std::system_error(-status, std::system_category(), "Joy/libevdev_new_from_fd");
    }

    bool isJoy = libevdev_has_event_type(dev, EV_ABS) && libevdev_has_event_type(dev, EV_KEY);
    if(!isJoy) {
        close(fd);
        throw std::runtime_error(fmt::format("Joy/device is not a joystick: {}", devPath.generic_string()));
    }

    for (int code = 0; code < KEY_CNT; ++code) {
        if (libevdev_has_event_code(dev, EV_KEY, code)) {
            state.buttons[code] = false;
        }
    }

    for(int code = 0; code < ABS_CNT; ++code) {
        if (libevdev_has_event_code(dev, EV_ABS, code)) {
            auto* info = libevdev_get_abs_info(dev, code);
            // fmt::print("code={} min={} max={} res={} val={} flat={} fuzz={}\n", axisName(code), info->minimum, info->maximum, info->resolution, info->value, info->flat, info->fuzz);
            axisInfo[code] = info;
            int value = info->value;
            if (value > info->maximum) {
                // fmt::print("{} overflow = {} / {}!\n", axisName(code), value, info->maximum);
                value = info->maximum;
            }
            if (value < info->minimum) {
                // fmt::print("{} underflow = {} / {}!\n", axisName(code), value, info->minimum);
                value = info->minimum;
            }
            axisZero[code] = info->value;
            state.axes[code] = axisValueToFloat(code, info->value); // best guess, may be not between <min, max> though
            // fmt::print("{} = {} [{}] ({} {})\n", axisName(code), state.axes[code], info->value, info->minimum, info->maximum);
        }
    }

}

const char *Joy::btnName(int btn)
{
    const char* name = libevdev_event_code_get_name(EV_KEY, btn);
    return (name == nullptr ? "(null button)" : name);
}

const char *Joy::axisName(int axis)
{
    const char* name = libevdev_event_code_get_name(EV_ABS, axis);
    return (name == nullptr ? "(null axis)" : name);
}

const JoyState &Joy::update()
{
    int status = 0;

    // reset sticky buttons
    while (!stickyButtons.empty()) {
        int code = stickyButtons.front();
        state.buttons[code] = false;
        stickyButtons.pop();
    }

    struct input_event ev;
    std::set<int> buttonsPressed;
    while (true) {
        status = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        // handle return condition
        if (status == -EAGAIN) {
            break;
        }
        if (status < 0) {
            throw std::runtime_error(fmt::format("Joy/libevdev_next_event: {}", status));

        }

        // handle events
        if (ev.type == EV_ABS) {
            state.axes[ev.code] = axisValueToFloat(ev.code, ev.value);
        }
        if (ev.type == EV_KEY) {
            if (ev.value == 1) {
                buttonsPressed.insert(ev.code);
                state.buttons[ev.code] = ev.value;
            }
            else {
                bool seenPress = (buttonsPressed.find(ev.code) != buttonsPressed.end());
                if (seenPress) {
                    stickyButtons.push(ev.code);
                }
                else {
                    state.buttons[ev.code] = ev.value;
                }
            }
        }
    }
    return state;
}

float Joy::axisValueToFloat(int axis, int value)
{
    static auto mapRange = [](float s, float a1, float a2, float b1, float b2) {
        return b1 + ((s - a1) * (b2 - b1) / (a2 - a1));
    };

    auto* info = axisInfo[axis];
    if (value > axisZero[axis]) {
        return mapRange(
                static_cast<float>(value),
                static_cast<float>(axisZero[axis]), static_cast<float>(info->maximum),
                0.0f, 1.0f
        );
    }
    else if(value < axisZero[axis]) {
        return mapRange(
                static_cast<float>(value),
                static_cast<float>(info->minimum), static_cast<float>(axisZero[axis]),
                -1.0f, 0.0f
        );
    }
    else {
        return 0.0f;
    }
}

std::set<fs::path> Joy::searchDevice(const std::string &nameSubstring, bool verbose)
{
    std::set<fs::path> results;
    for(auto&& entry : fs::directory_iterator("/dev/input")) {
        if (fs::status(entry.path()).type() != fs::file_type::character) {
            continue;
        }
        int fd = open(entry.path().c_str(), O_RDONLY|O_NONBLOCK);
        if (fd < 0) {
            if (verbose) {
                fmt::print("{} => !! [{}]\n", entry.path().c_str(), strerror(errno));
            }
            continue;
        }
        struct libevdev * dev = NULL;
        int status = libevdev_new_from_fd(fd, &dev);
        if (status < 0) {
            if (verbose) {
                fmt::print("{} => !! [{}]\n", entry.path().c_str(), strerror(-status));
            }
            continue;
        }
        std::string devName(libevdev_get_name(dev));
        libevdev_free(dev);
        close(fd);

        if (verbose) {
            fmt::print("{} => {}\n", entry.path().c_str(), devName);
        }

        auto pos = devName.find(nameSubstring);
        if (pos != std::string::npos) {
            results.insert(entry.path());
        }
    }
    return results;
}

Joy::~Joy()
{
    if (dev != nullptr) {
        libevdev_free(dev);
    }
    if (fd != 0) {
        close(fd);
    }
}

#include "ginput/profile.hpp"

#include <algorithm>
#include <cmath>

namespace ginput {
namespace {

bool same_float(float a, float b) {
    return std::fabs(a - b) <= 0.000001f;
}

} // namespace

bool same_bind(const ButtonBind& a, const ButtonBind& b) {
    return a.device_button == b.device_button && a.action == b.action;
}

bool same_bind(const Axis1DBind& a, const Axis1DBind& b) {
    return a.device_axis == b.device_axis && a.axis_1d == b.axis_1d &&
           same_float(a.scale, b.scale) && same_float(a.deadzone, b.deadzone);
}

bool same_bind(const Axis2DBind& a, const Axis2DBind& b) {
    return a.device_stick == b.device_stick && a.axis_2d == b.axis_2d &&
           same_float(a.scale_x, b.scale_x) && same_float(a.scale_y, b.scale_y) &&
           same_float(a.deadzone, b.deadzone);
}

bool add_button_bind(InputProfile& profile, ButtonBind bind) {
    for (const ButtonBind& existing : profile.button_binds) {
        if (same_bind(existing, bind)) {
            return false;
        }
    }
    profile.button_binds.push_back(bind);
    return true;
}

bool add_axis_1d_bind(InputProfile& profile, Axis1DBind bind) {
    for (const Axis1DBind& existing : profile.axis_1d_binds) {
        if (same_bind(existing, bind)) {
            return false;
        }
    }
    profile.axis_1d_binds.push_back(bind);
    return true;
}

bool add_axis_2d_bind(InputProfile& profile, Axis2DBind bind) {
    for (const Axis2DBind& existing : profile.axis_2d_binds) {
        if (same_bind(existing, bind)) {
            return false;
        }
    }
    profile.axis_2d_binds.push_back(bind);
    return true;
}

bool remove_button_bind(InputProfile& profile, ButtonBind bind) {
    auto it = std::find_if(profile.button_binds.begin(),
                           profile.button_binds.end(),
                           [&](const ButtonBind& existing) { return same_bind(existing, bind); });
    if (it == profile.button_binds.end()) {
        return false;
    }
    profile.button_binds.erase(it);
    return true;
}

bool remove_axis_1d_bind(InputProfile& profile, Axis1DBind bind) {
    auto it = std::find_if(profile.axis_1d_binds.begin(),
                           profile.axis_1d_binds.end(),
                           [&](const Axis1DBind& existing) { return same_bind(existing, bind); });
    if (it == profile.axis_1d_binds.end()) {
        return false;
    }
    profile.axis_1d_binds.erase(it);
    return true;
}

bool remove_axis_2d_bind(InputProfile& profile, Axis2DBind bind) {
    auto it = std::find_if(profile.axis_2d_binds.begin(),
                           profile.axis_2d_binds.end(),
                           [&](const Axis2DBind& existing) { return same_bind(existing, bind); });
    if (it == profile.axis_2d_binds.end()) {
        return false;
    }
    profile.axis_2d_binds.erase(it);
    return true;
}

float apply_axis_transform(float value, float scale, float deadzone) {
    const float scaled = value * scale;
    const float magnitude = std::fabs(scaled);
    if (magnitude <= std::max(deadzone, 0.0f)) {
        return 0.0f;
    }
    return std::clamp(scaled, -1.0f, 1.0f);
}

Vec2 apply_stick_transform(Vec2 value, float scale_x, float scale_y, float deadzone) {
    Vec2 scaled{value.x * scale_x, value.y * scale_y};
    const float magnitude = std::sqrt((scaled.x * scaled.x) + (scaled.y * scaled.y));
    if (magnitude <= std::max(deadzone, 0.0f)) {
        return Vec2{};
    }
    scaled.x = std::clamp(scaled.x, -1.0f, 1.0f);
    scaled.y = std::clamp(scaled.y, -1.0f, 1.0f);
    return scaled;
}

} // namespace ginput

#include "ginput/profile.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace ginput {
namespace {

bool same_float(float a, float b) {
    return std::fabs(a - b) <= 0.000001f;
}

template <typename T> const std::vector<T>& empty_vector() {
    static const std::vector<T> empty;
    return empty;
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
    profile.lookup.button_actions[bind.device_button].push_back(bind.action);
    return true;
}

bool add_axis_1d_bind(InputProfile& profile, Axis1DBind bind) {
    for (const Axis1DBind& existing : profile.axis_1d_binds) {
        if (same_bind(existing, bind)) {
            return false;
        }
    }
    profile.axis_1d_binds.push_back(bind);
    profile.lookup.axes_1d[bind.device_axis].push_back(bind);
    return true;
}

bool add_axis_2d_bind(InputProfile& profile, Axis2DBind bind) {
    for (const Axis2DBind& existing : profile.axis_2d_binds) {
        if (same_bind(existing, bind)) {
            return false;
        }
    }
    profile.axis_2d_binds.push_back(bind);
    profile.lookup.axes_2d[bind.device_stick].push_back(bind);
    return true;
}

bool remove_button_bind(InputProfile& profile, ButtonBind bind) {
    auto it = std::find_if(profile.button_binds.begin(), profile.button_binds.end(),
                           [&](const ButtonBind& existing) { return same_bind(existing, bind); });
    if (it == profile.button_binds.end()) {
        return false;
    }
    profile.button_binds.erase(it);
    rebuild_lookup(profile);
    return true;
}

bool remove_axis_1d_bind(InputProfile& profile, Axis1DBind bind) {
    auto it = std::find_if(profile.axis_1d_binds.begin(), profile.axis_1d_binds.end(),
                           [&](const Axis1DBind& existing) { return same_bind(existing, bind); });
    if (it == profile.axis_1d_binds.end()) {
        return false;
    }
    profile.axis_1d_binds.erase(it);
    rebuild_lookup(profile);
    return true;
}

bool remove_axis_2d_bind(InputProfile& profile, Axis2DBind bind) {
    auto it = std::find_if(profile.axis_2d_binds.begin(), profile.axis_2d_binds.end(),
                           [&](const Axis2DBind& existing) { return same_bind(existing, bind); });
    if (it == profile.axis_2d_binds.end()) {
        return false;
    }
    profile.axis_2d_binds.erase(it);
    rebuild_lookup(profile);
    return true;
}

InputProfile* find_profile(std::vector<InputProfile>& profiles, int id) {
    for (InputProfile& profile : profiles) {
        if (profile.id == id) {
            return &profile;
        }
    }
    return nullptr;
}

const InputProfile* find_profile(const std::vector<InputProfile>& profiles, int id) {
    for (const InputProfile& profile : profiles) {
        if (profile.id == id) {
            return &profile;
        }
    }
    return nullptr;
}

InputProfile* find_profile_by_name(std::vector<InputProfile>& profiles, const std::string& name) {
    for (InputProfile& profile : profiles) {
        if (profile.name == name) {
            return &profile;
        }
    }
    return nullptr;
}

const InputProfile* find_profile_by_name(const std::vector<InputProfile>& profiles,
                                         const std::string& name) {
    for (const InputProfile& profile : profiles) {
        if (profile.name == name) {
            return &profile;
        }
    }
    return nullptr;
}

bool add_profile(std::vector<InputProfile>& profiles, InputProfile profile) {
    if (profile.id <= 0 || profile.name.empty()) {
        return false;
    }
    if (find_profile(profiles, profile.id) != nullptr) {
        return false;
    }
    if (find_profile_by_name(profiles, profile.name) != nullptr) {
        return false;
    }
    rebuild_lookup(profile);
    profiles.push_back(std::move(profile));
    return true;
}

bool replace_profile(std::vector<InputProfile>& profiles, InputProfile profile) {
    if (profile.id <= 0 || profile.name.empty()) {
        return false;
    }
    for (const InputProfile& existing : profiles) {
        if (existing.id != profile.id && existing.name == profile.name) {
            return false;
        }
    }
    for (InputProfile& existing : profiles) {
        if (existing.id == profile.id) {
            rebuild_lookup(profile);
            existing = std::move(profile);
            return true;
        }
    }
    rebuild_lookup(profile);
    profiles.push_back(std::move(profile));
    return true;
}

void rebuild_lookup(InputProfile& profile) {
    profile.lookup.button_actions.clear();
    profile.lookup.axes_1d.clear();
    profile.lookup.axes_2d.clear();

    for (const ButtonBind& bind : profile.button_binds) {
        profile.lookup.button_actions[bind.device_button].push_back(bind.action);
    }
    for (const Axis1DBind& bind : profile.axis_1d_binds) {
        profile.lookup.axes_1d[bind.device_axis].push_back(bind);
    }
    for (const Axis2DBind& bind : profile.axis_2d_binds) {
        profile.lookup.axes_2d[bind.device_stick].push_back(bind);
    }
}

const std::vector<ActionId>& actions_for_button(const InputProfile& profile,
                                                EncodedControl device_button) {
    auto it = profile.lookup.button_actions.find(device_button);
    if (it == profile.lookup.button_actions.end()) {
        return empty_vector<ActionId>();
    }
    return it->second;
}

const std::vector<Axis1DBind>& axes_for_1d(const InputProfile& profile,
                                           EncodedControl device_axis) {
    auto it = profile.lookup.axes_1d.find(device_axis);
    if (it == profile.lookup.axes_1d.end()) {
        return empty_vector<Axis1DBind>();
    }
    return it->second;
}

const std::vector<Axis2DBind>& axes_for_2d(const InputProfile& profile,
                                           EncodedControl device_stick) {
    auto it = profile.lookup.axes_2d.find(device_stick);
    if (it == profile.lookup.axes_2d.end()) {
        return empty_vector<Axis2DBind>();
    }
    return it->second;
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

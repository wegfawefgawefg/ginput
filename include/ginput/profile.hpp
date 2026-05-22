#pragma once

#include "ginput/types.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace ginput {

struct ButtonBind {
    EncodedControl device_button = 0;
    ActionId action = -1;
};

struct Axis1DBind {
    EncodedControl device_axis = 0;
    Axis1DId axis_1d = -1;
    float scale = 1.0f;
    float deadzone = 0.0f;
};

struct Axis2DBind {
    EncodedControl device_stick = 0;
    Axis2DId axis_2d = -1;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float deadzone = 0.0f;
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct ProfileLookup {
    std::unordered_map<EncodedControl, std::vector<ActionId>> button_actions;
    std::unordered_map<EncodedControl, std::vector<Axis1DBind>> axes_1d;
    std::unordered_map<EncodedControl, std::vector<Axis2DBind>> axes_2d;
};

struct InputProfile {
    int id = -1;
    std::string name;
    std::vector<ButtonBind> button_binds;
    std::vector<Axis1DBind> axis_1d_binds;
    std::vector<Axis2DBind> axis_2d_binds;
    ProfileLookup lookup;
};

bool same_bind(const ButtonBind& a, const ButtonBind& b);
bool same_bind(const Axis1DBind& a, const Axis1DBind& b);
bool same_bind(const Axis2DBind& a, const Axis2DBind& b);

bool add_button_bind(InputProfile& profile, ButtonBind bind);
bool add_axis_1d_bind(InputProfile& profile, Axis1DBind bind);
bool add_axis_2d_bind(InputProfile& profile, Axis2DBind bind);

bool remove_button_bind(InputProfile& profile, ButtonBind bind);
bool remove_axis_1d_bind(InputProfile& profile, Axis1DBind bind);
bool remove_axis_2d_bind(InputProfile& profile, Axis2DBind bind);

InputProfile* find_profile(std::vector<InputProfile>& profiles, int id);
const InputProfile* find_profile(const std::vector<InputProfile>& profiles, int id);
InputProfile* find_profile_by_name(std::vector<InputProfile>& profiles, const std::string& name);
const InputProfile* find_profile_by_name(const std::vector<InputProfile>& profiles,
                                         const std::string& name);

bool add_profile(std::vector<InputProfile>& profiles, InputProfile profile);
bool replace_profile(std::vector<InputProfile>& profiles, InputProfile profile);

void rebuild_lookup(InputProfile& profile);
const std::vector<ActionId>& actions_for_button(const InputProfile& profile,
                                                EncodedControl device_button);
const std::vector<Axis1DBind>& axes_for_1d(const InputProfile& profile, EncodedControl device_axis);
const std::vector<Axis2DBind>& axes_for_2d(const InputProfile& profile,
                                           EncodedControl device_stick);

float apply_axis_transform(float value, float scale, float deadzone);
Vec2 apply_stick_transform(Vec2 value, float scale_x, float scale_y, float deadzone);

} // namespace ginput

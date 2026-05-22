#pragma once

#include "ginput/types.hpp"

#include <string>
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

struct InputProfile {
    int id = -1;
    std::string name;
    std::vector<ButtonBind> button_binds;
    std::vector<Axis1DBind> axis_1d_binds;
    std::vector<Axis2DBind> axis_2d_binds;
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

float apply_axis_transform(float value, float scale, float deadzone);
Vec2 apply_stick_transform(Vec2 value, float scale_x, float scale_y, float deadzone);

} // namespace ginput

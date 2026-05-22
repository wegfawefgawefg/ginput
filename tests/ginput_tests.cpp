#include "ginput/ginput.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << "\n";
        std::exit(1);
    }
}

void require_near(float actual, float expected, const char* message) {
    if (std::fabs(actual - expected) > 0.0001f) {
        std::cerr << "FAILED: " << message << " actual=" << actual << " expected=" << expected
                  << "\n";
        std::exit(1);
    }
}

void test_encoded_controls() {
    ginput::DeviceButton button{};
    button.kind = ginput::DeviceKind::Gamepad;
    button.device_id = 2;
    button.code = 12;

    const int encoded = ginput::encode_button(button);
    ginput::DeviceButton decoded{};
    require(ginput::decode_button(encoded, decoded), "decode extended button");
    require(decoded.kind == ginput::DeviceKind::Gamepad, "button kind round trip");
    require(decoded.device_id == 2, "button device round trip");
    require(decoded.code == 12, "button code round trip");

    ginput::DeviceAxis2D stick{};
    stick.kind = ginput::DeviceKind::Gamepad;
    stick.device_id = ginput::any_device_id;
    stick.x_code = 4;
    stick.y_code = 5;

    const int stick_encoded = ginput::encode_axis_2d(stick);
    ginput::DeviceAxis2D stick_decoded{};
    require(ginput::decode_axis_2d(stick_encoded, stick_decoded), "decode extended stick");
    require(stick_decoded.device_id == ginput::any_device_id, "any-device round trip");
    require(stick_decoded.x_code == 4, "stick x round trip");
    require(stick_decoded.y_code == 5, "stick y round trip");
}

void test_schema() {
    ginput::Schema schema;
    require(schema.add_action(0, "Jump", "Gameplay"), "add action");
    require(!schema.add_action(0, "Jump Again", "Gameplay"), "reject duplicate action id");
    require(schema.add_axis_1d(0, "Throttle", "Gameplay"), "add 1d axis");
    require(schema.add_axis_2d(0, "Aim", "Gameplay"), "add 2d axis");
    require(schema.has_action(0), "has action");
    require(!schema.has_action(99), "missing action");
    require(schema.find_action(0)->label == "Jump", "find action label");
}

void test_profile_helpers() {
    ginput::InputProfile profile;
    profile.id = 7;
    profile.name = "Default";

    require(ginput::add_button_bind(profile, ginput::ButtonBind{100, 1}), "add button bind");
    require(!ginput::add_button_bind(profile, ginput::ButtonBind{100, 1}), "reject exact duplicate");
    require(ginput::add_button_bind(profile, ginput::ButtonBind{100, 2}),
            "allow same button to different action");
    require(profile.button_binds.size() == 2, "button bind count");
    require(ginput::remove_button_bind(profile, ginput::ButtonBind{100, 1}), "remove bind");
    require(profile.button_binds.size() == 1, "button bind remove count");

    std::vector<ginput::InputProfile> profiles;
    require(ginput::add_profile(profiles, profile), "add profile");
    require(!ginput::add_profile(profiles, profile), "reject duplicate profile id");
    require(ginput::find_profile(profiles, 7) != nullptr, "find profile by id");
    require(ginput::find_profile_by_name(profiles, "Default") != nullptr, "find profile by name");

    ginput::InputProfile replacement;
    replacement.id = 7;
    replacement.name = "Updated";
    require(ginput::replace_profile(profiles, replacement), "replace profile");
    require(ginput::find_profile(profiles, 7)->name == "Updated", "replace profile name");
}

void test_transforms() {
    require_near(ginput::apply_axis_transform(0.04f, 1.0f, 0.05f), 0.0f, "axis deadzone");
    require_near(ginput::apply_axis_transform(0.5f, -1.0f, 0.05f), -0.5f, "axis scale");
    require_near(ginput::apply_axis_transform(2.0f, 1.0f, 0.0f), 1.0f, "axis clamp");

    const ginput::Vec2 stick =
        ginput::apply_stick_transform(ginput::Vec2{0.25f, 0.5f}, 1.0f, -1.0f, 0.1f);
    require_near(stick.x, 0.25f, "stick x scale");
    require_near(stick.y, -0.5f, "stick y scale");
}

void test_reconcile() {
    ginput::Schema schema;
    schema.add_action(1, "Use");
    schema.add_action(2, "Jump");
    schema.add_axis_1d(3, "Throttle");
    schema.add_axis_2d(4, "Aim");

    ginput::InputProfile profile;
    profile.id = 9;
    profile.button_binds.push_back(ginput::ButtonBind{100, 1});
    profile.button_binds.push_back(ginput::ButtonBind{100, 1});
    profile.button_binds.push_back(ginput::ButtonBind{101, 99});
    profile.button_binds.push_back(ginput::ButtonBind{100, 2});
    profile.axis_1d_binds.push_back(ginput::Axis1DBind{200, 3});
    profile.axis_1d_binds.push_back(ginput::Axis1DBind{201, 99});
    profile.axis_2d_binds.push_back(ginput::Axis2DBind{300, 4});
    profile.axis_2d_binds.push_back(ginput::Axis2DBind{300, 4});

    const ginput::ReconcileReport report = ginput::reconcile_profile(profile, schema);
    require(report.changed(), "reconcile changed");
    require(report.changes.size() == 4, "reconcile change count");
    require(profile.button_binds.size() == 2, "reconcile keeps valid distinct buttons");
    require(profile.axis_1d_binds.size() == 1, "reconcile removes invalid 1d axis");
    require(profile.axis_2d_binds.size() == 1, "reconcile removes duplicate 2d axis");
}

void test_profile_io() {
    ginput::InputProfile profile;
    profile.id = 12;
    profile.name = "Keyboard";
    profile.button_binds.push_back(ginput::ButtonBind{26, 0});
    profile.button_binds.push_back(ginput::ButtonBind{26, 1});
    profile.axis_1d_binds.push_back(ginput::Axis1DBind{1000, 2, -1.0f, 0.05f});
    profile.axis_2d_binds.push_back(ginput::Axis2DBind{2000, 3, 1.0f, -1.0f, 0.12f});

    std::string text;
    require(ginput::save_profiles_string({profile}, text), "save profiles string");
    require(text.find("(input_profiles") != std::string::npos, "save root");
    require(text.find("(scale -1") != std::string::npos, "save 1d scale");
    require(text.find("(scale_y -1") != std::string::npos, "save 2d scale");

    ginput::LoadProfilesResult loaded = ginput::load_profiles_string(text);
    require(loaded.ok, "load saved profiles");
    require(loaded.profiles.size() == 1, "load profile count");
    require(loaded.profiles[0].name == "Keyboard", "load profile name");
    require(loaded.profiles[0].button_binds.size() == 2, "load button bind count");
    require(loaded.profiles[0].axis_1d_binds.size() == 1, "load 1d bind count");
    require(loaded.profiles[0].axis_2d_binds.size() == 1, "load 2d bind count");
    require_near(loaded.profiles[0].axis_1d_binds[0].scale, -1.0f, "load 1d scale");
    require_near(loaded.profiles[0].axis_2d_binds[0].scale_y, -1.0f, "load 2d scale");

    ginput::Schema schema;
    schema.add_action(0, "Menu Up");
    schema.add_action(1, "Move Up");
    schema.add_axis_1d(2, "Throttle");
    schema.add_axis_2d(3, "Aim");
    ginput::LoadProfilesResult reconciled = ginput::load_profiles_string(text, schema);
    require(reconciled.ok, "load with schema");
    require(!reconciled.reconcile_report.changed(), "load with schema no changes");
}

} // namespace

int main() {
    require(ginput::version_major() == 0, "version placeholder");
    test_encoded_controls();
    test_schema();
    test_profile_helpers();
    test_transforms();
    test_reconcile();
    test_profile_io();
    std::cout << "ginput_tests passed\n";
    return 0;
}

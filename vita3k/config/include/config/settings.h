// Vita3K emulator project
// Copyright (C) 2026 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#pragma once

#include <config/state.h>

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace config {

enum class RestartRequiredSetting : uint8_t {
    CpuOpt = 0,
    BackendRenderer = 1,
    GraphicsDevice = 2,
    CustomDriver = 3,
    HighAccuracy = 4,
    ResolutionMultiplier = 5,
    MemoryMapping = 6,
    AudioBackend = 7,
    ValidationLayer = 8,
};

std::vector<RestartRequiredSetting> get_restart_required_settings(
    const Config::CurrentConfig &before,
    const Config::CurrentConfig &after);

bool load_custom_config(Config::CurrentConfig &out, const fs::path &config_path, const std::string &app_path);
// Keys are "<section>/<name>" as in the custom config file, e.g. "gpu/resolution-multiplier".
// Maps a key to whether a custom config overrides it; keys it does not name keep the default rule.
using CustomConfigOverrides = std::map<std::string, bool>;

bool save_custom_config(const Config::CurrentConfig &cc, const Config::CurrentConfig &global, const fs::path &config_path, const std::string &app_path,
    const CustomConfigOverrides *overrides = nullptr);
std::set<std::string> get_custom_config_keys(const fs::path &config_path, const std::string &app_path);
void copy_custom_config_keys(Config::CurrentConfig &dst, const Config::CurrentConfig &src, const std::vector<std::string> &keys);

// JSON shaped like a custom config: {"gpu": {"resolution-multiplier": 3}}. Unknown keys and values of the
// wrong type are skipped; false (with error set) only when the text does not parse or is not an object.
bool apply_config_override(Config::CurrentConfig &out, const std::string &json, std::string *error = nullptr);
std::string custom_config_to_json(const Config::CurrentConfig &cc, const std::vector<std::string> &keys);
bool delete_custom_config(const fs::path &config_path, const std::string &app_path);
int delete_all_custom_configs(const fs::path &config_path);
bool has_custom_config(const fs::path &config_path, const std::string &app_path);
// With use_config_override, the app launched with --config-override gets it instead of its custom config.
void set_current_config(Config &cfg, const fs::path &config_path, const std::string &app_path, bool use_config_override = false);
void copy_current_config_to_global(Config &cfg);
void save_current_config(Config &cfg, const fs::path &config_path, const std::string &app_path, bool create_custom_if_missing = false,
    const CustomConfigOverrides *overrides = nullptr);
std::vector<std::pair<std::string, bool>> get_modules_list(
    const fs::path &vita_fs_path,
    const std::vector<std::string> &lle_modules);

} // namespace config

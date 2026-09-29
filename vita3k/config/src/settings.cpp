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

#include <config/settings.h>

#include <config/functions.h>

#include <util/log.h>
#include <util/vector_utils.h>

#include <pugixml.hpp>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <cstdlib>
#include <string_view>
#include <type_traits>

namespace config {

namespace {

void copy_global_to_current(Config::CurrentConfig &current, const Config &cfg) {
    current.cpu_opt = cfg.cpu_opt;
    current.modules_mode = cfg.modules_mode;
    current.lle_modules = cfg.lle_modules;
    current.backend_renderer = cfg.backend_renderer;
    current.gpu_idx = cfg.gpu_idx;
#ifdef __ANDROID__
    current.custom_driver_name = cfg.custom_driver_name;
#endif
    current.high_accuracy = cfg.high_accuracy;
    current.resolution_multiplier = cfg.resolution_multiplier;
    current.disable_surface_sync = cfg.disable_surface_sync;
    current.screen_filter = cfg.screen_filter;
    current.memory_mapping = cfg.memory_mapping;
    current.v_sync = cfg.v_sync;
    current.anisotropic_filtering = cfg.anisotropic_filtering;
    current.async_pipeline_compilation = cfg.async_pipeline_compilation;
    current.import_textures = cfg.import_textures;
    current.export_textures = cfg.export_textures;
    current.export_as_png = cfg.export_as_png;
    current.fps_hack = cfg.fps_hack;
    current.shader_cache = cfg.shader_cache;
    current.spirv_shader = cfg.spirv_shader;
    current.texture_cache = cfg.texture_cache;
    current.audio_backend = cfg.audio_backend;
    current.audio_volume = cfg.audio_volume;
    current.ngs_enable = cfg.ngs_enable;
    current.pstv_mode = cfg.pstv_mode;
    current.stretch_the_display_area = cfg.stretch_the_display_area;
    current.fullscreen_hd_res_pixel_perfect = cfg.fullscreen_hd_res_pixel_perfect;
    current.file_loading_delay = cfg.file_loading_delay;
    current.psn_signed_in = cfg.psn_signed_in;
    current.sys_button = cfg.sys_button;
    current.sys_lang = cfg.sys_lang;
    current.sys_date_format = cfg.sys_date_format;
    current.sys_time_format = cfg.sys_time_format;
    current.ime_langs = cfg.ime_langs;
    current.log_active_shaders = cfg.log_active_shaders;
    current.log_uniforms = cfg.log_uniforms;
    current.color_surface_debug = cfg.color_surface_debug;
    current.validation_layer = cfg.validation_layer;
    current.tracy_primitive_impl = cfg.tracy_primitive_impl;
    current.tracy_advanced_profiling_modules = cfg.tracy_advanced_profiling_modules;
}

void copy_current_to_global(Config &cfg, const Config::CurrentConfig &current) {
    cfg.cpu_opt = current.cpu_opt;
    cfg.modules_mode = current.modules_mode;
    cfg.lle_modules = current.lle_modules;
    cfg.backend_renderer = current.backend_renderer;
    cfg.gpu_idx = current.gpu_idx;
#ifdef __ANDROID__
    cfg.custom_driver_name = current.custom_driver_name;
#endif
    cfg.high_accuracy = current.high_accuracy;
    cfg.resolution_multiplier = current.resolution_multiplier;
    cfg.disable_surface_sync = current.disable_surface_sync;
    cfg.screen_filter = current.screen_filter;
    cfg.memory_mapping = current.memory_mapping;
    cfg.v_sync = current.v_sync;
    cfg.anisotropic_filtering = current.anisotropic_filtering;
    cfg.async_pipeline_compilation = current.async_pipeline_compilation;
    cfg.import_textures = current.import_textures;
    cfg.export_textures = current.export_textures;
    cfg.export_as_png = current.export_as_png;
    cfg.fps_hack = current.fps_hack;
    cfg.shader_cache = current.shader_cache;
    cfg.spirv_shader = current.spirv_shader;
    cfg.texture_cache = current.texture_cache;
    cfg.audio_backend = current.audio_backend;
    cfg.audio_volume = current.audio_volume;
    cfg.ngs_enable = current.ngs_enable;
    cfg.pstv_mode = current.pstv_mode;
    cfg.stretch_the_display_area = current.stretch_the_display_area;
    cfg.fullscreen_hd_res_pixel_perfect = current.fullscreen_hd_res_pixel_perfect;
    cfg.file_loading_delay = current.file_loading_delay;
    cfg.psn_signed_in = current.psn_signed_in;
    cfg.sys_button = current.sys_button;
    cfg.sys_lang = current.sys_lang;
    cfg.sys_date_format = current.sys_date_format;
    cfg.sys_time_format = current.sys_time_format;
    cfg.ime_langs = current.ime_langs;
    cfg.log_active_shaders = current.log_active_shaders;
    cfg.log_uniforms = current.log_uniforms;
    cfg.color_surface_debug = current.color_surface_debug;
    cfg.validation_layer = current.validation_layer;
    cfg.tracy_primitive_impl = current.tracy_primitive_impl;
    cfg.tracy_advanced_profiling_modules = current.tracy_advanced_profiling_modules;
}

} // namespace

std::vector<RestartRequiredSetting> get_restart_required_settings(
    const Config::CurrentConfig &before,
    const Config::CurrentConfig &after) {
    std::vector<RestartRequiredSetting> changed;

    const auto append_if_changed = [&](bool has_changed, RestartRequiredSetting setting) {
        if (has_changed)
            changed.emplace_back(setting);
    };

    append_if_changed(before.cpu_opt != after.cpu_opt, RestartRequiredSetting::CpuOpt);
    append_if_changed(before.backend_renderer != after.backend_renderer, RestartRequiredSetting::BackendRenderer);
    append_if_changed(before.gpu_idx != after.gpu_idx, RestartRequiredSetting::GraphicsDevice);
#ifdef __ANDROID__
    append_if_changed(before.custom_driver_name != after.custom_driver_name, RestartRequiredSetting::CustomDriver);
#endif
    append_if_changed(before.high_accuracy != after.high_accuracy, RestartRequiredSetting::HighAccuracy);
    append_if_changed(before.resolution_multiplier != after.resolution_multiplier, RestartRequiredSetting::ResolutionMultiplier);
    append_if_changed(before.memory_mapping != after.memory_mapping, RestartRequiredSetting::MemoryMapping);
    append_if_changed(before.audio_backend != after.audio_backend, RestartRequiredSetting::AudioBackend);
    append_if_changed(before.validation_layer != after.validation_layer, RestartRequiredSetting::ValidationLayer);

    return changed;
}

static fs::path get_custom_config_path(const fs::path &config_path, const std::string &app_path) {
    return config_path / "config" / fmt::format("config_{}.xml", app_path);
}

namespace {

// Every per-app setting and its place in a custom config file ("<section> <name>=..."). The visitor gets
// (section, name, field of a, field of b), so this one list drives loading, saving and copying.
template <typename A, typename B, typename Visitor>
void for_each_custom_field(A &a, B &b, Visitor &&visit) {
    visit("core", "modules-mode", a.modules_mode, b.modules_mode);
    visit("core", "lle-modules", a.lle_modules, b.lle_modules);

    visit("cpu", "cpu-opt", a.cpu_opt, b.cpu_opt);

    visit("gpu", "backend-renderer", a.backend_renderer, b.backend_renderer);
    visit("gpu", "gpu-idx", a.gpu_idx, b.gpu_idx);
#ifdef __ANDROID__
    visit("gpu", "custom-driver-name", a.custom_driver_name, b.custom_driver_name);
#endif
    visit("gpu", "high-accuracy", a.high_accuracy, b.high_accuracy);
    visit("gpu", "resolution-multiplier", a.resolution_multiplier, b.resolution_multiplier);
    visit("gpu", "disable-surface-sync", a.disable_surface_sync, b.disable_surface_sync);
    visit("gpu", "screen-filter", a.screen_filter, b.screen_filter);
    visit("gpu", "memory-mapping", a.memory_mapping, b.memory_mapping);
    visit("gpu", "v-sync", a.v_sync, b.v_sync);
    visit("gpu", "anisotropic-filtering", a.anisotropic_filtering, b.anisotropic_filtering);
    visit("gpu", "async-pipeline-compilation", a.async_pipeline_compilation, b.async_pipeline_compilation);
    visit("gpu", "import-textures", a.import_textures, b.import_textures);
    visit("gpu", "export-textures", a.export_textures, b.export_textures);
    visit("gpu", "export-as-png", a.export_as_png, b.export_as_png);
    visit("gpu", "fps-hack", a.fps_hack, b.fps_hack);
    visit("gpu", "shader-cache", a.shader_cache, b.shader_cache);
    visit("gpu", "spirv-shader", a.spirv_shader, b.spirv_shader);
    visit("gpu", "texture-cache", a.texture_cache, b.texture_cache);

    visit("audio", "audio-backend", a.audio_backend, b.audio_backend);
    visit("audio", "audio-volume", a.audio_volume, b.audio_volume);
    visit("audio", "enable-ngs", a.ngs_enable, b.ngs_enable);

    visit("system", "pstv-mode", a.pstv_mode, b.pstv_mode);
    visit("system", "sys-button", a.sys_button, b.sys_button);
    visit("system", "sys-lang", a.sys_lang, b.sys_lang);
    visit("system", "sys-date-format", a.sys_date_format, b.sys_date_format);
    visit("system", "sys-time-format", a.sys_time_format, b.sys_time_format);
    visit("system", "ime-langs", a.ime_langs, b.ime_langs);

    visit("emulator", "file-loading-delay", a.file_loading_delay, b.file_loading_delay);
    visit("emulator", "stretch-the-display-area", a.stretch_the_display_area, b.stretch_the_display_area);
    visit("emulator", "fullscreen-hd-res-pixel-perfect", a.fullscreen_hd_res_pixel_perfect, b.fullscreen_hd_res_pixel_perfect);

    visit("debug", "log-active-shaders", a.log_active_shaders, b.log_active_shaders);
    visit("debug", "log-uniforms", a.log_uniforms, b.log_uniforms);
    visit("debug", "color-surface-debug", a.color_surface_debug, b.color_surface_debug);
    visit("debug", "validation-layer", a.validation_layer, b.validation_layer);

    visit("network", "psn-signed-in", a.psn_signed_in, b.psn_signed_in);
}

template <typename T>
constexpr bool is_list_v = false;
template <typename T>
constexpr bool is_list_v<std::vector<T>> = true;

std::string custom_key(const char *section, const char *name) {
    return fmt::format("{}/{}", section, name);
}

// Lists are stored as child elements, one item each.
const char *list_item_name(const std::vector<std::string> &) {
    return "module";
}

const char *list_item_name(const std::vector<uint64_t> &) {
    return "lang";
}

// A custom config only overrides what it names: an absent attribute (or list) keeps the global value.
// Unknown sections and attributes are skipped.
void read_value(const pugi::xml_node &node, const char *name, bool &out) {
    if (const auto attr = node.attribute(name))
        out = attr.as_bool();
}

void read_value(const pugi::xml_node &node, const char *name, int &out) {
    if (const auto attr = node.attribute(name))
        out = attr.as_int();
}

void read_value(const pugi::xml_node &node, const char *name, float &out) {
    if (const auto attr = node.attribute(name))
        out = attr.as_float();
}

void read_value(const pugi::xml_node &node, const char *name, std::string &out) {
    const auto attr = node.attribute(name);
    if (!attr)
        return;
    // An empty string is not a valid renderer, filter, mapping or backend, so it inherits too.
    // An empty custom driver name is a real choice (no custom driver).
    if (*attr.as_string() || std::string_view(name) == "custom-driver-name")
        out = attr.as_string();
}

void read_value(const pugi::xml_node &node, const char *name, std::vector<std::string> &out) {
    const auto list = node.child(name);
    if (!list)
        return;
    out.clear();
    for (const auto &item : list)
        out.emplace_back(item.text().as_string());
}

void read_value(const pugi::xml_node &node, const char *name, std::vector<uint64_t> &out) {
    const auto list = node.child(name);
    if (!list)
        return;
    out.clear();
    for (const auto &item : list) {
        const char *text = item.text().as_string();
        char *end = nullptr;
        const auto value = std::strtoull(text, &end, 10);
        if (end != text)
            out.push_back(value);
    }
    if (out.empty())
        out.push_back(4);
}

// Edits a custom config in place. With an override map, a key it names is written when true and removed
// when false. Any other key is written when the file already names it (the user pinned it) or when it
// differs from the global value, and is left absent otherwise so it inherits.
class CustomConfigWriter {
public:
    CustomConfigWriter(pugi::xml_node config, const CustomConfigOverrides *overrides)
        : config(config)
        , overrides(overrides) {}

    template <typename T>
    void field(const char *section, const char *name, const T &value, const T &global) {
        auto node = config.child(section);
        const bool named = node && (is_list_v<T> ? static_cast<bool>(node.child(name)) : static_cast<bool>(node.attribute(name)));

        bool write = named || !(value == global);
        if (overrides) {
            if (const auto it = overrides->find(custom_key(section, name)); it != overrides->end())
                write = it->second;
        }

        if (!write) {
            if (named)
                remove(node, name, is_list_v<T>);
            return;
        }

        if (!node)
            node = config.append_child(section);
        set(node, name, value);
    }

private:
    void remove(pugi::xml_node node, const char *name, bool list) {
        if (list)
            node.remove_child(name);
        else
            node.remove_attribute(name);
        if (!node.first_attribute() && !node.first_child())
            config.remove_child(node);
    }

    static pugi::xml_attribute attribute(pugi::xml_node node, const char *name) {
        auto attr = node.attribute(name);
        return attr ? attr : node.append_attribute(name);
    }

    static void set(pugi::xml_node node, const char *name, bool value) { attribute(node, name).set_value(value); }
    static void set(pugi::xml_node node, const char *name, int value) { attribute(node, name).set_value(value); }
    static void set(pugi::xml_node node, const char *name, float value) { attribute(node, name).set_value(value); }
    static void set(pugi::xml_node node, const char *name, const std::string &value) { attribute(node, name).set_value(value.c_str()); }

    template <typename T>
    static void set(pugi::xml_node node, const char *name, const std::vector<T> &value) {
        node.remove_child(name);
        auto list = node.append_child(name);
        for (const auto &v : value) {
            auto text = list.append_child(list_item_name(value)).append_child(pugi::node_pcdata);
            if constexpr (std::is_same_v<T, std::string>)
                text.set_value(v.c_str());
            else
                text.set_value(std::to_string(v).c_str());
        }
    }

    pugi::xml_node config;
    const CustomConfigOverrides *overrides;
};

// JSON (read as YAML, which accepts it) for --config-override. A value of the wrong type is skipped.
template <typename T>
void read_json_value(const YAML::Node &node, T &out) {
    try {
        out = node.as<T>();
    } catch (const YAML::Exception &) {
        LOG_WARN("--config-override: ignoring a value that is not the expected type.");
    }
}

void read_json_value(const YAML::Node &node, std::string &out) {
    if (!node.IsScalar())
        return;
    const auto value = node.as<std::string>();
    if (!value.empty())
        out = value;
}

template <typename T>
void read_json_value(const YAML::Node &node, std::vector<T> &out) {
    if (!node.IsSequence())
        return;
    std::vector<T> values;
    for (const auto &item : node) {
        T value{};
        try {
            value = item.as<T>();
        } catch (const YAML::Exception &) {
            continue;
        }
        values.push_back(value);
    }
    if constexpr (std::is_same_v<T, uint64_t>) {
        if (values.empty())
            values.push_back(4);
    }
    out = std::move(values);
}

std::string json_string(const std::string &value) {
    std::string out = "\"";
    for (const char c : value) {
        if (c == '"' || c == '\\')
            out += '\\';
        out += c;
    }
    return out + '"';
}

std::string json_value(bool value) {
    return value ? "true" : "false";
}

std::string json_value(int value) {
    return std::to_string(value);
}

std::string json_value(float value) {
    return fmt::format("{}", value);
}

std::string json_value(const std::string &value) {
    return json_string(value);
}

template <typename T>
std::string json_value(const std::vector<T> &value) {
    std::string out = "[";
    for (size_t i = 0; i < value.size(); ++i) {
        if (i)
            out += ',';
        if constexpr (std::is_same_v<T, std::string>)
            out += json_string(value[i]);
        else
            out += std::to_string(value[i]);
    }
    return out + ']';
}

bool load_custom_config_doc(pugi::xml_document &doc, const fs::path &custom_cfg_path) {
    return fs::exists(custom_cfg_path) && doc.load_file(custom_cfg_path.c_str()) && !doc.child("config").empty();
}

} // namespace

bool load_custom_config(Config::CurrentConfig &out, const fs::path &config_path, const std::string &app_path) {
    if (app_path.empty())
        return false;

    const auto custom_cfg_path = get_custom_config_path(config_path, app_path);
    if (!fs::exists(custom_cfg_path))
        return false;

    pugi::xml_document doc;
    if (!doc.load_file(custom_cfg_path.c_str()) || doc.child("config").empty()) {
        LOG_ERROR("Custom config at {} is corrupted or invalid.", custom_cfg_path);
        fs::remove(custom_cfg_path);
        return false;
    }

    const auto config_child = doc.child("config");
    for_each_custom_field(out, out, [&](const char *section, const char *name, auto &value, auto &) {
        read_value(config_child.child(section), name, value);
    });

    return true;
}

std::set<std::string> get_custom_config_keys(const fs::path &config_path, const std::string &app_path) {
    std::set<std::string> keys;
    if (app_path.empty())
        return keys;

    pugi::xml_document doc;
    if (!load_custom_config_doc(doc, get_custom_config_path(config_path, app_path)))
        return keys;

    const auto config_child = doc.child("config");
    Config::CurrentConfig unused;
    for_each_custom_field(unused, unused, [&](const char *section, const char *name, auto &value, auto &) {
        const auto node = config_child.child(section);
        const bool named = is_list_v<std::remove_reference_t<decltype(value)>>
            ? static_cast<bool>(node.child(name))
            : static_cast<bool>(node.attribute(name));
        if (named)
            keys.insert(custom_key(section, name));
    });

    return keys;
}

bool apply_config_override(Config::CurrentConfig &out, const std::string &json, std::string *error) {
    YAML::Node root;
    try {
        root = YAML::Load(json);
    } catch (const YAML::Exception &e) {
        if (error)
            *error = e.what();
        return false;
    }
    if (root.IsNull())
        return true;
    if (!root.IsMap()) {
        if (error)
            *error = "expected an object like {\"gpu\": {\"resolution-multiplier\": 3}}";
        return false;
    }

    const YAML::Node &const_root = root; // const lookups never insert
    for_each_custom_field(out, out, [&](const char *section, const char *name, auto &value, auto &) {
        const YAML::Node section_node = const_root[section];
        // A missing key gives an invalid node: IsDefined() is the only safe first question.
        if (!section_node.IsDefined() || !section_node.IsMap())
            return;
        const YAML::Node value_node = section_node[name];
        if (value_node.IsDefined() && !value_node.IsNull())
            read_json_value(value_node, value);
    });

    return true;
}

std::string custom_config_to_json(const Config::CurrentConfig &cc, const std::vector<std::string> &keys) {
    std::string out = "{";
    std::string current_section;
    for_each_custom_field(cc, cc, [&](const char *section, const char *name, const auto &value, const auto &) {
        if (!std::ranges::contains(keys, custom_key(section, name)))
            return;
        if (current_section != section) {
            if (!current_section.empty())
                out += "},";
            out += json_string(section) + ":{";
            current_section = section;
        } else {
            out += ',';
        }
        out += json_string(name) + ':' + json_value(value);
    });
    if (!current_section.empty())
        out += '}';
    return out + '}';
}

void copy_custom_config_keys(Config::CurrentConfig &dst, const Config::CurrentConfig &src, const std::vector<std::string> &keys) {
    for_each_custom_field(dst, src, [&](const char *section, const char *name, auto &to, const auto &from) {
        if (std::ranges::contains(keys, custom_key(section, name)))
            to = from;
    });
}

bool save_custom_config(const Config::CurrentConfig &cc, const Config::CurrentConfig &global, const fs::path &config_path, const std::string &app_path,
    const CustomConfigOverrides *overrides) {
    if (app_path.empty())
        return false;

    const auto dir = config_path / "config";
    fs::create_directories(dir);

    const auto custom_cfg_path = get_custom_config_path(config_path, app_path);

    pugi::xml_document doc;
    // Keep what a hand-written file carries besides settings (declaration, comments, unknown keys).
    if (!fs::exists(custom_cfg_path) || !doc.load_file(custom_cfg_path.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_comments) || doc.child("config").empty()) {
        doc.reset();
        auto decl = doc.append_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "utf-8";
        doc.append_child("config");
    }

    CustomConfigWriter writer(doc.child("config"), overrides);
    for_each_custom_field(cc, global, [&](const char *section, const char *name, const auto &value, const auto &global_value) {
        writer.field(section, name, value, global_value);
    });

    if (!doc.save_file(custom_cfg_path.c_str())) {
        LOG_ERROR("Failed to save custom config xml for app path: {}", app_path);
        return false;
    }

    return true;
}

bool delete_custom_config(const fs::path &config_path, const std::string &app_path) {
    if (app_path.empty())
        return false;

    const auto custom_cfg_path = get_custom_config_path(config_path, app_path);
    if (fs::exists(custom_cfg_path)) {
        fs::remove(custom_cfg_path);
        return true;
    }
    return false;
}

int delete_all_custom_configs(const fs::path &config_path) {
    const auto config_dir = config_path / "config";
    if (!fs::exists(config_dir) || !fs::is_directory(config_dir))
        return 0;

    int removed = 0;
    for (const auto &entry : fs::directory_iterator(config_dir)) {
        if (!entry.is_regular_file())
            continue;

        const auto file_name = entry.path().filename().string();
        if (!entry.path().has_extension() || entry.path().extension() != ".xml")
            continue;
        if (file_name.rfind("config_", 0) != 0)
            continue;

        boost::system::error_code error;
        if (fs::remove(entry.path(), error) && !error)
            ++removed;
    }

    return removed;
}

bool has_custom_config(const fs::path &config_path, const std::string &app_path) {
    if (app_path.empty())
        return false;
    return fs::exists(get_custom_config_path(config_path, app_path));
}

void set_current_config(Config &cfg, const fs::path &config_path, const std::string &app_path, bool use_config_override) {
    copy_global_to_current(cfg.current_config, cfg);
    if (app_path.empty())
        return;

    // The command line override replaces the app's custom config entirely.
    if (use_config_override && cfg.config_override && app_path == cfg.config_override_app)
        apply_config_override(cfg.current_config, *cfg.config_override);
    else
        load_custom_config(cfg.current_config, config_path, app_path);
}

void copy_current_config_to_global(Config &cfg) {
    copy_current_to_global(cfg, cfg.current_config);
}

void save_current_config(Config &cfg, const fs::path &config_path, const std::string &app_path, bool create_custom_if_missing,
    const CustomConfigOverrides *overrides) {
    if (!app_path.empty() && (create_custom_if_missing || has_custom_config(config_path, app_path))) {
        Config::CurrentConfig global;
        copy_global_to_current(global, cfg);
        save_custom_config(cfg.current_config, global, config_path, app_path, overrides);
    } else {
        copy_current_config_to_global(cfg);
    }
    serialize_config(cfg, config_path);
}

std::vector<std::pair<std::string, bool>> get_modules_list(
    const fs::path &vita_fs_path,
    const std::vector<std::string> &lle_modules) {
    std::vector<std::pair<std::string, bool>> modules;

    const auto modules_path = vita_fs_path / "vs0/sys/external/";
    if (fs::exists(modules_path) && !fs::is_empty(modules_path)) {
        for (const auto &entry : fs::directory_iterator(modules_path)) {
            if (entry.path().extension() == ".suprx")
                modules.emplace_back(entry.path().filename().replace_extension().string(), false);
        }

        for (auto &m : modules)
            m.second = std::ranges::contains(lle_modules, m.first);

        std::sort(modules.begin(), modules.end(), [](const auto &a, const auto &b) {
            if (a.second == b.second)
                return a.first < b.first;
            return a.second;
        });
    }

    return modules;
}

} // namespace config

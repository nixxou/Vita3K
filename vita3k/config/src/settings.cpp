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

#include <algorithm>
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

// A custom config only overrides what it names: an absent attribute (or list) keeps the global value.
static void read_attr(const pugi::xml_node &node, const char *name, bool &out) {
    if (const auto attr = node.attribute(name))
        out = attr.as_bool();
}

static void read_attr(const pugi::xml_node &node, const char *name, int &out) {
    if (const auto attr = node.attribute(name))
        out = attr.as_int();
}

static void read_attr(const pugi::xml_node &node, const char *name, float &out) {
    if (const auto attr = node.attribute(name))
        out = attr.as_float();
}

// An empty string is never a valid value for these settings, so it inherits too.
static void read_attr(const pugi::xml_node &node, const char *name, std::string &out) {
    const auto attr = node.attribute(name);
    if (attr && *attr.as_string())
        out = attr.as_string();
}

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

    const auto core = config_child.child("core");
    read_attr(core, "modules-mode", out.modules_mode);
    if (const auto lle = core.child("lle-modules")) {
        out.lle_modules.clear();
        for (const auto &m : lle)
            out.lle_modules.emplace_back(m.text().as_string());
    }

    read_attr(config_child.child("cpu"), "cpu-opt", out.cpu_opt);

    const auto gpu = config_child.child("gpu");
    read_attr(gpu, "backend-renderer", out.backend_renderer);
    read_attr(gpu, "gpu-idx", out.gpu_idx);
#ifdef __ANDROID__
    if (const auto attr = gpu.attribute("custom-driver-name"))
        out.custom_driver_name = attr.as_string();
#endif
    read_attr(gpu, "high-accuracy", out.high_accuracy);
    read_attr(gpu, "resolution-multiplier", out.resolution_multiplier);
    read_attr(gpu, "disable-surface-sync", out.disable_surface_sync);
    read_attr(gpu, "screen-filter", out.screen_filter);
    read_attr(gpu, "memory-mapping", out.memory_mapping);
    read_attr(gpu, "v-sync", out.v_sync);
    read_attr(gpu, "anisotropic-filtering", out.anisotropic_filtering);
    read_attr(gpu, "async-pipeline-compilation", out.async_pipeline_compilation);
    read_attr(gpu, "import-textures", out.import_textures);
    read_attr(gpu, "export-textures", out.export_textures);
    read_attr(gpu, "export-as-png", out.export_as_png);
    read_attr(gpu, "fps-hack", out.fps_hack);
    read_attr(gpu, "shader-cache", out.shader_cache);
    read_attr(gpu, "spirv-shader", out.spirv_shader);
    read_attr(gpu, "texture-cache", out.texture_cache);

    const auto audio = config_child.child("audio");
    read_attr(audio, "audio-backend", out.audio_backend);
    read_attr(audio, "audio-volume", out.audio_volume);
    read_attr(audio, "enable-ngs", out.ngs_enable);

    const auto sys = config_child.child("system");
    read_attr(sys, "pstv-mode", out.pstv_mode);
    read_attr(sys, "sys-button", out.sys_button);
    read_attr(sys, "sys-lang", out.sys_lang);
    read_attr(sys, "sys-date-format", out.sys_date_format);
    read_attr(sys, "sys-time-format", out.sys_time_format);
    if (const auto ime = sys.child("ime-langs")) {
        out.ime_langs.clear();
        for (const auto &lang : ime)
            out.ime_langs.push_back(std::stoull(lang.text().as_string()));
        if (out.ime_langs.empty())
            out.ime_langs.push_back(4);
    }

    const auto emu = config_child.child("emulator");
    read_attr(emu, "file-loading-delay", out.file_loading_delay);
    read_attr(emu, "stretch-the-display-area", out.stretch_the_display_area);
    read_attr(emu, "fullscreen-hd-res-pixel-perfect", out.fullscreen_hd_res_pixel_perfect);

    const auto dbg = config_child.child("debug");
    read_attr(dbg, "log-active-shaders", out.log_active_shaders);
    read_attr(dbg, "log-uniforms", out.log_uniforms);
    read_attr(dbg, "color-surface-debug", out.color_surface_debug);
    read_attr(dbg, "validation-layer", out.validation_layer);

    read_attr(config_child.child("network"), "psn-signed-in", out.psn_signed_in);

    return true;
}

namespace {

// Edits a custom config in place: a setting is written when the file already names it (the user
// pinned it) or when it differs from the global value; everything else stays absent and inherits.
class CustomConfigWriter {
public:
    explicit CustomConfigWriter(pugi::xml_node config)
        : config(config) {}

    template <typename T>
    void attr(const char *section, const char *name, const T &value, const T &global) {
        auto node = config.child(section);
        if (!(node && node.attribute(name)) && value == global)
            return;
        if (!node)
            node = config.append_child(section);
        auto attribute = node.attribute(name);
        if (!attribute)
            attribute = node.append_attribute(name);
        set(attribute, value);
    }

    template <typename T>
    void list(const char *section, const char *name, const char *item, const std::vector<T> &value, const std::vector<T> &global) {
        auto node = config.child(section);
        if (!(node && node.child(name)) && value == global)
            return;
        if (!node)
            node = config.append_child(section);
        node.remove_child(name);
        auto list_node = node.append_child(name);
        for (const auto &v : value) {
            auto text = list_node.append_child(item).append_child(pugi::node_pcdata);
            if constexpr (std::is_same_v<T, std::string>)
                text.set_value(v.c_str());
            else
                text.set_value(std::to_string(v).c_str());
        }
    }

private:
    static void set(pugi::xml_attribute &attribute, bool value) { attribute.set_value(value); }
    static void set(pugi::xml_attribute &attribute, int value) { attribute.set_value(value); }
    static void set(pugi::xml_attribute &attribute, float value) { attribute.set_value(value); }
    static void set(pugi::xml_attribute &attribute, const std::string &value) { attribute.set_value(value.c_str()); }

    pugi::xml_node config;
};

} // namespace

bool save_custom_config(const Config::CurrentConfig &cc, const Config::CurrentConfig &global, const fs::path &config_path, const std::string &app_path) {
    if (app_path.empty())
        return false;

    const auto dir = config_path / "config";
    fs::create_directories(dir);

    const auto custom_cfg_path = get_custom_config_path(config_path, app_path);

    pugi::xml_document doc;
    // Keep what a hand-written file carries besides settings (declaration, comments).
    if (!fs::exists(custom_cfg_path) || !doc.load_file(custom_cfg_path.c_str(), pugi::parse_default | pugi::parse_declaration | pugi::parse_comments) || doc.child("config").empty()) {
        doc.reset();
        auto decl = doc.append_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "utf-8";
        doc.append_child("config");
    }

    CustomConfigWriter w(doc.child("config"));

    w.attr("core", "modules-mode", cc.modules_mode, global.modules_mode);
    w.list("core", "lle-modules", "module", cc.lle_modules, global.lle_modules);

    w.attr("cpu", "cpu-opt", cc.cpu_opt, global.cpu_opt);

    w.attr("gpu", "backend-renderer", cc.backend_renderer, global.backend_renderer);
    w.attr("gpu", "gpu-idx", cc.gpu_idx, global.gpu_idx);
#ifdef __ANDROID__
    w.attr("gpu", "custom-driver-name", cc.custom_driver_name, global.custom_driver_name);
#endif
    w.attr("gpu", "high-accuracy", cc.high_accuracy, global.high_accuracy);
    w.attr("gpu", "resolution-multiplier", cc.resolution_multiplier, global.resolution_multiplier);
    w.attr("gpu", "disable-surface-sync", cc.disable_surface_sync, global.disable_surface_sync);
    w.attr("gpu", "screen-filter", cc.screen_filter, global.screen_filter);
    w.attr("gpu", "memory-mapping", cc.memory_mapping, global.memory_mapping);
    w.attr("gpu", "v-sync", cc.v_sync, global.v_sync);
    w.attr("gpu", "anisotropic-filtering", cc.anisotropic_filtering, global.anisotropic_filtering);
    w.attr("gpu", "async-pipeline-compilation", cc.async_pipeline_compilation, global.async_pipeline_compilation);
    w.attr("gpu", "import-textures", cc.import_textures, global.import_textures);
    w.attr("gpu", "export-textures", cc.export_textures, global.export_textures);
    w.attr("gpu", "export-as-png", cc.export_as_png, global.export_as_png);
    w.attr("gpu", "fps-hack", cc.fps_hack, global.fps_hack);
    w.attr("gpu", "shader-cache", cc.shader_cache, global.shader_cache);
    w.attr("gpu", "spirv-shader", cc.spirv_shader, global.spirv_shader);
    w.attr("gpu", "texture-cache", cc.texture_cache, global.texture_cache);

    w.attr("audio", "audio-backend", cc.audio_backend, global.audio_backend);
    w.attr("audio", "audio-volume", cc.audio_volume, global.audio_volume);
    w.attr("audio", "enable-ngs", cc.ngs_enable, global.ngs_enable);

    w.attr("system", "pstv-mode", cc.pstv_mode, global.pstv_mode);
    w.attr("system", "sys-button", cc.sys_button, global.sys_button);
    w.attr("system", "sys-lang", cc.sys_lang, global.sys_lang);
    w.attr("system", "sys-date-format", cc.sys_date_format, global.sys_date_format);
    w.attr("system", "sys-time-format", cc.sys_time_format, global.sys_time_format);
    w.list("system", "ime-langs", "lang", cc.ime_langs, global.ime_langs);

    w.attr("emulator", "file-loading-delay", cc.file_loading_delay, global.file_loading_delay);
    w.attr("emulator", "stretch-the-display-area", cc.stretch_the_display_area, global.stretch_the_display_area);
    w.attr("emulator", "fullscreen-hd-res-pixel-perfect", cc.fullscreen_hd_res_pixel_perfect, global.fullscreen_hd_res_pixel_perfect);

    w.attr("debug", "log-active-shaders", cc.log_active_shaders, global.log_active_shaders);
    w.attr("debug", "log-uniforms", cc.log_uniforms, global.log_uniforms);
    w.attr("debug", "color-surface-debug", cc.color_surface_debug, global.color_surface_debug);
    w.attr("debug", "validation-layer", cc.validation_layer, global.validation_layer);

    w.attr("network", "psn-signed-in", cc.psn_signed_in, global.psn_signed_in);

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

void set_current_config(Config &cfg, const fs::path &config_path, const std::string &app_path) {
    copy_global_to_current(cfg.current_config, cfg);
    if (!app_path.empty())
        load_custom_config(cfg.current_config, config_path, app_path);
}

void copy_current_config_to_global(Config &cfg) {
    copy_current_to_global(cfg, cfg.current_config);
}

void save_current_config(Config &cfg, const fs::path &config_path, const std::string &app_path, bool create_custom_if_missing) {
    if (!app_path.empty() && (create_custom_if_missing || has_custom_config(config_path, app_path))) {
        Config::CurrentConfig global;
        copy_global_to_current(global, cfg);
        save_custom_config(cfg.current_config, global, config_path, app_path);
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

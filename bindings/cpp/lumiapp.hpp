/**
 * lumiapp.hpp — C++ namespace wrapper for liblumiapp
 * Copyright 2026 Lumi Team. Apache-2.0
 *
 * Usage: #include <lumiapp.hpp>
 * Provides RAII wrappers and C++ idiomatic API over the C core.
 */

#ifndef LUMIAPP_HPP
#define LUMIAPP_HPP

#include "lumiapp.h"
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <stdexcept>

namespace lumi {

// ── Exception ──────────────────────────────────────────────────

class Error : public std::runtime_error {
public:
    lumi_result_t code;
    Error(lumi_result_t c) : std::runtime_error(lumi_result_str(c)), code(c) {}
};

inline void check(lumi_result_t r) {
    if (r != LUMI_OK) throw Error(r);
}

// ── Log ────────────────────────────────────────────────────────

namespace log {
    inline void verbose(const std::string &tag, const std::string &msg) {
        lumi_log(LUMI_LOG_VERBOSE, tag.c_str(), "%s", msg.c_str());
    }
    inline void debug(const std::string &tag, const std::string &msg) {
        lumi_log(LUMI_LOG_DEBUG, tag.c_str(), "%s", msg.c_str());
    }
    inline void info(const std::string &tag, const std::string &msg) {
        lumi_log(LUMI_LOG_INFO, tag.c_str(), "%s", msg.c_str());
    }
    inline void warn(const std::string &tag, const std::string &msg) {
        lumi_log(LUMI_LOG_WARN, tag.c_str(), "%s", msg.c_str());
    }
    inline void error(const std::string &tag, const std::string &msg) {
        lumi_log(LUMI_LOG_ERROR, tag.c_str(), "%s", msg.c_str());
    }
    inline void set_level(lumi_log_level_t level) {
        lumi_log_set_level(level);
    }
}

// ── View (RAII wrapper) ────────────────────────────────────────

class View {
    lumi_view_t *handle_;
    bool owned_;
public:
    explicit View(lumi_view_t *h, bool owned = true) : handle_(h), owned_(owned) {
        if (!h) throw Error(LUMI_ERR_NOMEM);
    }
    ~View() { if (owned_ && handle_) lumi_view_destroy(handle_); }

    View(const View &) = delete;
    View &operator=(const View &) = delete;
    View(View &&o) noexcept : handle_(o.handle_), owned_(o.owned_) { o.handle_ = nullptr; }
    View &operator=(View &&o) noexcept {
        if (this != &o) { if (owned_ && handle_) lumi_view_destroy(handle_); handle_ = o.handle_; owned_ = o.owned_; o.handle_ = nullptr; }
        return *this;
    }

    lumi_view_t *raw() const { return handle_; }

    /* Release ownership (e.g. when adding to a parent that will own it) */
    lumi_view_t *release() { owned_ = false; return handle_; }

    void set_id(const std::string &id) { lumi_view_set_id(handle_, id.c_str()); }
    std::string get_id() const { auto s = lumi_view_get_id(handle_); return s ? s : ""; }
    void set_visible(bool v) { lumi_view_set_visible(handle_, v); }
    bool get_visible() const { return lumi_view_get_visible(handle_); }

    void set_width(float w) { lumi_view_set_width(handle_, w); }
    void set_height(float h) { lumi_view_set_height(handle_, h); }
    void set_padding(float t, float r, float b, float l) { lumi_view_set_padding(handle_, t, r, b, l); }
    void set_margin(float t, float r, float b, float l) { lumi_view_set_margin(handle_, t, r, b, l); }
    void set_background(uint32_t rgba) { lumi_view_set_background(handle_, rgba); }
    void set_foreground(uint32_t rgba) { lumi_view_set_foreground(handle_, rgba); }
    void set_font_size(float s) { lumi_view_set_font_size(handle_, s); }
    void set_border_radius(float r) { lumi_view_set_border_radius(handle_, r); }

    void add_child(View &child) { lumi_view_add_child(handle_, child.release()); }

    void on_click(lumi_click_cb cb, void *ud = nullptr) { lumi_view_on_click(handle_, cb, ud); }

    /* Text-specific */
    void set_text(const std::string &t) { lumi_text_set_content(handle_, t.c_str()); }
    std::string get_text() const { auto s = lumi_text_get_content(handle_); return s ? s : ""; }

    /* Factory helpers */
    static View column() { return View(lumi_column()); }
    static View row()    { return View(lumi_row()); }
    static View stack()  { return View(lumi_stack()); }
    static View text(const std::string &s) { return View(lumi_text(s.c_str())); }
    static View button(const std::string &s) { return View(lumi_button(s.c_str())); }
    static View image(const std::string &s) { return View(lumi_image(s.c_str())); }
    static View text_field(const std::string &p) { return View(lumi_text_field(p.c_str())); }
    static View spacer()  { return View(lumi_spacer()); }
    static View divider() { return View(lumi_divider()); }
    static View card()    { return View(lumi_card()); }
};

// ── App (RAII wrapper) ─────────────────────────────────────────

class App {
    lumi_app_t *handle_;
public:
    struct Manifest {
        std::string app_id;
        std::string name;
        std::string version;
        std::string icon;
    };

    struct Lifecycle {
        std::function<void()> on_create;
        std::function<void()> on_start;
        std::function<void()> on_resume;
        std::function<void()> on_pause;
        std::function<void()> on_stop;
        std::function<void()> on_destroy;
    };

    App(const Manifest &m, const Lifecycle &lc) {
        lumi_manifest_t cm = { m.app_id.c_str(), m.name.c_str(), m.version.c_str(), m.icon.c_str() };
        auto *data = new Lifecycle(lc);
        lumi_lifecycle_t clc = {
            .on_create  = [](lumi_app_t*, void *ud) { auto *l = (Lifecycle*)ud; if (l->on_create) l->on_create(); },
            .on_start   = [](lumi_app_t*, void *ud) { auto *l = (Lifecycle*)ud; if (l->on_start) l->on_start(); },
            .on_resume  = [](lumi_app_t*, void *ud) { auto *l = (Lifecycle*)ud; if (l->on_resume) l->on_resume(); },
            .on_pause   = [](lumi_app_t*, void *ud) { auto *l = (Lifecycle*)ud; if (l->on_pause) l->on_pause(); },
            .on_stop    = [](lumi_app_t*, void *ud) { auto *l = (Lifecycle*)ud; if (l->on_stop) l->on_stop(); },
            .on_destroy = [](lumi_app_t*, void *ud) { auto *l = (Lifecycle*)ud; if (l->on_destroy) l->on_destroy(); delete l; },
        };
        handle_ = lumi_app_create(&cm, &clc, data);
        if (!handle_) { delete data; throw Error(LUMI_ERR_NOMEM); }
    }

    ~App() { if (handle_) lumi_app_destroy(handle_); }

    App(const App &) = delete;
    App &operator=(const App &) = delete;

    int run() { return lumi_app_run(handle_); }
    void quit() { lumi_app_quit(handle_); }
    void set_content(View &root) { lumi_app_set_content(handle_, root.release()); }

    lumi_app_t *raw() { return handle_; }
};

// ── Storage ────────────────────────────────────────────────────

namespace storage {
    inline void set(const std::string &key, const std::string &val) { check(lumi_storage_set(key.c_str(), val.c_str())); }
    inline std::string get(const std::string &key) { auto v = lumi_storage_get(key.c_str()); return v ? v : ""; }
    inline void remove(const std::string &key) { check(lumi_storage_remove(key.c_str())); }
    inline void clear() { check(lumi_storage_clear()); }
}

// ── Notify ─────────────────────────────────────────────────────

inline void notify(const std::string &title, const std::string &body) {
    check(lumi_notify_simple(title.c_str(), body.c_str()));
}

} // namespace lumi

#endif // LUMIAPP_HPP

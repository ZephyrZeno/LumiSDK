/**
 * hello_cpp — LumiOS app using C++ bindings
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.hpp"

int main() {
    lumi::App::Manifest manifest {
        .app_id  = "com.lumios.hello_cpp",
        .name    = "Hello C++",
        .version = "1.0.0",
    };

    lumi::App::Lifecycle lifecycle {
        .on_create = []() {
            lumi::log::info("hello", "C++ App created!");
        },
        .on_destroy = []() {
            lumi::log::info("hello", "C++ App destroyed!");
        },
    };

    try {
        lumi::App app(manifest, lifecycle);

        auto root = lumi::View::column();
        auto title = lumi::View::text("Hello from C++!");
        title.set_font_size(24.0f);
        root.add_child(title);

        auto btn = lumi::View::button("Click Me");
        btn.set_background(0x6200EEFF);
        btn.set_foreground(0xFFFFFFFF);
        root.add_child(btn);

        app.set_content(root);
        return app.run();
    } catch (const lumi::Error &e) {
        lumi::log::error("hello", std::string("Error: ") + e.what());
        return 1;
    }
}

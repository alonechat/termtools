#pragma once
#include "buffer.h"
#include <unordered_map>
#include <string>

namespace tt {
namespace pixel {

struct Style {
    Color fg = Color::white();
    Color bg = Color::black();
    char ch = ' ';
    uint8_t flags = FLAG_NONE;
};

struct Stylesheet {
    std::unordered_map<std::string, Style> styles;
    
    void set(const std::string& name, const Style& style) {
        styles[name] = style;
    }
    
    Style get(const std::string& name, const Style& fallback = Style()) const {
        auto it = styles.find(name);
        return it != styles.end() ? it->second : fallback;
    }
    
    bool has(const std::string& name) const {
        return styles.find(name) != styles.end();
    }
    
    void clear() { styles.clear(); }
    
    static Stylesheet defaultTheme() {
        Stylesheet theme;
        theme.set("default", Style{Color::white(), Color::black(), ' ', FLAG_NONE});
        theme.set("bold", Style{Color::white(), Color::black(), ' ', FLAG_BOLD});
        theme.set("title", Style{Color::cyan(), Color::black(), ' ', FLAG_BOLD});
        theme.set("header", Style{Color::yellow(), Color::black(), ' ', FLAG_BOLD});
        theme.set("dim", Style{Color::gray(128), Color::black(), ' ', FLAG_NONE});
        theme.set("inverse", Style{Color::black(), Color::white(), ' ', FLAG_INVERSE});
        return theme;
    }
};

}
}
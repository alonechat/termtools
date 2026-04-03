#include "termtools/ui/group/group.h"
#include "termtools/core/internal/internal.h"
#include <vector>
#include <sstream>

namespace termtools {

Group::Group() {}

Group::Group(const std::string& title)
    : title_(title) {}

Group& Group::add(const Text& text) {
    renderables_.push_back([text]() { return text.render(); });
    return *this;
}

Group& Group::add(const std::string& text) {
    renderables_.push_back([text]() { return text; });
    return *this;
}

Group& Group::add(const Panel& panel) {
    renderables_.push_back([panel]() { return panel.render(); });
    return *this;
}

Group& Group::add(const Table& table) {
    renderables_.push_back([table]() { return table.render(); });
    return *this;
}

Group& Group::setTitle(const std::string& title) {
    title_ = title;
    return *this;
}

Group& Group::setBorderStyle(const StyleBuilder& style) {
    borderStyle_ = style;
    return *this;
}

std::string Group::render() const {
    std::ostringstream oss;
    
    if (!title_.empty()) {
        StyleBuilder titleStyle;
        titleStyle.bold().color(Color(ColorName::Cyan));
        oss << titleStyle.toAnsi() << title_ << titleStyle.toAnsiReset() << "\n";
    }
    
    for (const auto& renderable : renderables_) {
        oss << renderable() << "\n";
    }
    
    return oss.str();
}

void Group::print(Console& console) const {
    console.print(render());
}

}

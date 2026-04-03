#include "termtools/ui/list/list.h"
#include <iostream>
#include <sstream>

namespace termtools {

ListItem::ListItem()
    : indent_(0) {
}

ListItem::ListItem(const std::string &text)
    : text_(text)
    , indent_(0) {
}

ListItem::ListItem(const std::string &text, const StyleBuilder &style)
    : text_(text)
    , style_(style)
    , indent_(0) {
}

ListItem &ListItem::setText(const std::string &text) {
    text_ = text;
    return *this;
}

ListItem &ListItem::setStyle(const StyleBuilder &style) {
    style_ = style;
    return *this;
}

ListItem &ListItem::setIndent(int indent) {
    indent_ = indent;
    return *this;
}

std::string ListItem::getText() const {
    return text_;
}

int ListItem::getIndent() const {
    return indent_;
}

List::List()
    : type_(ListType::Unordered)
    , color_(ColorName::Magenta)
    , bullet_("•")
    , indentSize_(2)
    , startNumber_(1) {
}

List::List(ListType type)
    : type_(type)
    , color_(ColorName::Magenta)
    , bullet_("•")
    , indentSize_(2)
    , startNumber_(1) {
}

List &List::setType(ListType type) {
    type_ = type;
    return *this;
}

List &List::addItem(const std::string &item) {
    items_.push_back(ListItem(item));
    return *this;
}

List &List::addItem(const ListItem &item) {
    items_.push_back(item);
    return *this;
}

List &List::setColor(const Color &color) {
    color_ = color;
    return *this;
}

List &List::setBullet(const std::string &bullet) {
    bullet_ = bullet;
    return *this;
}

List &List::setIndentSize(int size) {
    indentSize_ = size;
    return *this;
}

List &List::setStartNumber(int start) {
    startNumber_ = start;
    return *this;
}

ListType List::getType() const {
    return type_;
}

std::vector<ListItem> List::getItems() const {
    return items_;
}

std::string List::renderItem(const ListItem &item, int number) const {
    StyleBuilder markerStyle;
    markerStyle.color(color_);
    
    std::ostringstream oss;
    
    std::string indentation(item.indent_ * indentSize_, ' ');
    
    oss << indentation;
    
    if (type_ == ListType::Unordered) {
        oss << markerStyle.toAnsi() << bullet_ << " " << markerStyle.toAnsiReset();
    } else {
        oss << markerStyle.toAnsi() << number << ". " << markerStyle.toAnsiReset();
    }
    
    oss << item.text_ << "\n\n";
    
    return oss.str();
}

std::string List::render() const {
    std::ostringstream oss;
    int number = startNumber_;
    
    for (const auto &item : items_) {
        oss << renderItem(item, number);
        if (type_ == ListType::Ordered) {
            number++;
        }
    }
    
    return oss.str();
}

void List::print() const {
    std::cout << render() << std::flush;
}

std::string List::render(const std::vector<std::string> &items, ListType type, const Color &color) {
    List list(type);
    list.setColor(color);
    for (const auto &item : items) {
        list.addItem(item);
    }
    return list.render();
}

void List::print(const std::vector<std::string> &items, ListType type, const Color &color) {
    std::cout << render(items, type, color) << std::flush;
}

}

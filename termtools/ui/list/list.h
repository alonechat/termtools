#ifndef TERMTOOLS_UI_LIST_H
#define TERMTOOLS_UI_LIST_H

#include "termtools/core/formatting/style.h"
#include "termtools/core/formatting/text.h"
#include <string>
#include <vector>

namespace termtools {

enum class ListType : uint8_t {
    Unordered,
    Ordered
};

class ListItem {
  public:
    ListItem();
    explicit ListItem(const std::string &text);
    ListItem(const std::string &text, const StyleBuilder &style);
    
    ListItem &setText(const std::string &text);
    ListItem &setStyle(const StyleBuilder &style);
    ListItem &setIndent(int indent);
    
    std::string getText() const;
    int getIndent() const;
    
  private:
    friend class List;
    std::string text_;
    StyleBuilder style_;
    int indent_;
};

class List {
  public:
    List();
    explicit List(ListType type);
    
    List &setType(ListType type);
    List &addItem(const std::string &item);
    List &addItem(const ListItem &item);
    List &setColor(const Color &color);
    List &setBullet(const std::string &bullet);
    List &setIndentSize(int size);
    List &setStartNumber(int start);
    
    ListType getType() const;
    std::vector<ListItem> getItems() const;
    
    std::string render() const;
    void print() const;
    
    static std::string render(const std::vector<std::string> &items, ListType type = ListType::Unordered,
                               const Color &color = Color(ColorName::Magenta));
    static void print(const std::vector<std::string> &items, ListType type = ListType::Unordered,
                      const Color &color = Color(ColorName::Magenta));

  private:
    ListType type_;
    std::vector<ListItem> items_;
    Color color_;
    std::string bullet_;
    int indentSize_;
    int startNumber_;
    
    std::string renderItem(const ListItem &item, int number) const;
};

}
#endif

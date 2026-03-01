#ifndef TERMTOOLS_GROUP_H
#define TERMTOOLS_GROUP_H

#include "termtools/core/console.h"
#include "termtools/core/style.h"
#include "termtools/text/text.h"
#include "termtools/ui/panel.h"
#include "termtools/ui/table.h"

#include <functional>
#include <string>
#include <vector>

namespace termtools {

class Group {
  public:
    Group();
    explicit Group(const std::string &title);

    Group &add(const Text &text);
    Group &add(const std::string &text);
    Group &add(const Panel &panel);
    Group &add(const Table &table);

    Group &setTitle(const std::string &title);
    Group &setBorderStyle(const StyleBuilder &style);

    std::string render() const;
    void print(Console &console) const;

  private:
    std::string title_;
    std::vector<std::function<std::string()>> renderables_;
    StyleBuilder borderStyle_;
};

} 

#endif

#ifndef TERMTOOLS_COLUMNS_H
#define TERMTOOLS_COLUMNS_H

#include "termtools/core/output/console.h"
#include "termtools/core/formatting/text.h"

#include <string>
#include <vector>

namespace termtools {

class Columns {
  public:
    Columns();
    explicit Columns(int columnCount);

    Columns &add(const Text &text);
    Columns &add(const std::string &text);
    Columns &add(const std::vector<Text> &texts);

    Columns &setExpand(bool expand);
    Columns &setPadding(int padding);
    Columns &setEqualWidth(bool equalWidth);

    std::string render() const;
    void print(Console &console) const;

  private:
    std::vector<std::vector<Text>> columns_;
    bool expand_;
    int padding_;
    bool equalWidth_;

    std::vector<int> calculateColumnWidths() const;
};

} 

#endif

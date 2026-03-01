#ifndef TERMTOOLS_TEXT_H
#define TERMTOOLS_TEXT_H

#include "termtools/core/style.h"
#include "termtools/utils/utils.h"

#include <string>
#include <vector>

namespace termtools {

class Text {
  public:
    Text();
    explicit Text(const std::string &content);
    Text(const std::string &content, const Color &color);
    Text(const std::string &content, const StyleBuilder &style);

    Text &append(const Text &other);
    Text &append(const std::string &str);
    Text &append(const std::string &str, const Color &color);
    Text &append(const std::string &str, const StyleBuilder &style);

    Text &prepend(const Text &other);
    Text &prepend(const std::string &str);

    Text &styled(const StyleBuilder &style);
    Text &colored(const Color &color);

    Text &justify(Justify justify, int width = 0);
    Text &align(Justify justify, int width = 0);

    Text &padLeft(int width, char fill = ' ');
    Text &padRight(int width, char fill = ' ');

    Text &truncate(int max_width, Overflow overflow = Overflow::Ellipsis);

    std::string plain() const;
    std::string render() const;

    int width() const;
    int length() const;
    bool empty() const;

    static Text fromMarkup(const std::string &markup);

  private:
    struct Segment {
        std::string content;
        StyleBuilder style;
    };
    std::vector<Segment> segments_;
    int calculateWidth() const;
};

} 

#endif

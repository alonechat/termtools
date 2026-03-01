#include "termtools/core/style.h"
#include "internal.h"
#include <sstream>

namespace termtools {

StyleBuilder::StyleBuilder()
    : bold_(false)
    , dim_(false)
    , italic_(false)
    , underline_(false)
    , blink_(false)
    , reverse_(false)
    , hidden_(false)
    , strikethrough_(false)
    , fgColor_()
    , bgColor_() {}

StyleBuilder& StyleBuilder::bold(bool enable) {
    bold_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::dim(bool enable) {
    dim_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::italic(bool enable) {
    italic_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::underline(bool enable) {
    underline_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::blink(bool enable) {
    blink_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::reverse(bool enable) {
    reverse_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::hidden(bool enable) {
    hidden_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::strikethrough(bool enable) {
    strikethrough_ = enable;
    return *this;
}

StyleBuilder& StyleBuilder::color(const Color& color) {
    fgColor_ = color;
    return *this;
}

StyleBuilder& StyleBuilder::bgColor(const Color& color) {
    bgColor_ = color;
    return *this;
}

std::string StyleBuilder::toAnsi() const {
    std::ostringstream oss;
    
    if (bold_) oss << internal::ANSI_BOLD;
    if (dim_) oss << internal::ANSI_DIM;
    if (italic_) oss << internal::ANSI_ITALIC;
    if (underline_) oss << internal::ANSI_UNDERLINE;
    if (blink_) oss << internal::ANSI_BLINK;
    if (reverse_) oss << internal::ANSI_REVERSE;
    if (hidden_) oss << internal::ANSI_HIDDEN;
    if (strikethrough_) oss << internal::ANSI_STRIKETHROUGH;
    
    if (!fgColor_.isDefault()) {
        oss << fgColor_.toAnsi(true);
    }
    
    if (!bgColor_.isDefault()) {
        oss << bgColor_.toAnsi(false);
    }
    
    return oss.str();
}

std::string StyleBuilder::toAnsiReset() const {
    return internal::ANSI_RESET;
}

}

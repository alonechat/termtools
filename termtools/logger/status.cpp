#include "termtools/logger/status.h"
#include "../core/internal.h"
#include <sstream>

namespace termtools {

Status::Status(const std::string& status, const std::string& message)
    : status_(status)
    , message_(message)
    , console_(Console::getStdout())
    , stopped_(false) {
    statusStyle_.color(Color(ColorName::Cyan)).bold();
    render();
}

Status::~Status() {
    if (!stopped_) {
        stop();
    }
}

void Status::update(const std::string& status, const std::string& message) {
    status_ = status;
    message_ = message;
    render();
}

void Status::stop() {
    stopped_ = true;
    clearLastRender();
    console_.println();
}

void Status::setStatusStyle(const StyleBuilder& style) {
    statusStyle_ = style;
}

void Status::setMessageStyle(const StyleBuilder& style) {
    messageStyle_ = style;
}

Status& Status::start(const std::string& status, const std::string& message) {
    static Status instance(status, message);
    return instance;
}

void Status::render() {
    std::ostringstream oss;
    
    oss << statusStyle_.toAnsi() << status_ << statusStyle_.toAnsiReset();
    
    if (!message_.empty()) {
        oss << " " << messageStyle_.toAnsi() << message_ << messageStyle_.toAnsiReset();
    }
    
    std::string rendered = oss.str();
    
    clearLastRender();
    console_.print("\r" + rendered);
    console_.flush();
    
    lastRender_ = rendered;
}

void Status::clearLastRender() {
    if (!lastRender_.empty()) {
        console_.clearLine();
    }
}

}

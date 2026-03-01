#ifndef TERMTOOLS_STATUS_H
#define TERMTOOLS_STATUS_H

#include "termtools/core/console.h"
#include "termtools/core/style.h"

#include <string>

namespace termtools {

class Status {
  public:
    Status(const std::string &status, const std::string &message = "");
    ~Status();

    void update(const std::string &status, const std::string &message = "");
    void stop();

    void setStatusStyle(const StyleBuilder &style);
    void setMessageStyle(const StyleBuilder &style);

    static Status &start(const std::string &status,
                         const std::string &message = "");

  private:
    std::string status_;
    std::string message_;
    StyleBuilder statusStyle_;
    StyleBuilder messageStyle_;
    Console &console_;
    bool stopped_;
    std::string lastRender_;

    void render();
    void clearLastRender();
};

} 

#endif

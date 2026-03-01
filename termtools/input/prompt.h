#ifndef TERMTOOLS_PROMPT_H
#define TERMTOOLS_PROMPT_H

#include <string>
#include <vector>

namespace termtools {

class Prompt {
  public:
    static std::string ask(const std::string &question);
    static std::string ask(const std::string &question,
                           const std::string &defaultAnswer);

    static bool confirm(const std::string &question, bool defaultAnswer = true);

    static int askInt(const std::string &question, int min, int max);
    static int askInt(const std::string &question, int defaultAnswer, int min,
                      int max);

    static float askFloat(const std::string &question, float min = -INFINITY,
                          float max = INFINITY);
    static float askFloat(const std::string &question, float defaultAnswer,
                          float min = -INFINITY, float max = INFINITY);

    static std::string askPassword(const std::string &question);

    static std::string askChoice(const std::string &question,
                                 const std::vector<std::string> &choices);
    static std::string askChoice(const std::string &question,
                                 const std::vector<std::string> &choices,
                                 const std::string &defaultChoice);

    static std::vector<std::string>
    askMultipleChoice(const std::string &question,
                      const std::vector<std::string> &choices);

  private:
    static std::string readLine();
    static bool readYesNo(bool defaultAnswer);
};

} 

#endif

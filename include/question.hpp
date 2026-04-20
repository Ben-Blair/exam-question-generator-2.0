#ifndef QUESTION_HPP
#define QUESTION_HPP

#include <string>
#include <vector>

struct ParsedOption {
    std::string label;
    std::string text;
};

enum class QuestionType {
    MULTIPLE_CHOICE,
    TRUE_FALSE
};

struct Question {
    std::string objective;
    std::string prompt;
    std::string code_sample;
    std::string post_code_prompt;
    QuestionType type = QuestionType::TRUE_FALSE;
    std::vector<ParsedOption> options;
    std::string answer;
    std::string question_number;
};

#endif

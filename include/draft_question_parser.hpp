#ifndef DRAFT_QUESTION_PARSER_HPP
#define DRAFT_QUESTION_PARSER_HPP

#include "option_parser_strategy.hpp"
#include "question.hpp"
#include <string>
#include <vector>

class DraftQuestionParser {
public:
    Question parse_text(const std::string& text) const;
    Question parse_file(const std::string& file_path) const;

    std::vector<Question> parse_all_text(const std::string& text) const;
    std::vector<Question> parse_all_from_file(const std::string& file_path) const;

private:
    OptionParserFactory option_parser_factory_;

    Question parse_single_question(const std::vector<std::string>& lines) const;
};

#endif

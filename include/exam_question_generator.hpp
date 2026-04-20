#ifndef EXAM_QUESTION_GENERATOR_HPP
#define EXAM_QUESTION_GENERATOR_HPP

#include "draft_question_parser.hpp"
#include <string>

class ExamQuestionGenerator {
public:
    std::string generate_from_text(const std::string& text) const;
    std::string generate_from_file(const std::string& file_path) const;

    std::string generate_all_from_text(const std::string& text) const;
    std::string generate_all_from_file(const std::string& file_path) const;

private:
    DraftQuestionParser parser_;
};

#endif

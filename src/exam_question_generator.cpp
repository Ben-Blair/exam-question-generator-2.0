#include "exam_question_generator.hpp"

#include "latex_renderer.hpp"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace {

std::string render_error_block(const std::string& message) {
    std::ostringstream out;
    out << "\\**\\*******\\ Error encountered - " << message << "\n"
        << "\\**\\*******\\ End of error message\n\n";
    return out.str();
}

}  // namespace

std::string ExamQuestionGenerator::generate_from_text(const std::string& text) const {
    Question question = parser_.parse_text(text);
    return LatexRendererFactory::create(question)->render(question);
}

std::string ExamQuestionGenerator::generate_from_file(const std::string& file_path) const {
    Question question = parser_.parse_file(file_path);
    return LatexRendererFactory::create(question)->render(question);
}

std::string ExamQuestionGenerator::generate_all_from_text(const std::string& text) const {
    std::vector<Question> questions = parser_.parse_all_text(text);
    std::ostringstream out;

    for (std::size_t i = 0; i < questions.size(); ++i) {
        try {
            out << LatexRendererFactory::create(questions[i])->render(questions[i]);
        } catch (const std::exception& ex) {
            out << render_error_block(ex.what());
        }
        if (i + 1 < questions.size()) {
            out << "\n";
        }
    }

    return out.str();
}

std::string ExamQuestionGenerator::generate_all_from_file(const std::string& file_path) const {
    std::vector<Question> questions = parser_.parse_all_from_file(file_path);
    std::ostringstream out;

    for (std::size_t i = 0; i < questions.size(); ++i) {
        try {
            out << LatexRendererFactory::create(questions[i])->render(questions[i]);
        } catch (const std::exception& ex) {
            out << render_error_block(ex.what());
        }
        if (i + 1 < questions.size()) {
            out << "\n";
        }
    }

    return out.str();
}

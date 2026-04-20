#include "latex_renderer.hpp"

#include <sstream>
#include <stdexcept>

namespace {

std::string render_solution_block(const std::string& answer) {
    std::ostringstream out;
    out << "\\begin{solution}\n"
        << "    " << answer << "\n"
        << "\\end{solution}\n";
    return out.str();
}

std::string render_error_block(const std::string& message) {
    std::ostringstream out;
    out << "\\**\\*******\\ Error encountered - " << message << "\n"
        << "\\**\\*******\\ End of error message\n";
    return out.str();
}

}  // namespace

std::string LatexRendererTemplate::render(const Question& question) const {
    std::ostringstream out;

    if (!question.question_number.empty()) {
        out << "% Question " << question.question_number << "\n";
    }

    out << "\\qtag {" << question.objective << " }\n\n";
    out << render_body(question);
    out << "\n";

    if (question.answer.empty()) {
        out << render_error_block("Answer line is missing for question about \""
                                  + question.prompt.substr(0, 60) + "\"");
    } else {
        out << render_solution(question);
    }
    return out.str();
}

std::string MultipleChoiceLatexRenderer::render_body(const Question& question) const {
    std::ostringstream out;
    out << "\\item " << question.prompt << "\n";

    if (!question.code_sample.empty()) {
        out << "\n\\begin{lstlisting}\n";
        out << question.code_sample << "\n";
        out << "\\end{lstlisting}\n";
    }

    if (!question.post_code_prompt.empty()) {
        out << "\n" << question.post_code_prompt << "\n";
    }

    if (question.options.empty()) {
        out << render_error_block(
            "No options found for multiple-choice question about \""
            + question.prompt.substr(0, 60) + "\"");
    } else {
        out << "\\begin{enumerate}[label=\\Alph*.]\n";
        for (const ParsedOption& option : question.options) {
            out << "\\item " << option.text << "\n";
        }
        out << "\\end{enumerate}\n";
    }
    return out.str();
}

std::string MultipleChoiceLatexRenderer::render_solution(const Question& question) const {
    return render_solution_block(question.answer);
}

std::string TrueFalseLatexRenderer::render_body(const Question& question) const {
    std::ostringstream out;
    out << "\\item \n";
    out << question.prompt << " \n";

    if (!question.code_sample.empty()) {
        out << "\n\\begin{lstlisting}\n";
        out << question.code_sample << "\n";
        out << "\\end{lstlisting}\n";
    }

    if (!question.post_code_prompt.empty()) {
        out << "\n" << question.post_code_prompt << " \n";
    }
    return out.str();
}

std::string TrueFalseLatexRenderer::render_solution(const Question& question) const {
    return render_solution_block(question.answer);
}

std::unique_ptr<LatexRendererTemplate> LatexRendererFactory::create(const Question& question) {
    if (question.type == QuestionType::MULTIPLE_CHOICE) {
        return std::unique_ptr<LatexRendererTemplate>(new MultipleChoiceLatexRenderer());
    }
    if (question.type == QuestionType::TRUE_FALSE) {
        return std::unique_ptr<LatexRendererTemplate>(new TrueFalseLatexRenderer());
    }
    throw std::runtime_error("Unknown question type.");
}

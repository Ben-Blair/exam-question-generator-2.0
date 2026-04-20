#include "draft_question_parser.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {

std::string trim(const std::string& text) {
    const std::string whitespace = " \t\r\n";
    const std::size_t start = text.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const std::size_t end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

bool starts_with_ignore_case(const std::string& text, const std::string& prefix) {
    if (text.size() < prefix.size()) {
        return false;
    }
    for (std::size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(text[i])) !=
            std::tolower(static_cast<unsigned char>(prefix[i]))) {
            return false;
        }
    }
    return true;
}

std::string strip_prefix(const std::string& text, const std::string& prefix) {
    return trim(text.substr(prefix.size()));
}

std::string normalize_answer(std::string answer_text) {
    answer_text = trim(answer_text);
    if (answer_text.empty()) {
        return answer_text;
    }

    std::string lowered = answer_text;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    if (lowered == "true" || lowered == "t") {
        return "T";
    }
    if (lowered == "false" || lowered == "f") {
        return "F";
    }

    return std::string(1, static_cast<char>(std::toupper(static_cast<unsigned char>(answer_text[0]))));
}

std::string to_lower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return out;
}

bool line_ends_with_code_trigger(const std::string& line) {
    std::string low = to_lower(line);
    const std::string triggers[] = {
        "consider the following code.",
        "consider the following pseudocode.",
        "consider the following code",
        "consider the following pseudocode"
    };
    for (const auto& t : triggers) {
        if (low.size() >= t.size() && low.substr(low.size() - t.size()) == t) {
            return true;
        }
    }
    return false;
}

bool is_code_like_line(const std::string& line) {
    if (line.empty()) return true;
    if (!line.empty() && (line[0] == ' ' || line[0] == '\t')) return true;

    const std::string code_chars[] = {"{", "}", ";", "#", "//", "/*", "<<", ">>", "->", "::"};
    for (const auto& cc : code_chars) {
        if (line.find(cc) != std::string::npos) return true;
    }

    const std::string keywords[] = {
        "int ", "void ", "double ", "float ", "char ", "bool ",
        "return ", "if ", "if(", "else", "for ", "for(", "while ",
        "while(", "class ", "struct ", "using ", "namespace ",
        "cout", "cin", "endl", "nullptr", "new ", "delete ",
        "function ", "def ", "print(", "print "
    };
    std::string low = to_lower(line);
    for (const auto& kw : keywords) {
        if (starts_with_ignore_case(low, kw) || low.find(kw) != std::string::npos) {
            return true;
        }
    }

    return false;
}

std::string extract_question_number(const std::string& text, std::string& remainder) {
    std::string trimmed = trim(text);
    if (trimmed.empty()) {
        remainder = trimmed;
        return "";
    }

    std::size_t i = 0;
    while (i < trimmed.size() && std::isdigit(static_cast<unsigned char>(trimmed[i]))) {
        ++i;
    }
    if (i > 0 && (i == trimmed.size() || std::isspace(static_cast<unsigned char>(trimmed[i])))) {
        std::string number = trimmed.substr(0, i);
        remainder = trim(trimmed.substr(i));
        return number;
    }

    remainder = trimmed;
    return "";
}

enum class ParseState {
    IDLE,
    PROMPT,
    CODE,
    POST_CODE,
    OPTIONS
};

}  // namespace

Question DraftQuestionParser::parse_single_question(const std::vector<std::string>& lines) const {
    Question question;
    ParseState state = ParseState::IDLE;
    bool saw_blank_in_code = false;
    std::vector<std::string> code_buffer;

    for (std::size_t idx = 0; idx < lines.size(); ++idx) {
        const std::string trimmed = trim(lines[idx]);

        if (starts_with_ignore_case(trimmed, "Answer:")) {
            question.answer = normalize_answer(strip_prefix(trimmed, "Answer:"));
            continue;
        }
        if (starts_with_ignore_case(trimmed, "Answer")) {
            std::string after = strip_prefix(trimmed, "Answer");
            if (!after.empty() && !std::isalpha(static_cast<unsigned char>(after[0]))) {
                question.answer = normalize_answer(after);
                continue;
            }
            if (!after.empty()) {
                question.answer = normalize_answer(after);
                continue;
            }
        }

        if (starts_with_ignore_case(trimmed, "Objective:")) {
            question.objective = trimmed;
            continue;
        }
        if (starts_with_ignore_case(trimmed, "Objective")) {
            std::string after = strip_prefix(trimmed, "Objective");
            if (after.empty() || after[0] == ':') {
                question.objective = trimmed;
                continue;
            }
        }

        switch (state) {
        case ParseState::IDLE: {
            if (trimmed.empty()) continue;

            if (starts_with_ignore_case(trimmed, "Question")) {
                std::string after_question = strip_prefix(trimmed, "Question");
                std::string remainder;
                question.question_number = extract_question_number(after_question, remainder);
                if (!remainder.empty()) {
                    question.prompt = remainder;
                }
                state = ParseState::PROMPT;

                if (!question.prompt.empty() && line_ends_with_code_trigger(question.prompt)) {
                    state = ParseState::CODE;
                    saw_blank_in_code = false;
                    code_buffer.clear();
                }
                continue;
            }

            if (starts_with_ignore_case(trimmed, "Consider the following")) {
                question.prompt = trimmed;
                state = ParseState::PROMPT;
                if (line_ends_with_code_trigger(trimmed)) {
                    state = ParseState::CODE;
                    saw_blank_in_code = false;
                    code_buffer.clear();
                }
                continue;
            }

            question.prompt = trimmed;
            state = ParseState::PROMPT;
            if (line_ends_with_code_trigger(trimmed)) {
                state = ParseState::CODE;
                saw_blank_in_code = false;
                code_buffer.clear();
            }
            continue;
        }

        case ParseState::PROMPT: {
            if (trimmed.empty()) continue;

            if (option_parser_factory_.can_parse(trimmed)) {
                const OptionParserStrategy& parser = option_parser_factory_.create_parser_for(trimmed);
                question.options.push_back(parser.parse(trimmed));
                state = ParseState::OPTIONS;
                continue;
            }

            if (question.prompt.empty()) {
                question.prompt = trimmed;
            } else {
                question.prompt += " " + trimmed;
            }

            if (line_ends_with_code_trigger(question.prompt)) {
                state = ParseState::CODE;
                saw_blank_in_code = false;
                code_buffer.clear();
            }
            continue;
        }

        case ParseState::CODE: {
            if (option_parser_factory_.can_parse(trimmed)) {
                for (const auto& cl : code_buffer) {
                    if (!question.code_sample.empty()) question.code_sample += "\n";
                    question.code_sample += cl;
                }

                const OptionParserStrategy& parser = option_parser_factory_.create_parser_for(trimmed);
                question.options.push_back(parser.parse(trimmed));
                state = ParseState::OPTIONS;
                continue;
            }

            if (trimmed.empty()) {
                saw_blank_in_code = true;
                code_buffer.push_back("");
                continue;
            }

            if (saw_blank_in_code && !is_code_like_line(trimmed)) {
                for (const auto& cl : code_buffer) {
                    if (!question.code_sample.empty()) question.code_sample += "\n";
                    question.code_sample += cl;
                }
                while (!question.code_sample.empty() &&
                       (question.code_sample.back() == '\n' || question.code_sample.back() == ' ')) {
                    question.code_sample.pop_back();
                }

                question.post_code_prompt = trimmed;
                state = ParseState::POST_CODE;
                continue;
            }

            code_buffer.push_back(lines[idx]);
            continue;
        }

        case ParseState::POST_CODE: {
            if (trimmed.empty()) continue;

            if (option_parser_factory_.can_parse(trimmed)) {
                const OptionParserStrategy& parser = option_parser_factory_.create_parser_for(trimmed);
                question.options.push_back(parser.parse(trimmed));
                state = ParseState::OPTIONS;
                continue;
            }

            if (question.post_code_prompt.empty()) {
                question.post_code_prompt = trimmed;
            } else {
                question.post_code_prompt += " " + trimmed;
            }
            continue;
        }

        case ParseState::OPTIONS: {
            if (trimmed.empty()) continue;

            if (option_parser_factory_.can_parse(trimmed)) {
                const OptionParserStrategy& parser = option_parser_factory_.create_parser_for(trimmed);
                question.options.push_back(parser.parse(trimmed));
                continue;
            }

            if (!question.options.empty()) {
                question.options.back().text += " " + trimmed;
            }
            continue;
        }
        }
    }

    if (state == ParseState::CODE) {
        for (const auto& cl : code_buffer) {
            if (!question.code_sample.empty()) question.code_sample += "\n";
            question.code_sample += cl;
        }
        while (!question.code_sample.empty() &&
               (question.code_sample.back() == '\n' || question.code_sample.back() == ' ')) {
            question.code_sample.pop_back();
        }
    }

    if (question.objective.empty()) {
        question.objective = "Objective is a work in progress";
    }

    question.type = question.options.empty() ? QuestionType::TRUE_FALSE
                                             : QuestionType::MULTIPLE_CHOICE;
    return question;
}

std::vector<Question> DraftQuestionParser::parse_all_text(const std::string& text) const {
    std::istringstream input(text);
    std::string line;
    std::vector<std::string> all_lines;
    while (std::getline(input, line)) {
        all_lines.push_back(line);
    }

    std::vector<std::vector<std::string>> chunks;
    std::vector<std::string> current_chunk;
    bool chunk_has_substance = false;
    bool chunk_has_answer = false;

    for (std::size_t i = 0; i < all_lines.size(); ++i) {
        const std::string trimmed = trim(all_lines[i]);

        bool is_consider = starts_with_ignore_case(trimmed, "Consider the following");
        bool is_code_trigger_here = line_ends_with_code_trigger(trimmed);


        bool is_boundary = starts_with_ignore_case(trimmed, "Objective") ||
                           starts_with_ignore_case(trimmed, "Question") ||
                           (is_consider &&
                            (!is_code_trigger_here || !chunk_has_substance || chunk_has_answer));

        if (is_boundary && chunk_has_substance) {
            chunks.push_back(current_chunk);
            current_chunk.clear();
            chunk_has_substance = false;
            chunk_has_answer = false;
        }

        current_chunk.push_back(all_lines[i]);

        if (starts_with_ignore_case(trimmed, "Question") ||
            starts_with_ignore_case(trimmed, "Consider the following")) {
            chunk_has_substance = true;
        }
        if (starts_with_ignore_case(trimmed, "Answer")) {
            chunk_has_substance = true;
            chunk_has_answer = true;
        }
    }
    if (!current_chunk.empty()) {
        chunks.push_back(current_chunk);
    }

    std::vector<Question> questions;
    for (const auto& chunk : chunks) {
        bool all_empty = true;
        for (const auto& l : chunk) {
            if (!trim(l).empty()) { all_empty = false; break; }
        }
        if (all_empty) continue;

        questions.push_back(parse_single_question(chunk));
    }

    return questions;
}

std::vector<Question> DraftQuestionParser::parse_all_from_file(const std::string& file_path) const {
    std::ifstream input(file_path);
    if (!input.is_open()) {
        throw std::runtime_error("Could not open input file: " + file_path);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return parse_all_text(buffer.str());
}

Question DraftQuestionParser::parse_text(const std::string& text) const {
    std::vector<Question> questions = parse_all_text(text);
    if (questions.empty()) {
        throw std::runtime_error("No questions found in input.");
    }
    return questions[0];
}

Question DraftQuestionParser::parse_file(const std::string& file_path) const {
    std::vector<Question> questions = parse_all_from_file(file_path);
    if (questions.empty()) {
        throw std::runtime_error("No questions found in input file: " + file_path);
    }
    return questions[0];
}

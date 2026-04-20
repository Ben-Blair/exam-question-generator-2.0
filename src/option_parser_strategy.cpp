#include "option_parser_strategy.hpp"

#include <cctype>
#include <stdexcept>

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

char normalize_label(char label) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(label)));
}

bool is_choice_letter(char c) {
    c = normalize_label(c);
    return c >= 'A' && c <= 'E';
}

ParsedOption make_option(char label, const std::string& text) {
    return ParsedOption{std::string(1, normalize_label(label)), trim(text)};
}

}  // namespace

bool LetterPeriodOptionParser::can_parse(const std::string& line) const {
    return line.size() >= 3 && is_choice_letter(line[0]) && line[1] == '.' &&
           std::isspace(static_cast<unsigned char>(line[2]));
}

ParsedOption LetterPeriodOptionParser::parse(const std::string& line) const {
    if (!can_parse(line)) {
        throw std::invalid_argument("Line is not in letter-period format.");
    }
    return make_option(line[0], line.substr(2));
}

bool LetterParenOptionParser::can_parse(const std::string& line) const {
    return line.size() >= 3 && is_choice_letter(line[0]) && line[1] == ')' &&
           std::isspace(static_cast<unsigned char>(line[2]));
}

ParsedOption LetterParenOptionParser::parse(const std::string& line) const {
    if (!can_parse(line)) {
        throw std::invalid_argument("Line is not in letter-parenthesis format.");
    }
    return make_option(line[0], line.substr(2));
}

bool BareLetterOptionParser::can_parse(const std::string& line) const {
    return line.size() >= 2 && is_choice_letter(line[0]) &&
           std::isspace(static_cast<unsigned char>(line[1]));
}

ParsedOption BareLetterOptionParser::parse(const std::string& line) const {
    if (!can_parse(line)) {
        throw std::invalid_argument("Line is not in bare-letter format.");
    }
    return make_option(line[0], line.substr(1));
}

OptionParserFactory::OptionParserFactory() {
    parsers_.push_back(std::make_shared<LetterPeriodOptionParser>());
    parsers_.push_back(std::make_shared<LetterParenOptionParser>());
    parsers_.push_back(std::make_shared<BareLetterOptionParser>());
}

bool OptionParserFactory::can_parse(const std::string& line) const {
    for (const auto& parser : parsers_) {
        if (parser->can_parse(line)) {
            return true;
        }
    }
    return false;
}

const OptionParserStrategy& OptionParserFactory::create_parser_for(const std::string& line) const {
    for (const auto& parser : parsers_) {
        if (parser->can_parse(line)) {
            return *parser;
        }
    }
    throw std::invalid_argument("No parser strategy matches the option line.");
}

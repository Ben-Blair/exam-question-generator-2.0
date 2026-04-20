#ifndef OPTION_PARSER_STRATEGY_HPP
#define OPTION_PARSER_STRATEGY_HPP

#include "question.hpp"
#include <memory>
#include <string>
#include <vector>

class OptionParserStrategy {
public:
    virtual ~OptionParserStrategy() = default;
    virtual bool can_parse(const std::string& line) const = 0;
    virtual ParsedOption parse(const std::string& line) const = 0;
};

class LetterPeriodOptionParser : public OptionParserStrategy {
public:
    bool can_parse(const std::string& line) const override;
    ParsedOption parse(const std::string& line) const override;
};

class LetterParenOptionParser : public OptionParserStrategy {
public:
    bool can_parse(const std::string& line) const override;
    ParsedOption parse(const std::string& line) const override;
};

class BareLetterOptionParser : public OptionParserStrategy {
public:
    bool can_parse(const std::string& line) const override;
    ParsedOption parse(const std::string& line) const override;
};

class OptionParserFactory {
public:
    OptionParserFactory();

    bool can_parse(const std::string& line) const;
    const OptionParserStrategy& create_parser_for(const std::string& line) const;

private:
    std::vector<std::shared_ptr<OptionParserStrategy>> parsers_;
};

#endif

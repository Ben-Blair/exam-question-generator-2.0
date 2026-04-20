#ifndef LATEX_RENDERER_HPP
#define LATEX_RENDERER_HPP

#include "question.hpp"
#include <memory>
#include <string>

class LatexRendererTemplate {
public:
    virtual ~LatexRendererTemplate() = default;

    std::string render(const Question& question) const;

protected:
    virtual std::string render_body(const Question& question) const = 0;
    virtual std::string render_solution(const Question& question) const = 0;
};

class MultipleChoiceLatexRenderer : public LatexRendererTemplate {
protected:
    std::string render_body(const Question& question) const override;
    std::string render_solution(const Question& question) const override;
};

class TrueFalseLatexRenderer : public LatexRendererTemplate {
protected:
    std::string render_body(const Question& question) const override;
    std::string render_solution(const Question& question) const override;
};

class LatexRendererFactory {
public:
    static std::unique_ptr<LatexRendererTemplate> create(const Question& question);
};

#endif

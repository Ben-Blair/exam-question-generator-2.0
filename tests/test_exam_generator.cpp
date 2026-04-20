#include "draft_question_parser.hpp"
#include "exam_question_generator.hpp"
#include "question.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

// Unit tests for parser and LaTeX generator (GoogleTest).

TEST(DraftQuestionParser, ParsesLetterPeriodOptions) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 12.C Explain encapsulation in classes\n\n"
        "Question Which member should usually stay private?\n"
        "A. Constructor\n"
        "B. Data field\n"
        "C. Include guard\n"
        "D. Namespace\n\n"
        "Answer B\n";

    Question question = parser.parse_text(input);

    EXPECT_EQ(question.type, QuestionType::MULTIPLE_CHOICE);
    EXPECT_EQ(question.options.size(), 4u);
    EXPECT_EQ(question.options[0].label, "A");
    EXPECT_EQ(question.options[1].text, "Data field");
    EXPECT_EQ(question.answer, "B");
}

TEST(DraftQuestionParser, ParsesBareLetterOptions) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 10.A Recognize constructor behavior\n\n"
        "Question Which statement describes a constructor?\n"
        "A Runs automatically when an object is created\n"
        "B Deletes an object from memory\n"
        "C Replaces all member functions\n"
        "D Prevents compilation\n\n"
        "Answer A\n";

    Question question = parser.parse_text(input);

    EXPECT_EQ(question.type, QuestionType::MULTIPLE_CHOICE);
    EXPECT_EQ(question.options.size(), 4u);
    EXPECT_EQ(question.options[2].label, "C");
    EXPECT_EQ(question.options[0].text, "Runs automatically when an object is created");
    EXPECT_EQ(question.answer, "A");
}

TEST(DraftQuestionParser, ParsesLetterParenOptions) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 9.B Identify polymorphism examples\n\n"
        "Question Which example shows runtime polymorphism?\n"
        "a) Calling an overridden virtual method through a base pointer\n"
        "b) Declaring a local integer\n"
        "c) Writing a comment\n"
        "d) Including a header file\n\n"
        "Answer: a\n";

    Question question = parser.parse_text(input);

    EXPECT_EQ(question.type, QuestionType::MULTIPLE_CHOICE);
    EXPECT_EQ(question.options.size(), 4u);
    EXPECT_EQ(question.options[0].label, "A");
    EXPECT_EQ(question.options[0].text, "Calling an overridden virtual method through a base pointer");
    EXPECT_EQ(question.answer, "A");
}

TEST(DraftQuestionParser, ParsesMixedOptionFormatsInOneQuestion) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 11.A Explain inheritance relationships\n\n"
        "Question Which statement best describes inheritance?\n"
        "A. A derived class can reuse behavior from a base class\n"
        "B A derived class must remove every base member\n"
        "c A class can inherit only comments\n"
        "d) Inheritance always replaces polymorphism\n\n"
        "Answer A\n";

    Question question = parser.parse_text(input);

    EXPECT_EQ(question.type, QuestionType::MULTIPLE_CHOICE);
    EXPECT_EQ(question.options.size(), 4u);
    EXPECT_EQ(question.options[0].label, "A");
    EXPECT_EQ(question.options[1].label, "B");
    EXPECT_EQ(question.options[2].label, "C");
    EXPECT_EQ(question.options[3].label, "D");
    EXPECT_EQ(question.options[0].text, "A derived class can reuse behavior from a base class");
    EXPECT_EQ(question.answer, "A");
}

TEST(ExamQuestionGenerator, GeneratesLatexForMultipleChoiceQuestion) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 12.C Explain encapsulation in classes\n\n"
        "Question Which member should usually stay private?\n"
        "A. Constructor\n"
        "B. Data field\n"
        "C. Include guard\n"
        "D. Namespace\n\n"
        "Answer B\n";

    const std::string latex = generator.generate_from_text(input);

    EXPECT_NE(latex.find("\\qtag {Objective: 12.C Explain encapsulation in classes }"), std::string::npos);
    EXPECT_NE(latex.find("\\item Which member should usually stay private?"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{enumerate}[label=\\Alph*.]"), std::string::npos);
    EXPECT_NE(latex.find("\\item Data field"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{solution}\n    B\n\\end{solution}"), std::string::npos);
}

TEST(ExamQuestionGenerator, GeneratesLatexForTrueFalseQuestion) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 6.D Identify languages that are compiled\n\n"
        "Question T/F Java source code is compiled before execution.\n"
        "Answer: True\n";

    const std::string latex = generator.generate_from_text(input);

    EXPECT_NE(latex.find("\\item \nT/F Java source code is compiled before execution."), std::string::npos);
    EXPECT_EQ(latex.find("\\begin{enumerate}[label=\\Alph*.]"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{solution}\n    T\n\\end{solution}"), std::string::npos);
}

TEST(ExamQuestionGenerator, GeneratesLatexFromTextFile) {
    ExamQuestionGenerator generator;
    const std::string sample_path = std::string(PROJECT_SOURCE_DIR) + "/samples/multiple_choice_sample.txt";

    const std::string latex = generator.generate_from_file(sample_path);

    EXPECT_NE(latex.find("\\item Which C++ feature allows one interface to support many implementations?"), std::string::npos);
    EXPECT_NE(latex.find("\\item Polymorphism"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{solution}\n    A\n\\end{solution}"), std::string::npos);
}

TEST(ExamQuestionGenerator, GeneratesLatexFromMixedFormatTextFile) {
    ExamQuestionGenerator generator;
    const std::string sample_path = std::string(PROJECT_SOURCE_DIR) + "/samples/mixed_multiple_choice_sample.txt";

    const std::string latex = generator.generate_from_file(sample_path);

    EXPECT_NE(latex.find("\\item Which statement best describes inheritance?"), std::string::npos);
    EXPECT_NE(latex.find("\\item A derived class can reuse behavior from a base class"), std::string::npos);
    EXPECT_NE(latex.find("\\item Inheritance always replaces polymorphism"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{solution}\n    A\n\\end{solution}"), std::string::npos);
}

TEST(MultiQuestion, ParsesMultipleQuestionsFromSameInput) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 1.A First objective\n"
        "Question What is 1+1?\n"
        "A. 1\n"
        "B. 2\n"
        "C. 3\n"
        "Answer B\n"
        "\n"
        "Objective: 2.B Second objective\n"
        "Question T/F The sky is blue.\n"
        "Answer True\n";

    std::vector<Question> questions = parser.parse_all_text(input);

    ASSERT_EQ(questions.size(), 2u);
    EXPECT_EQ(questions[0].prompt, "What is 1+1?");
    EXPECT_EQ(questions[0].answer, "B");
    EXPECT_EQ(questions[0].type, QuestionType::MULTIPLE_CHOICE);
    EXPECT_EQ(questions[1].prompt, "T/F The sky is blue.");
    EXPECT_EQ(questions[1].answer, "T");
    EXPECT_EQ(questions[1].type, QuestionType::TRUE_FALSE);
}

TEST(MultiQuestion, GenerateAllProducesLatexForAllQuestions) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 1.A First\n"
        "Question What is 2+2?\n"
        "A. 3\n"
        "B. 4\n"
        "Answer B\n"
        "\n"
        "Objective: 2.B Second\n"
        "Question T/F Water is wet.\n"
        "Answer True\n";

    const std::string latex = generator.generate_all_from_text(input);

    EXPECT_NE(latex.find("\\qtag {Objective: 1.A First }"), std::string::npos);
    EXPECT_NE(latex.find("\\item What is 2+2?"), std::string::npos);
    EXPECT_NE(latex.find("\\qtag {Objective: 2.B Second }"), std::string::npos);
    EXPECT_NE(latex.find("T/F Water is wet."), std::string::npos);
}

TEST(MultiQuestion, ConsiderTheFollowingStartsNewQuestion) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 3.A Mem management\n"
        "Question What is a pointer?\n"
        "A. An address\n"
        "B. A value\n"
        "Answer A\n"
        "\n"
        "Consider the following code.\n"
        "#include <iostream>\n"
        "int main() { return 0; }\n"
        "\n"
        "What does this program do?\n"
        "A. Nothing\n"
        "B. Prints hello\n"
        "Answer A\n";

    std::vector<Question> questions = parser.parse_all_text(input);
    ASSERT_EQ(questions.size(), 2u);
    EXPECT_EQ(questions[0].prompt, "What is a pointer?");
    EXPECT_FALSE(questions[1].code_sample.empty());
}

TEST(CodeSample, DetectsCodeTriggerAndCollectsCode) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 5.A Memory\n"
        "Question Consider the following code.\n"
        "#include <iostream>\n"
        "int main() {\n"
        "    return 0;\n"
        "}\n"
        "\n"
        "What does this program output?\n"
        "A. Nothing\n"
        "B. Hello\n"
        "Answer A\n";

    Question q = parser.parse_text(input);

    EXPECT_FALSE(q.code_sample.empty());
    EXPECT_NE(q.code_sample.find("#include <iostream>"), std::string::npos);
    EXPECT_NE(q.code_sample.find("return 0;"), std::string::npos);
    EXPECT_EQ(q.post_code_prompt, "What does this program output?");
}

TEST(CodeSample, RendersLstlisting) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 5.A Memory\n"
        "Question Consider the following code.\n"
        "#include <iostream>\n"
        "int main() {\n"
        "    return 0;\n"
        "}\n"
        "\n"
        "What does this program output?\n"
        "A. Nothing\n"
        "B. Hello\n"
        "Answer A\n";

    const std::string latex = generator.generate_from_text(input);

    EXPECT_NE(latex.find("\\begin{lstlisting}"), std::string::npos);
    EXPECT_NE(latex.find("\\end{lstlisting}"), std::string::npos);
    EXPECT_NE(latex.find("#include <iostream>"), std::string::npos);
}

TEST(CodeSample, DetectsPseudocodeTrigger) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 7.C Linked lists\n"
        "Question Consider the following pseudocode.\n"
        "function insert(head, node):\n"
        "    head.next = node\n"
        "\n"
        "What does this function do?\n"
        "A. Inserts at head\n"
        "B. Inserts at tail\n"
        "Answer A\n";

    Question q = parser.parse_text(input);
    EXPECT_FALSE(q.code_sample.empty());
    EXPECT_NE(q.code_sample.find("function insert"), std::string::npos);
}

TEST(CodeSample, TriggerWithoutPeriod) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 7.C Linked lists\n"
        "Question Consider the following code\n"
        "int x = 5;\n"
        "\n"
        "What is x?\n"
        "A. 5\n"
        "B. 0\n"
        "Answer A\n";

    Question q = parser.parse_text(input);
    EXPECT_FALSE(q.code_sample.empty());
}

TEST(MultiLinePrompt, CollectsMultiLineQuestionText) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 4.B Data structures\n"
        "Question\n"
        "The following function inserts a node into a doubly linked list.\n"
        "Assume that the variable, head, is a pointer to the first node.\n"
        "What is the time complexity?\n"
        "A. O(1)\n"
        "B. O(n)\n"
        "Answer B\n";

    Question q = parser.parse_text(input);

    EXPECT_NE(q.prompt.find("doubly linked list"), std::string::npos);
    EXPECT_NE(q.prompt.find("time complexity"), std::string::npos);
    EXPECT_EQ(q.answer, "B");
}

TEST(MultiLinePrompt, MultiLinePromptWithCodeTriggerAtEnd) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 4.B Data structures\n"
        "Question\n"
        "The following function inserts a node.\n"
        "Consider the following pseudocode.\n"
        "function insert(head, node):\n"
        "    head.next = node\n"
        "\n"
        "What does it do?\n"
        "A. Inserts at head\n"
        "B. Deletes\n"
        "Answer A\n";

    Question q = parser.parse_text(input);

    EXPECT_NE(q.prompt.find("inserts a node"), std::string::npos);
    EXPECT_NE(q.prompt.find("pseudocode"), std::string::npos);
    EXPECT_FALSE(q.code_sample.empty());
    EXPECT_EQ(q.post_code_prompt, "What does it do?");
}

TEST(OptionCount, AcceptsTwoOptions) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 1.A Basics\n"
        "Question Is C++ compiled?\n"
        "A. Yes\n"
        "B. No\n"
        "Answer A\n";

    Question q = parser.parse_text(input);
    EXPECT_EQ(q.options.size(), 2u);
}

TEST(OptionCount, AcceptsThreeOptions) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 1.A Basics\n"
        "Question Pick the color.\n"
        "A. Red\n"
        "B. Blue\n"
        "C. Green\n"
        "Answer B\n";

    Question q = parser.parse_text(input);
    EXPECT_EQ(q.options.size(), 3u);
}

TEST(OptionCount, AcceptsFiveOptions) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 1.A Basics\n"
        "Question Pick the best answer.\n"
        "A. Alpha\n"
        "B. Beta\n"
        "C. Gamma\n"
        "D. Delta\n"
        "E. Epsilon\n"
        "Answer C\n";

    Question q = parser.parse_text(input);
    EXPECT_EQ(q.options.size(), 5u);
    EXPECT_EQ(q.options[4].label, "E");
    EXPECT_EQ(q.options[4].text, "Epsilon");
}

TEST(MultiLineOption, ContinuationLineAppendsToLastOption) {
    DraftQuestionParser parser;
    const std::string input =
        "Objective: 3.A Memory\n"
        "Question What is true?\n"
        "A. The program runs\n"
        "   and terminates normally.\n"
        "B. The program crashes.\n"
        "Answer A\n";

    Question q = parser.parse_text(input);

    ASSERT_EQ(q.options.size(), 2u);
    EXPECT_NE(q.options[0].text.find("terminates normally"), std::string::npos);
}

TEST(MissingObjective, UsesPlaceholder) {
    DraftQuestionParser parser;
    const std::string input =
        "Question What is 1+1?\n"
        "A. 1\n"
        "B. 2\n"
        "Answer B\n";

    Question q = parser.parse_text(input);
    EXPECT_EQ(q.objective, "Objective is a work in progress");
}

TEST(MissingObjective, PlaceholderAppearsInLatex) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Question What color is the sky?\n"
        "A. Green\n"
        "B. Blue\n"
        "Answer B\n";

    const std::string latex = generator.generate_from_text(input);
    EXPECT_NE(latex.find("\\qtag {Objective is a work in progress }"), std::string::npos);
}

TEST(ErrorBlocks, MissingAnswerProducesError) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 1.A Test\n"
        "Question What is this?\n"
        "A. Something\n"
        "B. Nothing\n";

    const std::string latex = generator.generate_all_from_text(input);

    EXPECT_NE(latex.find("Error encountered"), std::string::npos);
    EXPECT_NE(latex.find("End of error message"), std::string::npos);
}

TEST(ErrorBlocks, ContinuesAfterBadQuestion) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 1.A Test\n"
        "Question Bad question with no answer.\n"
        "A. Something\n"
        "B. Nothing\n"
        "\n"
        "Objective: 2.A Good\n"
        "Question What is 1+1?\n"
        "A. 1\n"
        "B. 2\n"
        "Answer B\n";

    const std::string latex = generator.generate_all_from_text(input);

    EXPECT_NE(latex.find("Error encountered"), std::string::npos);
    EXPECT_NE(latex.find("\\item What is 1+1?"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{solution}\n    B\n\\end{solution}"), std::string::npos);
}

TEST(QuestionNumber, ExtractsNumberAndRendersComment) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 1.A Test\n"
        "Question 14 True / False The weather is nice out.\n"
        "Answer True\n";

    const std::string latex = generator.generate_from_text(input);

    EXPECT_NE(latex.find("% Question 14"), std::string::npos);
    EXPECT_NE(latex.find("True / False The weather is nice out."), std::string::npos);
    EXPECT_EQ(latex.find("14 True"), std::string::npos);
}

TEST(QuestionNumber, NoNumberMeansNoComment) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 1.A Test\n"
        "Question What is 1+1?\n"
        "A. 1\n"
        "B. 2\n"
        "Answer B\n";

    const std::string latex = generator.generate_from_text(input);

    EXPECT_EQ(latex.find("% Question"), std::string::npos);
}

TEST(QuestionNumber, OptionLettersGeneratedByLatex) {
    ExamQuestionGenerator generator;
    const std::string input =
        "Objective: 1.A Test\n"
        "Question Pick one.\n"
        "A. First\n"
        "B. Second\n"
        "Answer A\n";

    const std::string latex = generator.generate_from_text(input);

    EXPECT_NE(latex.find("\\begin{enumerate}[label=\\Alph*.]"), std::string::npos);
    EXPECT_NE(latex.find("\\item First"), std::string::npos);
    EXPECT_EQ(latex.find("\\item A. First"), std::string::npos);
    EXPECT_EQ(latex.find("\\item A First"), std::string::npos);
}

TEST(MultiQuestionFile, GeneratesFromMultiQuestionSampleFile) {
    ExamQuestionGenerator generator;
    const std::string sample_path = std::string(PROJECT_SOURCE_DIR) + "/samples/multi_question_sample.txt";

    const std::string latex = generator.generate_all_from_file(sample_path);

    EXPECT_NE(latex.find("\\qtag {"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{enumerate}[label=\\Alph*.]"), std::string::npos);
    EXPECT_NE(latex.find("\\begin{solution}"), std::string::npos);
}

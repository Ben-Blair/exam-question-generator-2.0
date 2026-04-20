#include "exam_question_generator.hpp"

#include <fstream>
#include <iostream>
#include <string>

namespace {

std::string derive_output_filename(const std::string& input_path) {
    std::size_t dot = input_path.rfind('.');
    if (dot != std::string::npos) {
        return input_path.substr(0, dot) + ".tex";
    }
    return input_path + ".tex";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: begin_exam_generator <input_file>\n";
        return 1;
    }

    const std::string input_path = argv[1];
    const std::string output_path = derive_output_filename(input_path);

    std::ifstream test_open(input_path);
    if (!test_open.is_open()) {
        std::cout << "Error: Could not read input file: " << input_path << "\n";
        return 1;
    }
    test_open.close();

    try {
        ExamQuestionGenerator generator;
        const std::string latex = generator.generate_all_from_file(input_path);

        std::ofstream output(output_path);
        if (!output.is_open()) {
            std::cerr << "Error: Could not open output file: " << output_path << "\n";
            return 1;
        }
        output << latex;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}

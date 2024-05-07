#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>
#include <memory>
#include <numeric>
#include <cstring>
#include <climits>
#include <cmath>
#include <unordered_map>

/*************
 * Setup code
 **************/

std::string part1(std::stringstream &file_content);
std::string part2(std::stringstream &file_content);

std::optional<std::stringstream> readFileContent(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file " << path << std::endl;
        return std::nullopt;
    }

    std::stringstream file_content;
    file_content << file.rdbuf();
    file.close();

    return std::move(file_content);
};

void executeFunction(std::stringstream &file_content, std::function<std::string(std::stringstream &)> function, std::string label, std::string expected)
{

    file_content.clear();
    file_content.seekg(0, std::ios::beg);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::string result = function(file_content);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if (result != expected)
    {
        std::cerr << "\033[1;31m" << label << " failed. Expected " << expected << " but got " << result << "\033[0m" << std::endl;
    }
    else
    {
        std::cout << "\033[1;32m" << label << " result: " << result << "\033[0m" << std::endl;
    }
    std::cerr << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <path> <expected_path>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::string expected_path = argv[2];
    std::optional<std::stringstream> file_content = readFileContent(path);
    std::optional<std::stringstream> file_expected_content = readFileContent(expected_path);

    if (!file_content)
    {
        std::cerr << "Could not read file " << path << std::endl;
        return 1;
    }

    if (!file_expected_content)
    {
        std::cerr << "Could not read file " << expected_path << std::endl;
        return 1;
    }

    std::stringstream &file_content_ref = *file_content;
    std::stringstream &file_expected_content_ref = *file_expected_content;

    std::string expected;

    file_expected_content_ref >> expected;
    executeFunction(file_content_ref, part1, "Part 1", expected);
    file_expected_content_ref >> expected;
    executeFunction(file_content_ref, part2, "Part 2", expected);

    return 0;
}

/***************************************
 *              Solutions
 **************************************/
using instruction_t = std::tuple<std::string, std::string, std::string>;

instruction_t stringToInstruction(std::string &str)
{
    std::replace(str.begin(), str.end(), ',', ' ');
    std::stringstream ss(str);
    std::string instr, arg1, arg2;

    ss >> instr >> arg1 >> arg2;
    return std::make_tuple(instr, arg1, arg2);
}

std::string processInstructions(std::stringstream& fileContent, int initialA)
{
    std::vector<instruction_t> instructions;
    std::string line;
    while (std::getline(fileContent, line))    
        instructions.push_back(stringToInstruction(line));

    std::unordered_map<std::string, int> register_set;
    register_set["a"] = initialA;
    for (auto instruction_pointer = cbegin(instructions); instruction_pointer < cend(instructions);)
    {
        const auto &[instr, arg1, arg2] = *instruction_pointer;

        if (instr == "hlf")
            register_set[arg1] /= 2;
        else if (instr == "tpl")
            register_set[arg1] *= 3;
        else if (instr == "inc")
            register_set[arg1]++;
        else if (instr == "jmp")
            instruction_pointer += std::stoi(arg1) - 1;
        else if (instr == "jie" && register_set[arg1] % 2 == 0)
            instruction_pointer += std::stoi(arg2) - 1;
        else if (instr == "jio" && register_set[arg1] == 1)
            instruction_pointer += std::stoi(arg2) - 1;

        instruction_pointer++;
    }
    return std::to_string(register_set["b"]);
}


std::string part1(std::stringstream& file_content)
{
    return processInstructions(file_content, 0);
}

std::string part2(std::stringstream& file_content)
{
    return processInstructions(file_content, 1);
}
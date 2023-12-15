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
#include <map>

/*************
 * Setup code  - Solution after line 97
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

unsigned char hashString(std::string &input)
{
    return std::accumulate(input.begin(), input.end(), 0, [](unsigned char hash, unsigned char c)
                           { return 17 * (hash + c); });
}

std::string part1(std::stringstream &file_content)
{
    int sum = 0;
    for (std::string step; std::getline(file_content, step, ',');)
        sum += hashString(step);
    return std::to_string(sum);
}

#define DELETION_OP '-'
#define INSERTION_OP '='

struct step_t
{
    std::string label;
    char op;
    int value;
};

struct box_t
{
    std::string label;
    int value;
};

step_t parseStep(std::string &step)
{
    std::string label;
    char op = 0;
    int value = 0;
    for (char c : step)
    {
        if (c == DELETION_OP || c == INSERTION_OP)
        {
            op = c;
            continue;
        }
        if (op)
            value = value * 10 + c - '0';
        else
            label += c;
    }
    return {label, op, value};
}

std::string part2(std::stringstream &file_content)
{

    std::map<int, std::vector<box_t>> registers;
    for (std::string step_line; std::getline(file_content, step_line, ',');)
    {

        step_t step = parseStep(step_line);

        if (step.op == DELETION_OP)
        {
            std::vector<box_t> &reg = registers[hashString(step.label)];
            std::vector<box_t>::iterator current_box_it = std::find_if(reg.begin(), reg.end(), [step](box_t &box)
                                                                       { return box.label == step.label; });
            bool current_box_exists = current_box_it != reg.end();
            if (current_box_exists)
                reg.erase(current_box_it);
        }
        else
        {
            std::vector<box_t> &reg = registers[hashString(step.label)];
            std::vector<box_t>::iterator current_box_it = std::find_if(reg.begin(), reg.end(), [step](box_t &box)
                                                                       { return box.label == step.label; });

            bool current_box_exists = current_box_it != reg.end();

            if (current_box_exists)
                current_box_it->value = step.value;
            else
                registers[hashString(step.label)].push_back({step.label, step.value});
        }
    }

    int sum_forces = 0;
    for (std::pair<const int, std::vector<box_t>> &reg : registers)
    {
        int slot = 0;
        int box = reg.first;
        std::vector<box_t> &content = reg.second;

        for (box_t &box_content : content)
            sum_forces += box_content.value * (box + 1) * (++slot);
    }

    return std::to_string(sum_forces);
}

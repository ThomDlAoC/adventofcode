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

struct dig_inst_t
{
    char dir;
    int steps;
    std::string color;
};

std::vector<dig_inst_t> parse(std::stringstream &file_content)
{
    std::vector<dig_inst_t> instructions;
    std::string line;
    while (std::getline(file_content, line))
    {
        dig_inst_t inst;
        std::stringstream ss(line);
        ss >> inst.dir;
        ss >> inst.steps;
        ss >> inst.color;
        inst.color = inst.color.substr(2, inst.color.size() - 3);
        instructions.push_back(inst);
    }
    return instructions;
}

std::string part1(std::stringstream &file_content)
{

    auto instructions = parse(file_content);
    std::array<int, 2> pos = {0, 0};
    std::array<int, 2> prev_pos = {0, 0};

    int perimeter = 0;
    int sum = 0;
    for (const auto &inst : instructions)
    {
        switch (inst.dir)
        {
        case 'R':
            pos[0] += inst.steps;
            break;
        case 'D':
            pos[1] += inst.steps;
            break;
        case 'L':
            pos[0] -= inst.steps;
            break;
        case 'U':
            pos[1] -= inst.steps;
            break;
        }
        perimeter += inst.steps;
        sum += prev_pos[0] * pos[1] - prev_pos[1] * pos[0];
        prev_pos = pos;
    }

    return std::to_string(std::abs(sum + perimeter) / 2 + 1);
}

std::string part2(std::stringstream &file_content)
{

    auto instructions = parse(file_content);
    std::array<long long, 2> pos = {0, 0};
    std::array<long long, 2> prev_pos = {0, 0};

    long long perimeter = 0;
    long long sum = 0;
    for (const auto &inst : instructions)
    {
        char dir = "RDLU"[inst.color[inst.color.size() - 1] % 4];
        int steps = std::stoi(inst.color.substr(0, inst.color.size() - 1), nullptr, 16);
        switch (dir)
        {
        case 'R':
            pos[0] += steps;
            break;
        case 'D':
            pos[1] += steps;
            break;
        case 'L':
            pos[0] -= steps;
            break;
        case 'U':
            pos[1] -= steps;
            break;
        }
        perimeter += steps;
        sum += prev_pos[0] * pos[1] - prev_pos[1] * pos[0];
        prev_pos = pos;
    }

    return std::to_string(std::abs(sum + perimeter) / 2 + 1);
}

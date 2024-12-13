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
struct location
{
    int64_t X;
    int64_t Y;
};

using machine = std::tuple<location, location, location>;

std::vector<machine> parseMachines(std::stringstream &file_content)
{

    std::vector<machine> machines;
    std::string line;
    int64_t num{}, X{}, Y{};
    std::vector<location> locations;
    while (std::getline(file_content, line))
    {
        if (line.empty())
        {
            machines.push_back({locations[0], locations[1], locations[2]});
            locations.clear();
            continue;
        }
        for (const char c : line)
        {
            if (isdigit(c))
            {
                num = num * 10 + (c - '0');
            }
            else if (c == ',')
            {
                X = num;
                num = 0;
            }
        }
        Y = num;
        num = 0;
        locations.push_back({X, Y});
    }
    machines.push_back({locations[0], locations[1], locations[2]});

    return machines;
}

uint64_t totalTokenToWin(const machine &machine)
{

    const auto &[A, B, Prize] = machine;

    int64_t det = A.X * B.Y - A.Y * B.X;

    if (det == 0)
    {
        return 0;
    }

    int64_t K = (Prize.X * B.Y - Prize.Y * B.X) / det;
    int64_t J = (A.X * Prize.Y - A.Y * Prize.X) / det;

    if (A.X * K + B.X * J == Prize.X && A.Y * K + B.Y * J == Prize.Y)
    {
        return 3 * K + J;
    }

    return 0;
}

std::string part1(std::stringstream &file_content)
{

    auto machines = parseMachines(file_content);
    uint64_t total_tokens = 0;
    for (const auto &machine : machines)
    {

        total_tokens += totalTokenToWin(machine);
    }

    return std::to_string(total_tokens);
}

std::string part2(std::stringstream &file_content)
{

    auto machines = parseMachines(file_content);
    uint64_t total_tokens = 0;
    for (auto &machine : machines)
    {

        auto &[A, B, Prize] = machine;
        Prize.X += 10000000000000;
        Prize.Y += 10000000000000;

        total_tokens += totalTokenToWin(machine);
    }

    return std::to_string(total_tokens);
}

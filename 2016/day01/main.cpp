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
#include <unordered_set>

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

std::string part1(std::stringstream &file_content)
{

    int pos_x = 0, pos_y = 0;
    int dir_x = 0, dir_y = 1;
    while (!file_content.eof())
    {
        char dir, dummy;
        int steps;
        file_content >> dir >> steps >> dummy;
        if (dir == 'R')
            dir_x = std::exchange(dir_y, -dir_x);
        else
            dir_y = std::exchange(dir_x, -dir_y);

        pos_x += dir_x * steps;
        pos_y += dir_y * steps;
    }

    return std::to_string(std::abs(pos_x) + std::abs(pos_y));
}

std::string part2(std::stringstream &file_content)
{

    auto cmp_points = [](const std::pair<int, int> &a, const std::pair<int, int> &b)
    {
        return a.first == b.first && a.second == b.second;
    };
    auto hash_points = [](const std::pair<int, int> &a)
    {
        return std::hash<int>()(a.first) ^ std::hash<int>()(a.second);
    };
    std::unordered_set<std::pair<int, int>, decltype(hash_points), decltype(cmp_points)> visited(0, hash_points, cmp_points);

    int pos_x = 0, pos_y = 0;
    int dir_x = 0, dir_y = 1;

    visited.insert({0, 0});

    while (!file_content.eof())
    {
        char dir, dummy;
        int steps;
        file_content >> dir >> steps >> dummy;
        if (dir == 'R')
            dir_x = std::exchange(dir_y, -dir_x);
        else
            dir_y = std::exchange(dir_x, -dir_y);

        for (; steps--;)
        {
            pos_x += dir_x;
            pos_y += dir_y;

            if (visited.contains({pos_x, pos_y}))
                return std::to_string(std::abs(pos_x) + std::abs(pos_y));

            visited.insert({pos_x, pos_y});
        }
    }

    return "oops, something went wrong!";
}

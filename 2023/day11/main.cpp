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
std::vector<std::string> parseInput(std::stringstream &file_content)
{
    std::vector<std::string> input;
    std::string line;
    while (std::getline(file_content, line))
    {
        input.push_back(line);
    }
    return input;
}

std::string part1(std::stringstream &file_content)
{

    auto input_map = parseInput(file_content);

    int width = input_map[0].size();
    int height = input_map.size();
    std::vector<bool> empty_rows(height, true);
    std::vector<bool> empty_cols(width, true);

    for (int i = 0; i < height; i++)
    {
        bool empty = true;
        for (int j = 0; j < width; j++)
        {
            if (input_map[i][j] == '#')
            {
                empty = false;
                break;
            }
        }
        empty_rows[i] = empty;
    }

    for (int i = 0; i < width; i++)
    {
        bool empty = true;
        for (int j = 0; j < height; j++)
        {
            if (input_map[j][i] == '#')
            {
                empty = false;
                break;
            }
        }
        empty_cols[i] = empty;
    }

    std::vector<std::pair<int, int>> galaxies;

    int offset_x = 0;
    int offset_y = 0;
    for (int i = 0; i < height; ++i)
    {
        if (empty_rows[i])
        {
            offset_y++;
            continue;
        }
        offset_x = 0;
        for (int j = 0; j < width; ++j)
        {
            if (empty_cols[j])
            {
                offset_x++;
                continue;
            }
            if (input_map[i][j] == '#')
            {
                galaxies.push_back({i + offset_y, j + offset_x});
            }
        }
    }

    int sum_distances = 0;
    for (size_t i = 0; i < galaxies.size(); ++i)
    {
        for (size_t j = i + 1; j < galaxies.size(); ++j)
            sum_distances += std::abs(galaxies[i].first - galaxies[j].first) + std::abs(galaxies[i].second - galaxies[j].second);
    }

    return std::to_string(sum_distances);
}

std::string part2(std::stringstream &file_content)
{

    auto input_map = parseInput(file_content);

    int width = input_map[0].size();
    int height = input_map.size();
    std::vector<bool> empty_rows(height, true);
    std::vector<bool> empty_cols(width, true);

    for (int i = 0; i < height; i++)
    {
        bool empty = true;
        for (int j = 0; j < width; j++)
        {
            if (input_map[i][j] == '#')
            {
                empty = false;
                break;
            }
        }
        empty_rows[i] = empty;
    }

    for (int i = 0; i < width; i++)
    {
        bool empty = true;
        for (int j = 0; j < height; j++)
        {
            if (input_map[j][i] == '#')
            {
                empty = false;
                break;
            }
        }
        empty_cols[i] = empty;
    }

    std::vector<std::pair<long long, long long>> galaxies;

    long long offset_x = 0;
    long long offset_y = 0;
    for (int i = 0; i < height; ++i)
    {
        if (empty_rows[i])
        {
            offset_y += 999999;
            continue;
        }
        offset_x = 0;
        for (int j = 0; j < width; ++j)
        {
            if (empty_cols[j])
            {
                offset_x += 999999;
                continue;
            }
            if (input_map[i][j] == '#')
            {
                galaxies.push_back({(long long)i + offset_y, (long long)j + offset_x});
            }
        }
    }

    unsigned long long sum_distances = 0;
    for (size_t i = 0; i < galaxies.size(); ++i)
    {
        for (size_t j = i + 1; j < galaxies.size(); ++j)
            sum_distances += std::abs(galaxies[i].first - galaxies[j].first) + std::abs(galaxies[i].second - galaxies[j].second);
    }

    return std::to_string(sum_distances);
}

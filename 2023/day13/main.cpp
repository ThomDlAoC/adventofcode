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

std::vector<std::pair<std::vector<int>, std::vector<int>>> parseInput(std::stringstream &file_content)
{
    std::string line;
    std::vector<std::string> lines;

    std::vector<std::pair<std::vector<int>, std::vector<int>>> tile_values;

    while (std::getline(file_content, line))
    {
        if (line.empty())
        {
            int width = lines[0].size();
            int height = lines.size();

            std::vector<int> row_values(height, 0);
            std::vector<int> col_values(width, 0);

            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    row_values[i] |= (lines[i][j] == '#' ? 1 : 0) << j;
                    col_values[j] |= (lines[i][j] == '#' ? 1 : 0) << i;
                }
            }

            tile_values.push_back(std::make_pair(row_values, col_values));

            lines.clear();
            continue;
        }

        lines.push_back(line);
    }

    return tile_values;
}

int evalSymmetryIndex(std::vector<int> &values, int max_fixes)
{
    for (int i = 0, arr_size = values.size(); i < arr_size - 1; i++)
    {
        int match = false;
        int xored_fix = values[i] ^ values[i + 1];
        int smudge_fixed = xored_fix && (xored_fix & (xored_fix - 1)) == 0;
        if (values[i] == values[i + 1] || smudge_fixed)
        {
            match = true;
            for (int h = i - 1, k = i + 2; h >= 0 && k < arr_size && match; h--, k++)
            {
                int xored_fix = values[h] ^ values[k];
                int can_fix = xored_fix && (xored_fix & (xored_fix - 1)) == 0;
                smudge_fixed += can_fix;
                if (values[h] != values[k] && !can_fix)
                    match = false;
            }
        }
        if (match && smudge_fixed == max_fixes)
            return i + 1;
    }

    return 0;
}

std::string part1(std::stringstream &file_content)
{
    auto input_values = parseInput(file_content);
    int sum = std::accumulate(input_values.begin(), input_values.end(), 0, [](int acc, auto &tile)
                              { return acc + 100 * evalSymmetryIndex(tile.first, 0) + evalSymmetryIndex(tile.second, 0); });
    return std::to_string(sum);
}

std::string part2(std::stringstream &file_content)
{
    auto input_values = parseInput(file_content);
    int sum = std::accumulate(input_values.begin(), input_values.end(), 0, [](int acc, auto &tile)
                              { return acc + 100 * evalSymmetryIndex(tile.first, 1) + evalSymmetryIndex(tile.second, 1); });
    return std::to_string(sum);
}
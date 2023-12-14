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

void rollNorth(std::vector<std::string> &input)
{
    int rows = input.size();
    int cols = input[0].size();
    std::vector<int> cursors(cols, 0);
    for (int col = 0; col < cols; col++)
    {
        for (int row = 0; row < rows; row++)
        {
            auto &c = cursors[col];
            if (input[row][col] == '#')
            {
                c = row + 1;
            }
            else if (input[row][col] == 'O')
            {
                std::swap(input[row][col], input[c][col]);
                c++;
            }
        }
    }
}

void rollSouth(std::vector<std::string> &input)
{
    int rows = input.size();
    int cols = input[0].size();
    std::vector<int> cursors(cols, rows - 1);
    for (int col = 0; col < cols; col++)
    {
        for (int row = rows - 1; row >= 0; row--)
        {
            auto &c = cursors[col];
            if (input[row][col] == '#')
            {
                c = row - 1;
            }
            else if (input[row][col] == 'O')
            {
                std::swap(input[row][col], input[c][col]);
                c--;
            }
        }
    }
}

void rollWest(std::vector<std::string> &input)
{
    int rows = input.size();
    int cols = input[0].size();
    std::vector<int> cursors(rows, 0);
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            auto &c = cursors[row];
            if (input[row][col] == '#')
            {
                c = col + 1;
            }
            else if (input[row][col] == 'O')
            {
                std::swap(input[row][col], input[row][c]);
                c++;
            }
        }
    }
}

void rollEast(std::vector<std::string> &input)
{
    int rows = input.size();
    int cols = input[0].size();
    std::vector<int> cursors(rows, cols - 1);
    for (int row = 0; row < rows; row++)
    {
        for (int col = cols - 1; col >= 0; col--)
        {
            auto &c = cursors[row];
            if (input[row][col] == '#')
            {
                c = col - 1;
            }
            else if (input[row][col] == 'O')
            {
                std::swap(input[row][col], input[row][c]);
                c--;
            }
        }
    }
}

int calculateLoad(std::vector<std::string> &input)
{
    int rows = input.size();
    int cols = input[0].size();
    int sum = 0;
    for (int col = 0; col < cols; col++)
        for (int row = 0; row < rows; row++)
            if (input[row][col] == 'O')
                sum += rows - row;

    return sum;
}

std::string part1(std::stringstream &file_content)
{
    std::vector<std::string> input = parseInput(file_content);
    rollNorth(input);
    return std::to_string(calculateLoad(input));
}

std::string part2(std::stringstream &file_content)
{
    std::vector<std::string> input = parseInput(file_content);
    std::unordered_map<std::string, int> occurences;
    for (int i = 0; i < 10000000; ++i)
    {
        rollNorth(input);
        rollWest(input);
        rollSouth(input);
        rollEast(input);

        std::string key = std::accumulate(input.begin(), input.end(), std::string());
        if (occurences[key] && ((1000000000 - 1 - occurences[key]) % (i - occurences[key]) == 0))
            return std::to_string(calculateLoad(input));
        occurences[key] = i;
    }

    return "";
}

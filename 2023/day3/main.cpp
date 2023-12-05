#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <numeric>

/*************
 * Setup code
 **************/

void part1(std::stringstream &file_content);
void part2(std::stringstream &file_content);

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

void executeFunction(std::stringstream &file_content, std::function<void(std::stringstream &)> function)
{
    file_content.clear();
    file_content.seekg(0, std::ios::beg);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    function(file_content);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cerr << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::optional<std::stringstream> file_content = readFileContent(path);

    if (!file_content)
    {
        std::cerr << "Could not read file " << path << std::endl;
        return 1;
    }

    std::stringstream &file_content_ref = *file_content;

    executeFunction(file_content_ref, part1);
    executeFunction(file_content_ref, part2);

    return 0;
}

/***************************************
 *              Solutions
 ************************************/

inline bool indexOutOfBounds(const int row,const int col,const int rows,const int cols)
{
    return row < 0 || row >= rows || col < 0 || col >= cols;
}

bool hasAdjacentSymbol(const std::vector<std::string> &lines, int i, int j)
{

    int rows = lines.size();
    int cols = lines[0].size();

    for (int h = -1; h <= 1; ++h)
    {
        for (int l = -1; l <= 1; ++l)
        {
            int row = i + h;
            int col = j + l;
            if (indexOutOfBounds(row, col, rows, cols))
                continue;
            char current_char = lines[row][col];
            if (std::isdigit(current_char) || current_char == '.')
                continue;
            return true;
        }
    }

    return false;
}

std::vector<int> idxAdjacentStars(const std::vector<std::string> &lines, int i, int j)
{

    int rows = lines.size();
    int cols = lines[0].size();
    std::vector<int> v;

    for (int h = -1; h <= 1; ++h)
    {
        for (int l = -1; l <= 1; ++l)
        {
            int row = i + h;
            int col = j + l;
            if (indexOutOfBounds(row, col, rows, cols))
                continue;
            char current_char = lines[row][col];
            if (current_char == '*')
                v.push_back(row * cols + col);
        }
    }

    return v;
}

void part1(std::stringstream &file_content)
{

    std::vector<std::string> lines;

    for (std::string line; getline(file_content, line);)
        lines.push_back(line + ".");

    int rows = lines.size();
    int cols = lines[0].size();
    int sum = 0;
    int current_parsed_number = 0;
    int has_symbol_around = 0;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            char current_char = lines[row][col];

            if (std::isdigit(current_char))
            {
                current_parsed_number = current_parsed_number * 10 + (current_char - '0');
                has_symbol_around |= hasAdjacentSymbol(lines, row, col);
            }
            else
            {
                if (has_symbol_around)
                {
                    sum += current_parsed_number;
                }

                current_parsed_number = 0;
                has_symbol_around = 0;
            }
        }
    }

    std::cerr << "Part 1: " << sum << std::endl;
}

void part2(std::stringstream &file_content)
{
    std::vector<std::string> lines;

    for (std::string line; getline(file_content, line);)
        lines.push_back(line + ".");

    int rows = lines.size();
    int cols = lines[0].size();
    std::unordered_map<int, std::vector<int>> numbers_adjacent_to_star;
    int current_parsed_number = 0;
    std::unordered_set<int> stars_adjacent_to_number;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            char current_char = lines[row][col];

            if (std::isdigit(current_char))
            {
                current_parsed_number = current_parsed_number * 10 + (current_char - '0');
                std::vector<int> stars = idxAdjacentStars(lines, row, col);
                stars_adjacent_to_number.insert(stars.begin(), stars.end());
            }
            else
            {
                if (stars_adjacent_to_number.size() > 0)
                {
                    for (int star : stars_adjacent_to_number)
                        numbers_adjacent_to_star[star].push_back(current_parsed_number);
                }

                current_parsed_number = 0;
                stars_adjacent_to_number.clear();
            }
        }
    }

    int totalsum = std::accumulate(numbers_adjacent_to_star.begin(), numbers_adjacent_to_star.end(), 0, [](unsigned long long sum, std::pair<int, std::vector<int>> pair)
                                   { return sum + (pair.second.size() != 2 ? 0 : pair.second[0] * pair.second[1]); });

    std::cerr << "Part 2: " << totalsum << std::endl;
}
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
#include <unordered_map>
#include <queue>
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

namespace Direction
{
    constexpr int Nort = 0;
    constexpr int East = 1;
    constexpr int Sout = 2;
    constexpr int West = 3;
    constexpr int None = 4;
}

namespace Turn
{
    constexpr int Left = 0;
    constexpr int Righ = 1;
    constexpr int None = 2;
}

const std::array<std::array<int, 2>, 5> dirs = {{{-1, 0},
                                                 {0, 1},
                                                 {1, 0},
                                                 {0, -1},
                                                 {0, 0}}};

const std::array<std::array<int, 2>, 2> turns = {{{-1, 1},
                                                  {1, -1}}};

using next_direction_t = std::array<int, 4>;
using next_rotation_t = std::array<int, 4>;
using direction_t = int;
using position_t = std::array<int, 2>;
using steps_t = int;

std::unordered_map<char, next_direction_t> next_directions_map = {
    //      NORTH,              EAST,             SOUTH,            WEST
    {'|', {Direction::Nort, Direction::None, Direction::Sout, Direction::None}},
    {'-', {Direction::None, Direction::East, Direction::None, Direction::West}},
    {'L', {Direction::None, Direction::None, Direction::East, Direction::Nort}},
    {'J', {Direction::None, Direction::Nort, Direction::West, Direction::None}},
    {'7', {Direction::West, Direction::Sout, Direction::None, Direction::None}},
    {'F', {Direction::East, Direction::None, Direction::None, Direction::Sout}},
    {'.', {Direction::None, Direction::None, Direction::None, Direction::None}},
    {'S', {Direction::None, Direction::None, Direction::None, Direction::None}},

};

std::unordered_map<char, next_rotation_t> next_rotations_map = {
    //      NORTH,              EAST,             SOUTH,            WEST
    {'|', {Turn::None, Turn::None, Turn::None, Turn::None}},
    {'-', {Turn::None, Turn::None, Turn::None, Turn::None}},
    {'L', {Turn::None, Turn::None, Turn::Left, Turn::Righ}},
    {'J', {Turn::None, Turn::Left, Turn::Righ, Turn::None}},
    {'7', {Turn::Left, Turn::Righ, Turn::None, Turn::None}},
    {'F', {Turn::Righ, Turn::None, Turn::None, Turn::Left}},
    {'.', {Turn::None, Turn::None, Turn::None, Turn::None}},
    {'S', {Turn::None, Turn::None, Turn::None, Turn::None}},

};

std::unordered_map<char, std::string> direction_to_unicode = {
    {'|', "│"},
    {'-', "─"},
    {'L', "└"},
    {'J', "┘"},
    {'7', "┐"},
    {'F', "┌"},
    {'.', "·"},
    {'S', "S"},
};

std::vector<std::string> parseInput(std::stringstream &file_content)
{
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file_content, line))
    {
        lines.push_back("." + line + ".");
    }
    std::string top_bottom = std::string(lines[0].size(), '.');
    lines.insert(lines.begin(), top_bottom);
    lines.push_back(top_bottom);
    return lines;
}

std::string part1(std::stringstream &file_content)
{
    auto input_map = parseInput(file_content);

    int width = input_map[0].size();
    int height = input_map.size();

    std::vector<std::vector<bool>> visited_map(height, std::vector<bool>(width, false));

    std::array<int, 2> pos = {0, 0};
    bool start_found = false;

    for (size_t row = 0; row < input_map.size(); row++)
    {
        for (size_t col = 0; col < input_map[row].size(); col++)
        {
            if (input_map[row][col] == 'S')
            {
                pos = {static_cast<int>(row), static_cast<int>(col)};
                start_found = true;
                break;
            }
        }
        if (start_found)
        {
            break;
        }
    }

    std::queue<std::tuple<position_t, direction_t, steps_t>> queue;
    auto visited_hash = [](const position_t &pos)
    {
        return pos[0] * 1000 + pos[1];
    };
    auto visited_equal = [](const position_t &pos1, const position_t &pos2)
    {
        return pos1[0] == pos2[0] && pos1[1] == pos2[1];
    };
    std::unordered_set<position_t, decltype(visited_hash), decltype(visited_equal)> visited(1000, visited_hash, visited_equal);
    visited.insert(pos);
    visited_map[pos[0]][pos[1]] = true;

    for (size_t i = 0; i < 4; i++)
    {
        position_t next_pos = {pos[0] + dirs[i][0], pos[1] + dirs[i][1]};
        if (next_pos[0] < 0 || next_pos[0] >= (int)input_map.size() || next_pos[1] < 0 || next_pos[1] >= (int)input_map[next_pos[0]].size())
        {
            continue;
        }
        if (next_directions_map[input_map[next_pos[0]][next_pos[1]]][i] != Direction::None)
        {
            queue.push({pos, i, 0});
        }
        visited.insert(pos);
    }
    int max_steps = 0;
    while (!queue.empty())
    {
        auto [pos, dir, steps] = queue.front();
        queue.pop();
        max_steps = std::max(max_steps, steps);
        position_t next_pos = {pos[0] + dirs[dir][0], pos[1] + dirs[dir][1]};

        if (visited.find(next_pos) != visited.end())
            continue;

        if (next_pos[0] < 0 || next_pos[0] >= (int)input_map.size() || next_pos[1] < 0 || next_pos[1] >= (int)input_map[0].size())
            continue;

        if (next_directions_map[input_map[next_pos[0]][next_pos[1]]][dir] == Direction::None)
            continue;

        queue.push({next_pos, next_directions_map[input_map[next_pos[0]][next_pos[1]]][dir], steps + 1});
        visited.insert(next_pos);
        visited_map[next_pos[0]][next_pos[1]] = true;
    }

    return std::to_string(max_steps);
}

std::string part2(std::stringstream &file_content)
{

    auto input_map = parseInput(file_content);

    int width = input_map[0].size();
    int height = input_map.size();

    std::vector<std::vector<bool>> visited_map(height, std::vector<bool>(width, false));

    std::vector<std::vector<int>> inout_map(height, std::vector<int>(width, 0));

    std::array<int, 2> pos = {0, 0};
    bool start_found = false;

    for (size_t row = 0; row < input_map.size(); row++)
    {
        for (size_t col = 0; col < input_map[row].size(); col++)
        {
            if (input_map[row][col] == 'S')
            {
                pos = {static_cast<int>(row), static_cast<int>(col)};
                start_found = true;
                break;
            }
        }
        if (start_found)
        {
            break;
        }
    }

    std::queue<std::tuple<position_t, direction_t, steps_t, position_t, position_t>> queue;
    auto visited_hash = [](const position_t &pos)
    {
        return pos[0] * 1000 + pos[1];
    };
    auto visited_equal = [](const position_t &pos1, const position_t &pos2)
    {
        return pos1[0] == pos2[0] && pos1[1] == pos2[1];
    };
    std::unordered_set<position_t, decltype(visited_hash), decltype(visited_equal)> visited(1000, visited_hash, visited_equal);
    visited.insert(pos);
    visited_map[pos[0]][pos[1]] = true;

    for (size_t i = 0; i < 4; i++)
    {
        position_t next_pos = {pos[0] + dirs[i][0], pos[1] + dirs[i][1]};
        if (next_directions_map[input_map[next_pos[0]][next_pos[1]]][i] != Direction::None)
        {
            position_t vector_left = {dirs[i][1] * turns[Turn::Left][0], dirs[i][0] * turns[Turn::Left][1]};
            position_t vector_right = {dirs[i][1] * turns[Turn::Righ][0], dirs[i][0] * turns[Turn::Righ][1]};
            queue.push({pos, i, 0, vector_left, vector_right});
            break;
        }
    }

    while (!queue.empty())
    {
        auto [pos, dir, steps, vector_left, vector_right] = queue.front();
        queue.pop();
        inout_map[pos[0] + vector_left[0]][pos[1] + vector_left[1]]++;
        inout_map[pos[0] + vector_right[0]][pos[1] + vector_right[1]]--;
        position_t next_pos = {pos[0] + dirs[dir][0], pos[1] + dirs[dir][1]};

        if (visited.find(next_pos) != visited.end())
            continue;

        if (next_directions_map[input_map[next_pos[0]][next_pos[1]]][dir] == Direction::None)
            continue;

        int next_rotation = next_rotations_map[input_map[next_pos[0]][next_pos[1]]][dir];
        if (next_rotation != Turn::None)
        {
            position_t next_vector_left = {vector_left[1] * turns[next_rotation][0], vector_left[0] * turns[next_rotation][1]};
            position_t next_vector_right = {vector_right[1] * turns[next_rotation][0], vector_right[0] * turns[next_rotation][1]};

            inout_map[next_pos[0] + vector_left[0]][pos[1] + vector_left[1]]++;
            inout_map[next_pos[0] + vector_right[0]][pos[1] + vector_right[1]]--;
            queue.push({next_pos, next_directions_map[input_map[next_pos[0]][next_pos[1]]][dir], steps + 1, next_vector_left, next_vector_right});
        }
        else
        {
            queue.push({next_pos, next_directions_map[input_map[next_pos[0]][next_pos[1]]][dir], steps + 1, vector_left, vector_right});
        }
        visited.insert(next_pos);
        visited_map[next_pos[0]][next_pos[1]] = true;
    }

    int totalin = 0;
    for (size_t row = 1; row < input_map.size() - 1; row++)
    {
        bool is_in = false;
        for (size_t col = 1; col < input_map[row].size() - 1; col++)
        {
            if (visited_map[row][col])
            {
                is_in = false;
                continue;
            }
            if (inout_map[row][col] < 0)
                is_in = true;
            if (is_in)
                totalin++, inout_map[row][col] = -1;
        }
    }

    for (size_t row = 0; row < input_map.size(); row++)
    {
        for (size_t col = 0; col < input_map[row].size(); col++)
        {
            if (visited_map[row][col])
            {
                std::cout << direction_to_unicode[input_map[row][col]];
            }
            else
            {
                if (inout_map[row][col] < 0)
                {
                    std::cout << 'X';
                }
                else
                    std::cout << " ";
            }
        }
        std::cout << std::endl;
    }

    return std::to_string(totalin);
}

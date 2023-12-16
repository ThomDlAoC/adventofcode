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

#define D_RIGHT 0
#define D_DOWN 1
#define D_LEFT 2
#define D_UP 3

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

std::vector<std::array<int, 2>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

int getStateKey(std::tuple<int, int, int> state)
{
    return std::get<0>(state) + std::get<1>(state) * 1000 + std::get<2>(state) * 1000000;
}

void insertNewPosition(std::vector<std::string> &input_map, int current_x, int current_y, int direction, std::queue<std::tuple<int, int, int>> &positions, std::unordered_set<int> &visited)
{
    int new_direction = direction;
    int ny = current_y + directions[new_direction][0];
    int nx = current_x + directions[new_direction][1];
    if (nx < 0 || ny < 0 || ny >= (int)input_map.size() || nx >= (int)input_map[ny].size())
        return;
    if (visited.find(getStateKey({ny, nx, new_direction})) != visited.end())
        return;

    positions.push({ny, nx, new_direction});
    visited.insert(getStateKey({ny, nx, new_direction}));
}

int getEnergy(std::vector<std::string> &input_map, int y, int x, int direction)
{
    std::tuple<int, int, int> position_direction = {y, x, direction};
    std::queue<std::tuple<int, int, int>> positions;
    std::unordered_set<int> visited_state;
    std::unordered_set<int> visited_pos;
    positions.push(position_direction);
    visited_state.insert(getStateKey(position_direction));

    int total_energy = 0;
    while (!positions.empty())
    {
        auto [y, x, direction] = positions.front();
        positions.pop();
        if (visited_pos.find(getStateKey({y, x, 0})) == visited_pos.end())
            total_energy++;
        visited_pos.insert(getStateKey({y, x, 0}));

        if (input_map[y][x] == '.' || (input_map[y][x] == '|' && (direction % 2 == 1)) || (input_map[y][x] == '-' && (direction % 2 == 0)))
        {
            insertNewPosition(input_map, x, y, direction, positions, visited_state);
        }
        else if (input_map[y][x] == '|')
        {

            insertNewPosition(input_map, x, y, 1, positions, visited_state);
            insertNewPosition(input_map, x, y, 3, positions, visited_state);
        }
        else if (input_map[y][x] == '-')
        {

            insertNewPosition(input_map, x, y, 0, positions, visited_state);
            insertNewPosition(input_map, x, y, 2, positions, visited_state);
        }
        else if (input_map[y][x] == '\\')
        {
            int new_direction = direction % 2 == 0 ? (4 + direction + 1) % 4 : (4 + direction - 1) % 4;
            insertNewPosition(input_map, x, y, new_direction, positions, visited_state);
        }
        else if (input_map[y][x] == '/')
        {
            int new_direction = direction % 2 == 0 ? (4 + direction - 1) % 4 : (4 + direction + 1) % 4;
            insertNewPosition(input_map, x, y, new_direction, positions, visited_state);
        }
    }

    return total_energy;
}

std::string part1(std::stringstream &file_content)
{
    auto input_map = parseInput(file_content);
    return std::to_string(getEnergy(input_map, 0, 0, D_RIGHT));
}

std::string part2(std::stringstream &file_content)
{
    auto input_map = parseInput(file_content);
    int max_energy = 0;
    for (int col = 0; col < (int)input_map[0].size(); col++)
    {

        if (col == 0)
        {
            max_energy = std::max(getEnergy(input_map, 0, 0, D_DOWN), max_energy);
            max_energy = std::max(getEnergy(input_map, 0, 0, D_RIGHT), max_energy);

            max_energy = std::max(getEnergy(input_map, (int)input_map.size() - 1, 0, D_RIGHT), max_energy);
            max_energy = std::max(getEnergy(input_map, (int)input_map.size() - 1, 0, D_UP), max_energy);
        }
        else if (col == (int)input_map[0].size() - 1)
        {
            max_energy = std::max(getEnergy(input_map, 0, col, D_DOWN), max_energy);
            max_energy = std::max(getEnergy(input_map, 0, col, D_LEFT), max_energy);

            max_energy = std::max(getEnergy(input_map, (int)input_map.size() - 1, col, D_LEFT), max_energy);
            max_energy = std::max(getEnergy(input_map, (int)input_map.size() - 1, col, D_UP), max_energy);
        }
        else
        {
            max_energy = std::max(getEnergy(input_map, 0, col, D_DOWN), max_energy);
            max_energy = std::max(getEnergy(input_map, (int)input_map.size() - 1, col, D_UP), max_energy);
        }
    }

    for (int row = 0; row < (int)input_map.size(); row++)
    {
        if (row == 0)
        {
            max_energy = std::max(getEnergy(input_map, row, 0, D_RIGHT), max_energy);
            max_energy = std::max(getEnergy(input_map, row, 0, D_DOWN), max_energy);

            max_energy = std::max(getEnergy(input_map, row, (int)input_map[0].size() - 1, D_DOWN), max_energy);
            max_energy = std::max(getEnergy(input_map, row, (int)input_map[0].size() - 1, D_LEFT), max_energy);
        }
        else if (row == (int)input_map.size() - 1)
        {
            max_energy = std::max(getEnergy(input_map, row, 0, D_RIGHT), max_energy);
            max_energy = std::max(getEnergy(input_map, row, 0, D_UP), max_energy);

            max_energy = std::max(getEnergy(input_map, row, (int)input_map[0].size() - 1, D_LEFT), max_energy);
            max_energy = std::max(getEnergy(input_map, row, (int)input_map[0].size() - 1, D_UP), max_energy);
        }
        else
        {
            max_energy = std::max(getEnergy(input_map, row, 0, D_RIGHT), max_energy);
            max_energy = std::max(getEnergy(input_map, row, (int)input_map[0].size() - 1, D_LEFT), max_energy);
        }
    }

    return std::to_string(max_energy);
}

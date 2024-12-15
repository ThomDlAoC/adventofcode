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

#define MOVE_UP 0
#define MOVE_DOWN 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3

std::pair<std::vector<std::string>, std::vector<int>> parseInput(std::stringstream &file_content)
{
    std::vector<std::string> map;
    for (std::string line; std::getline(file_content, line);)
    {
        if (line.empty())
        {
            break;
        }
        map.push_back(line);
    }

    std::vector<int> moves;
    for (std::string line; std::getline(file_content, line);)
    {
        for (char c : line)
        {
            switch (c)
            {
            case '^':
                moves.push_back(MOVE_UP);
                break;
            case 'v':
                moves.push_back(MOVE_DOWN);
                break;
            case '<':
                moves.push_back(MOVE_LEFT);
                break;
            case '>':
                moves.push_back(MOVE_RIGHT);
                break;
            default:
                break;
            }
        }
    }

    return std::make_pair(map, moves);
}

std::pair<int, int> findLocation(const std::vector<std::string> &map)
{

    int width = map[0].size();
    int height = map.size();

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (map[i][j] == '@')
            {
                return std::make_pair(i, j);
            }
        }
    }

    return std::make_pair(-1, -1);
}

std::string part1(std::stringstream &file_content)
{

    const std::array<std::pair<int, int>, 4> directions = {
        std::make_pair(-1, 0),
        std::make_pair(1, 0),
        std::make_pair(0, -1),
        std::make_pair(0, 1)};

    auto [map, moves] = parseInput(file_content);

    auto robot_loc = findLocation(map);

    int width = map[0].size();
    int height = map.size();

    for (const auto m : moves)
    {

        const auto &[dy, dx] = directions[m];
        bool can_move{false};
        int ny = robot_loc.first + dy, nx = robot_loc.second + dx;

        for (; ny >= 0 && ny < height && nx >= 0 && nx < width; ny += dy, nx += dx)
        {
            if (map[ny][nx] == '#')
            {
                break;
            }
            if (map[ny][nx] == '.')
            {
                can_move = true;
                break;
            }
        }
        if (!can_move)
        {
            continue;
        }
        int rdy = -dy, rdx = -dx;

        for (int y = ny, x = nx; y != robot_loc.first || x != robot_loc.second; y += rdy, x += rdx)
        {
            std::swap(map[y][x], map[y + rdy][x + rdx]);
        }

        robot_loc.first += dy;
        robot_loc.second += dx;
    }

    int result{};
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (map[i][j] == 'O')
            {
                result += 100 * i + j;
            }
        }
    }

    return std::to_string(result);
}

std::vector<std::string> dilateMap(const std::vector<std::string> &map)
{

    std::vector<std::string> new_map;
    for (const auto &line : map)
    {
        std::string new_line;
        for (char c : line)
        {
            switch (c)
            {
            case '#':
                new_line.push_back('#');
                new_line.push_back('#');
                break;
            case 'O':
                new_line.push_back('[');
                new_line.push_back(']');
                break;
            case '.':
                new_line.push_back('.');
                new_line.push_back('.');
                break;
            case '@':
                new_line.push_back('@');
                new_line.push_back('.');
                break;
            default:
                break;
            }
        }
        new_map.push_back(new_line);
    }

    return new_map;
}

void printMap(const std::vector<std::string> &map)
{
    for (const auto &line : map)
    {
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
}

std::string part2(std::stringstream &file_content)
{

    [[maybe_unused]]
    const std::array<std::pair<int, int>, 4> directions = {
        std::make_pair(-1, 0),
        std::make_pair(1, 0),
        std::make_pair(0, -1),
        std::make_pair(0, 1)};

    auto [map, moves] = parseInput(file_content);

    auto new_map = dilateMap(map);

    auto robot_loc = findLocation(new_map);

    int width = new_map[0].size();
    int height = new_map.size();

    for (const auto m : moves)
    {

        const auto &[dy, dx] = directions[m];
        bool can_move{true};

        if (dy == 0)
        {
            int ny = robot_loc.first, nx = robot_loc.second + dx;
            for (; nx >= 0 && nx < width; nx += dx)
            {
                if (new_map[ny][nx] == '#')
                {
                    can_move = false;
                    break;
                }
                if (new_map[ny][nx] == '.')
                {
                    break;
                }
            }

            if (can_move)
            {
                int rdx = -dx;
                for (int x = nx; x != robot_loc.second; x += rdx)
                {
                    std::swap(new_map[ny][x], new_map[ny][x + rdx]);
                }
                robot_loc.second += dx;
            }

            continue;
        }

        std::vector<std::pair<int, int>> to_moves = {{robot_loc.first, robot_loc.second}};
        std::unordered_set<int> visited = {robot_loc.first * 100 + robot_loc.second};
        for (size_t i = 0; i < to_moves.size(); i++)
        {
            const auto [ny, nx] = to_moves[i];

            if (new_map[ny + dy][nx] == '#')
            {
                can_move = false;
                break;
            }

            if (new_map[ny][nx] == '[' && !visited.count(ny * 100 + nx + 1))
            {
                visited.insert(ny * 100 + nx + 1);
                to_moves.push_back(std::make_pair(ny, nx + 1));
            }
            if (new_map[ny][nx] == ']' && !visited.count(ny * 100 + nx - 1))
            {
                visited.insert(ny * 100 + nx - 1);
                to_moves.push_back(std::make_pair(ny, nx - 1));
            }

            if (new_map[ny + dy][nx] == '.')
            {
                continue;
            }

            if (visited.count((ny + dy) * 100 + nx))
                continue;

            visited.insert((ny + dy) * 100 + nx);
            to_moves.push_back(std::make_pair(ny + dy, nx));
        }

        if (!can_move)
        {
            continue;
        }

        std::reverse(to_moves.begin(), to_moves.end());

        for (const auto &[ny, nx] : to_moves)
        {
            std::swap(new_map[ny][nx], new_map[ny + dy][nx]);
        }

        robot_loc.first += dy;
    }

  

    int result{};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (new_map[i][j] == '[')
            {
                result += 100 * i + j;
            }
        }
    }

    return std::to_string(result);
}

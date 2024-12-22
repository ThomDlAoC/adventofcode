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
#include <array>
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
const std::array<std::pair<int, int>, 4> directions = {{{1, 0}, {-1, 0}, {0, -1}, {0, 1}}};

char directions_char(int dir)
{
    switch (dir)
    {
    case 0:
        return 'v';
    case 1:
        return '^';
    case 2:
        return '<';
    case 3:
        return '>';
    case -1:
        return '#';
    default:
        return 'X';
    }
}

constexpr std::array<const char *, 4> keypad = {
    "789",
    "456",
    "123",
    "X0A"};

const std::unordered_map<char, std::pair<int, int>> keypad_positions = {
    {'1', {2, 0}},
    {'2', {2, 1}},
    {'3', {2, 2}},
    {'4', {1, 0}},
    {'5', {1, 1}},
    {'6', {1, 2}},
    {'7', {0, 0}},
    {'8', {0, 1}},
    {'9', {0, 2}},
    {'0', {3, 1}},
    {'A', {3, 2}}};

std::vector<std::string> directional_keypad = {
    "X^A",
    "<v>"};

const std::unordered_map<char, std::pair<int, int>> directional_keypad_positions = {
    {'^', {0, 1}},
    {'v', {1, 1}},
    {'<', {1, 0}},
    {'>', {1, 2}},
    {'A', {0, 2}}};

auto key_map_dist = []()
{
    std::unordered_map<char, std::array<std::array<int, 3>, 4>> key_map_dist;
    for (const auto &[key, start_pos] : keypad_positions)
    {
        std::vector<std::pair<int, int>> q;
        std::array<std::array<bool, 3>, 4> visited = {};
        q.push_back(start_pos);
        visited[start_pos.first][start_pos.second] = true;
        key_map_dist[key] = {};
        key_map_dist[key][start_pos.first][start_pos.second] = 0;
        key_map_dist[key][3][0] = 100;

        for (size_t i = 0; i < q.size(); ++i)
        {
            auto [y, x] = q[i];

            for (int dir = 0; dir < 4; ++dir)
            {
                int new_y = y + directions[dir].first;
                int new_x = x + directions[dir].second;

                bool oob = new_y < 0 || new_y >= 4 || new_x < 0 || new_x >= 3;

                if (oob || visited[new_y][new_x])
                    continue;
                if (keypad[new_y][new_x] == 'X')
                    continue;

                visited[new_y][new_x] = true;
                key_map_dist[key][new_y][new_x] = key_map_dist[key][y][x] + 1;
                q.push_back({new_y, new_x});
            }
        }
    }
    return key_map_dist;
}();

auto directional_key_map_dist = []()
{
    std::unordered_map<char, std::array<std::array<int, 3>, 2>> directional_key_map_dist;
    for (const auto &[key, start_pos] : directional_keypad_positions)
    {

        if (key == 'X')
        {
            continue;
        }
        directional_key_map_dist[key] = {};
        std::vector<std::pair<int, int>> q;
        std::array<std::array<bool, 3>, 2> visited = {};
        q.push_back(start_pos);
        visited[start_pos.first][start_pos.second] = true;
        directional_key_map_dist[key][start_pos.first][start_pos.second] = 0;
        directional_key_map_dist[key][0][0] = 100;

        for (size_t i = 0; i < q.size(); ++i)
        {
            auto [y, x] = q[i];

            for (int dir = 0; dir < 4; ++dir)
            {
                int new_y = y + directions[dir].first;
                int new_x = x + directions[dir].second;

                bool oob = new_y < 0 || new_y >= 2 || new_x < 0 || new_x >= 3;

                if (oob || visited[new_y][new_x])
                    continue;
                if (directional_keypad[new_y][new_x] == 'X')
                    continue;

                visited[new_y][new_x] = true;
                directional_key_map_dist[key][new_y][new_x] = directional_key_map_dist[key][y][x] + 1;
                q.push_back({new_y, new_x});
            }
        }
    }
    return directional_key_map_dist;
}();

std::unordered_set<std::string> fromToListKeymap(char from, char to)
{

    std::string key = std::to_string(from) + std::to_string(to);

    std::unordered_set<std::string> result;
    auto start_pos = keypad_positions.at(from);
    auto target_pos = keypad_positions.at(to);

    std::vector<std::tuple<int, int, std::string>> q;
    q.push_back({start_pos.first, start_pos.second, ""});

    for (size_t i = 0; i < q.size(); ++i)
    {
        auto [y, x, path] = q[i];

        if (y == target_pos.first && x == target_pos.second)
        {
            path.push_back('A');
            result.insert(path);
        }

        for (int dir = 0; dir < 4; ++dir)
        {
            int new_y = y + directions[dir].first;
            int new_x = x + directions[dir].second;

            bool oob = new_y < 0 || new_y >= 4 || new_x < 0 || new_x >= 3;

            if (oob)
                continue;
            if (keypad[new_y][new_x] == 'X')
                continue;
            int next_dist = key_map_dist.at(to)[new_y][new_x];
            int current_dist = key_map_dist.at(to)[y][x];
            if (next_dist >= current_dist)
                continue;

            q.push_back({new_y, new_x, path + directions_char(dir)});
        }
    }

    return result;
}

std::unordered_set<std::string> fromToListDirectionalKeymap(char from, char to)
{
    std::string key = std::to_string(from) + std::to_string(to);

    std::unordered_set<std::string> result;
    auto start_pos = directional_keypad_positions.at(from);
    auto target_pos = directional_keypad_positions.at(to);

    std::vector<std::tuple<int, int, std::string>> q;
    q.push_back({start_pos.first, start_pos.second, ""});

    for (size_t i = 0; i < q.size(); ++i)
    {
        auto [y, x, path] = q[i];

        if (y == target_pos.first && x == target_pos.second)
        {
            path.push_back('A');
            result.insert(path);
        }

        for (int dir = 0; dir < 4; ++dir)
        {
            int new_y = y + directions[dir].first;
            int new_x = x + directions[dir].second;

            bool oob = new_y < 0 || new_y >= 2 || new_x < 0 || new_x >= 3;

            if (oob)
                continue;
            if (directional_keypad[new_y][new_x] == 'X')
                continue;
            int next_dist = directional_key_map_dist.at(to)[new_y][new_x];
            int current_dist = directional_key_map_dist.at(to)[y][x];
            if (next_dist >= current_dist)
                continue;

            q.push_back({new_y, new_x, path + directions_char(dir)});
        }
    }

    return result;
}

std::unordered_set<std::string> KeysToNextArrows(const std::string &sequence, size_t index)
{
    if (index == sequence.size() - 1)
    {
        return std::unordered_set<std::string>{""};
    }

    std::unordered_set<std::string> result;
    std::unordered_set<std::string> paths = fromToListKeymap(sequence[index], sequence[index + 1]);

    for (const std::string &path : paths)
    {
        std::unordered_set<std::string> sub_paths = KeysToNextArrows(sequence, index + 1);
        for (const std::string &sub_path : sub_paths)
        {
            result.insert(path + sub_path);
        }
    }

    return result;
}



std::unordered_set<std::string> ArrowsToNextArrows(const std::string_view &sequence)
{

    if (sequence.size() == 1)
    {
        return std::unordered_set<std::string>{""};
    }

    std::unordered_set<std::string> result;
    std::unordered_set<std::string> paths = fromToListDirectionalKeymap(sequence[0], sequence[1]);

    for (const std::string &path : paths)
    {
        std::unordered_set<std::string> sub_paths = ArrowsToNextArrows(sequence.substr(1));
        for (const std::string &sub_path : sub_paths)
        {
            result.insert(path + sub_path);
        }
    }

    return result;
}

std::unordered_map<std::string, uint64_t> memoShortest;
uint64_t getShortestPath(const std::string &arrow, int start)
{

    std::string key = arrow + "," + std::to_string(start);

    if (start == 0)
    {
        return arrow.size();
    }

    if (memoShortest.count(key))
    {
        return memoShortest[key];
    }

    uint64_t shortest = UINT64_MAX;

    auto narrows = ArrowsToNextArrows("A" + arrow);

    for (const auto &next_arrows : narrows)
    {
        uint64_t sum = 0;
        std::vector<std::string> sub;
        int last_idx = 0;
        for (size_t i = 0; i < next_arrows.size(); ++i)
        {
            if (next_arrows[i] == 'A')
            {
                sub.push_back(next_arrows.substr(last_idx, i - last_idx) + 'A');
                last_idx = i + 1;
            }
        }

        for (const auto &s : sub)
        {
            sum += getShortestPath(s, start - 1);
        }

        shortest = std::min(shortest, sum);
    }

    return memoShortest[key] = shortest;
}

std::string part1(std::stringstream &file_content)
{
    std::vector<std::string> instructions;
    uint64_t res{};
    for (std::string line; std::getline(file_content, line);)
    {
        auto arrowseq = KeysToNextArrows("A" + line, 0);
        uint64_t minLen = UINT64_MAX;
        for (const auto &arrow : arrowseq)
        {
            memoShortest.clear();
            minLen = std::min(minLen, getShortestPath(arrow, 2));
        }

        res += std::stoull(line) * minLen;
    }
    return std::to_string(res);
}

std::string part2(std::stringstream &file_content)
{

    std::vector<std::string> instructions;
    uint64_t res{};
    for (std::string line; std::getline(file_content, line);)
    {
        auto arrowseq = KeysToNextArrows("A" + line, 0);
        uint64_t minLen = UINT64_MAX;
        for (const auto &arrow : arrowseq)
        {
            memoShortest.clear();
            minLen = std::min(minLen, getShortestPath(arrow, 25));
        }

        res += std::stoull(line) * minLen;
    }
    return std::to_string(res);
}
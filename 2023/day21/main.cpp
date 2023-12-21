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
#include <algorithm>
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
const std::array<std::array<int, 2>, 4> directions = {{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}};

std::vector<std::string> parseInput(std::stringstream &file_content)
{
    std::vector<std::string> input;
    std::string line;
    while (std::getline(file_content, line))
    {
        std::string newline;
        for (int i = 0; i < 1; ++i)
            newline += line;
        input.push_back(newline);
    }

    std::vector<std::string> new_input;
    for (int i = 0; i < 1; ++i)
        new_input.insert(new_input.end(), input.begin(), input.end());

    return new_input;
}

#define PART1_KEEP_STEP 64

std::string part1(std::stringstream &file_content)
{

    auto input_map = parseInput(file_content);

    std::array<int, 2> position = {0, 0};
    bool position_found = false;

    for (size_t i = 0; i < input_map.size() && !position_found; i++)
    {
        for (size_t j = 0; j < input_map[i].size(); j++)
        {
            if (input_map[i][j] == 'S')
            {
                position[0] = i;
                position[1] = j;
                input_map[i][j] = '.';
                position_found = true;
                break;
            }
        }
    }

    std::queue<std::pair<std::array<int, 2>, int>> queue;
    auto hash = [](const std::pair<std::array<int, 2>, int> &p)
    { return std::hash<int>()(p.first[0]) ^ std::hash<int>()(p.first[1]); };
    auto cmp = [](const std::pair<std::array<int, 2>, int> &p1, const std::pair<std::array<int, 2>, int> &p2)
    { return p1.first == p2.first; };
    std::unordered_set<std::pair<std::array<int, 2>, int>, decltype(hash), decltype(cmp)> visited(100000, hash, cmp);

    queue.push({position, 0});
    visited.insert({position, 0});

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        for (auto &direction : directions)
        {
            std::array<int, 2> new_position = {current.first[0] + direction[0], current.first[1] + direction[1]};

            if (new_position[0] < 0 || new_position[0] >= (int)input_map.size() || new_position[1] < 0 || new_position[1] >= (int)input_map[0].size())
                continue;
            if (visited.find({new_position, current.second}) != visited.end())
                continue;
            if (input_map[new_position[0]][new_position[1]] != '.')
                continue;
            if (current.second >= PART1_KEEP_STEP)
                continue;

            queue.push({new_position, current.second + 1});
            visited.insert({new_position, current.second + 1});
        }
    }

    std::vector<std::pair<std::array<int, 2>, int>> kept_positions;
    std::copy_if(visited.begin(), visited.end(), std::back_inserter(kept_positions), [](const std::pair<std::array<int, 2>, int> &p)
                 { return p.second <= PART1_KEEP_STEP && p.second % 2 == (PART1_KEEP_STEP % 2); });

    return std::to_string(kept_positions.size());
}

#define PART2_KEEP_STEP 330

std::string part2(std::stringstream &file_content)
{

    auto input_map = parseInput(file_content);

    int map_width = input_map[0].size();
    int map_height = input_map.size();

    std::array<int, 2> position = {0, 0};
    bool position_found = false;
    for (size_t i = 0; i < input_map.size() && !position_found; i++)
    {
        for (size_t j = 0; j < input_map[i].size(); j++)
        {
            if (input_map[i][j] == 'S')
            {
                position[0] = i;
                position[1] = j;
                input_map[i][j] = '.';
                position_found = true;
                break;
            }
        }
    }

    std::queue<std::pair<std::array<int, 2>, int>> queue;
    auto hash = [](const std::pair<std::array<int, 2>, int> &p)
    { return std::hash<int>()(p.first[0]) ^ std::hash<int>()(p.first[1]); };
    auto cmp = [](const std::pair<std::array<int, 2>, int> &p1, const std::pair<std::array<int, 2>, int> &p2)
    { return p1.first == p2.first; };
    std::unordered_set<std::pair<std::array<int, 2>, int>, decltype(hash), decltype(cmp)> visited(100000, hash, cmp);

    queue.push({position, 0});
    visited.insert({position, 0});
    int kept_positions_cnt = PART2_KEEP_STEP % 2 == 0 ? 1 : 0;

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        for (auto &direction : directions)
        {
            std::array<int, 2> new_position = {current.first[0] + direction[0], current.first[1] + direction[1]};
            std::array<int, 2> modulo_position = {(map_height + new_position[0] % map_height) % map_height, (map_width + new_position[1] % map_width) % map_width};
            if (visited.find({new_position, current.second}) != visited.end())
                continue;
            if (input_map[modulo_position[0]][modulo_position[1]] != '.')
                continue;
            if (current.second >= PART2_KEEP_STEP)
                continue;

            queue.push({new_position, current.second + 1});
            if ((current.second + 1) % 2 == (PART2_KEEP_STEP % 2))
                kept_positions_cnt++;
            visited.insert({new_position, current.second + 1});
        }
    }

    std::vector<int> pos_cnt;
    for (int i = 0; i <= PART2_KEEP_STEP; ++i)
    {
        int cnt = std::count_if(visited.begin(), visited.end(), [i](const std::pair<std::array<int, 2>, int> &p)
                                { return p.second <= i && p.second % 2 == (i % 2); });

        pos_cnt.push_back(cnt);
    }

    std::vector<int> delta_pos_cnt(PART2_KEEP_STEP + 1, 0);
    for (size_t i = map_width; i < pos_cnt.size(); ++i)
    {
        delta_pos_cnt[i] = (pos_cnt[i] - pos_cnt[i - map_width]);
    }

    std::vector<int> delta_pos_delta_cnt(PART2_KEEP_STEP + 1, 0);
    for (size_t i = 2 * map_width; i < delta_pos_cnt.size(); ++i)
    {
        delta_pos_delta_cnt[i] = (delta_pos_cnt[i] - delta_pos_cnt[i - map_width]);
    }

    std::vector<std::vector<unsigned long long>> cycles;
    for (size_t i = 0; i < pos_cnt.size(); ++i)
    {
        cycles.push_back({(unsigned long long)pos_cnt[i], (unsigned long long)delta_pos_cnt[i], (unsigned long long)delta_pos_delta_cnt[i], (unsigned long long)i});
    }

    unsigned long long cycle_offset = 2 * map_width + 26501365 % map_width;
    auto &starting_cycle = cycles[cycle_offset];
    std::vector<unsigned long long> r = starting_cycle;
    unsigned long long k = (26501365 - r[3]) / map_width;
    return std::to_string(r[0] + r[1] * k + r[2] * (k * (k + 1) / 2));
}

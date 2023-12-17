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
#include <unordered_map>

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

#define MAX_STRAIGHT_COUNT 3
#define MAX_STRAIGHT_COUNT_P2 10
#define MIN_STRAIGHT_COUNT_P2 4
struct state_t
{
    std::array<int, 2> pos;
    std::array<int, 2> dir;
    int straight_count;
    int heat_loss;
};

bool is_valid_pos(const std::vector<std::string> &input_map, const std::array<int, 2> &pos)
{
    return pos[0] >= 0 && pos[0] < (int)input_map.size() && pos[1] >= 0 && pos[1] < (int)input_map[0].size();
}

std::array<int, 2> turn_left(const std::array<int, 2> &dir)
{
    return {-dir[1], dir[0]};
}

std::array<int, 2> turn_right(const std::array<int, 2> &dir)
{
    return {dir[1], -dir[0]};
}

bool cmp(const state_t &a, const state_t &b)
{
    return a.heat_loss > b.heat_loss;
}

void enqueueState(const std::vector<std::string> &input_map, std::priority_queue<state_t, std::vector<state_t>, decltype(&cmp)> &queue, std::unordered_map<int, int> &visited_loss, const std::array<int, 2> &next_pos, const std::array<int, 2> &next_dir, int next_straight_count, int heat_loss)
{
    if (!is_valid_pos(input_map, next_pos))
    {
        return;
    }

    int next_heat_loss = heat_loss + (input_map[next_pos[0]][next_pos[1]] - '0');
    int key = next_pos[0] * 1000000 + next_pos[1] * 1000 + (1 + next_dir[0]) * 100 + (1 + next_dir[1]) * 10 + next_straight_count;
    if (visited_loss.find(key) != visited_loss.end() && visited_loss[key] <= next_heat_loss)
    {
        return;
    }

    visited_loss[key] = next_heat_loss;
    queue.push({next_pos, next_dir, next_straight_count, next_heat_loss});
}

std::string part1(std::stringstream &file_content)
{

    auto input_map = parseInput(file_content);

    std::array<int, 2> ending_pos = {(int)input_map.size() - 1, (int)input_map[0].size() - 1};

    std::priority_queue<state_t, std::vector<state_t>, decltype(&cmp)> queue(cmp);
    std::unordered_map<int, int> visited_loss;
    queue.push({{0, 0}, {1, 0}, 0, 0});
    queue.push({{0, 0}, {0, 1}, 0, 0});

    while (!queue.empty())
    {
        auto state = queue.top();
        queue.pop();

        if (state.pos == ending_pos)
        {

            return std::to_string(state.heat_loss);
        }

        {
            auto next_dir = turn_left(state.dir);
            auto next_pos = state.pos;
            auto next_straight_count = 0;
            next_pos[0] += next_dir[0];
            next_pos[1] += next_dir[1];

            enqueueState(input_map, queue, visited_loss, next_pos, next_dir, next_straight_count, state.heat_loss);
        }

        {
            auto next_dir = turn_right(state.dir);
            auto next_pos = state.pos;
            auto next_straight_count = 0;
            next_pos[0] += next_dir[0];
            next_pos[1] += next_dir[1];

            enqueueState(input_map, queue, visited_loss, next_pos, next_dir, next_straight_count, state.heat_loss);
        }

        if (state.straight_count < MAX_STRAIGHT_COUNT - 1)
        {
            auto next_pos = state.pos;
            auto next_dir = state.dir;
            auto next_straight_count = state.straight_count + 1;
            next_pos[0] += state.dir[0];
            next_pos[1] += state.dir[1];

            enqueueState(input_map, queue, visited_loss, next_pos, next_dir, next_straight_count, state.heat_loss);
        }
    }

    return "failed";
}

std::string part2(std::stringstream &file_content)
{

    auto input_map = parseInput(file_content);

    std::array<int, 2> ending_pos = {(int)input_map.size() - 1, (int)input_map[0].size() - 1};

    std::priority_queue<state_t, std::vector<state_t>, decltype(&cmp)> queue(cmp);
    std::unordered_map<int, int> visited_loss;
    queue.push({{0, 0}, {1, 0}, 0, 0});
    queue.push({{0, 0}, {0, 1}, 0, 0});

    while (!queue.empty())
    {
        auto state = queue.top();
        queue.pop();

        if (state.pos == ending_pos)
        {

            return std::to_string(state.heat_loss);
        }

        if (state.straight_count >= MIN_STRAIGHT_COUNT_P2 - 1)
        {
            auto next_dir = turn_left(state.dir);
            auto next_pos = state.pos;
            auto next_straight_count = 0;
            next_pos[0] += next_dir[0];
            next_pos[1] += next_dir[1];

            enqueueState(input_map, queue, visited_loss, next_pos, next_dir, next_straight_count, state.heat_loss);
        }

        if (state.straight_count >= MIN_STRAIGHT_COUNT_P2 - 1)
        {
            auto next_dir = turn_right(state.dir);
            auto next_pos = state.pos;
            auto next_straight_count = 0;
            next_pos[0] += next_dir[0];
            next_pos[1] += next_dir[1];

            enqueueState(input_map, queue, visited_loss, next_pos, next_dir, next_straight_count, state.heat_loss);
        }

        if (state.straight_count < MAX_STRAIGHT_COUNT_P2 - 1)
        {
            auto next_pos = state.pos;
            auto next_dir = state.dir;
            auto next_straight_count = state.straight_count + 1;
            next_pos[0] += state.dir[0];
            next_pos[1] += state.dir[1];

            enqueueState(input_map, queue, visited_loss, next_pos, next_dir, next_straight_count, state.heat_loss);
        }
    }

    return "failed";
}

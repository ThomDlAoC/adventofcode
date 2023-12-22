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
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <queue>

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

struct cube_location_t
{
    int x,y,z;
};

struct brick_t
{
    std::vector<cube_location_t> cubes;
    int z;
    int height;
    std::unordered_set<brick_t *> supported_by;
    std::unordered_set<brick_t *> supports;
};

std::vector<brick_t> parseInput(std::stringstream &file_content)
{
    std::string line;
    std::vector<brick_t> bricks;
    while (std::getline(file_content, line))
    {
        std::istringstream iss(line);
        cube_location_t starting_cube;
        cube_location_t ending_cube;
        brick_t brick;
        char c;
        iss >> starting_cube.x >> c >> starting_cube.y >> c >> starting_cube.z >> c >> ending_cube.x >> c >> ending_cube.y >> c >> ending_cube.z;

        std::array<int, 3> diffs = {std::abs(starting_cube.x - ending_cube.x), std::abs(starting_cube.y - ending_cube.y), std::abs(starting_cube.z - ending_cube.z)};
        int max_diff = *std::max_element(diffs.begin(), diffs.end());
        for (int i = 0; i <= max_diff; i++)
        {
            cube_location_t cube = starting_cube;
            cube.x = starting_cube.x + (starting_cube.x != ending_cube.x ? i : 0);
            cube.y = starting_cube.y + (starting_cube.y != ending_cube.y ? i : 0);
            cube.z = starting_cube.z + (starting_cube.z != ending_cube.z ? i : 0);
            brick.cubes.push_back(cube);
        }

        brick.z = starting_cube.z;
        brick.height = ending_cube.z - starting_cube.z + 1;
        bricks.push_back(brick);
    }
    return bricks;
}

void stackBricks(std::vector<brick_t> &bricks)
{
    std::unordered_map<int, std::pair<int, brick_t *>> max_heights;
    auto computeCubeKey = [](const cube_location_t &cube)
    {
        return cube.x * 1000 + cube.y;
    };

    std::sort(bricks.begin(), bricks.end(), [](const brick_t &a, const brick_t &b)
              { return a.z < b.z; });

    for (auto &upper_brick : bricks)
    {
        std::unordered_set<brick_t *> lower_bricks;
        int next_min_height = 0;
        for (auto &cube : upper_brick.cubes)
        {
            int key = computeCubeKey(cube);
            if (max_heights.find(key) == max_heights.end())
                continue;

            if (max_heights[key].first > next_min_height)
            {
                next_min_height = max_heights[key].first;
                lower_bricks.clear();
                lower_bricks.insert(max_heights[key].second);
            }
            else if (max_heights[key].first == next_min_height)
                lower_bricks.insert(max_heights[key].second);
        }

        for (auto &cube : upper_brick.cubes)
            max_heights[computeCubeKey(cube)] = std::make_pair(next_min_height + upper_brick.height, &upper_brick);

        upper_brick.supported_by = lower_bricks;
        for (auto &lower_brick : lower_bricks)
            lower_brick->supports.insert(&upper_brick);
    }
}

std::string part1(std::stringstream &file_content)
{

    auto bricks = parseInput(file_content);

    stackBricks(bricks);

    auto canBeDestroyed = [](const brick_t &lower_brick)
    {
        return std::all_of(lower_brick.supports.begin(), lower_brick.supports.end(), [](const brick_t *upper_brick)
                           { return upper_brick->supported_by.size() >= 2; });
    };

    return std::to_string(std::count_if(bricks.begin(), bricks.end(), canBeDestroyed));
}

std::string part2(std::stringstream &file_content)
{

    auto bricks = parseInput(file_content);

    stackBricks(bricks);

    int total_destroy = 0;
    for (const auto &brick : bricks)
    {
        std::unordered_map<const brick_t *, bool> destroyed;
        for (const auto &brick : bricks)
            destroyed[&brick] = false;
        destroyed[&brick] = true;

        std::queue<const brick_t *> to_destroy;
        to_destroy.push(&brick);

        while (!to_destroy.empty())
        {
            const brick_t *current = to_destroy.front();
            to_destroy.pop();

            bool is_destroyable = destroyed[current] || std::all_of(current->supported_by.begin(), current->supported_by.end(), [&destroyed](const brick_t *b)
                                                                    { return destroyed[b]; });
            if (!is_destroyable)
                continue;

            total_destroy += !destroyed[current];
            destroyed[current] = true;
            for (const auto &support : current->supports)
                to_destroy.push(support);
        }
    }

    return std::to_string(total_destroy);
}
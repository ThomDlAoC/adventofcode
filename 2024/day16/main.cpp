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

std::pair<int, int> getLocation(const std::vector<std::string> &map, char c)
{

    int width = map[0].size();
    int height = map.size();

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (map[i][j] == c)
            {
                return std::make_pair(i, j);
            }
        }
    }

    return std::make_pair(-1, -1);
};

std::string part1(std::stringstream &file_content)
{

    std::vector<std::string> map;
    for (std::string line; std::getline(file_content, line);)
    {
        map.push_back(line);
    }

    auto start_location = getLocation(map, 'S');
    auto end_location = getLocation(map, 'E');

    using loc_t = std::tuple<int, int, int, int, int>;
    std::priority_queue<loc_t, std::vector<loc_t>, std::greater<loc_t>> pq;
    pq.push({0, start_location.first, start_location.second, 0, 1});

    std::vector<std::vector<int>> visited(map.size(), std::vector<int>(map[0].size(), INT_MAX - 1000));

    visited[start_location.first][start_location.second] = 0;

    while (!pq.empty())
    {
        auto [score, y, x, dy, dx] = pq.top();
        pq.pop();

        if (y == end_location.first && x == end_location.second)
        {
            return std::to_string(score);
        }

        for (int i = 0; i < 4; i++)
        {
            int new_dy = (i == 0) - (i == 1);
            int new_dx = (i == 2) - (i == 3);

            int new_x = x + new_dx;
            int new_y = y + new_dy;

            if (new_x < 0 || new_x >= (int)map[0].size() || new_y < 0 || new_y >= (int)map.size() || map[new_y][new_x] == '#')
            {
                continue;
            }

            if (new_dx && new_dx == -dx)
                continue;
            if (new_dy && new_dy == -dy)
                continue;

            int new_score = score + 1 + ((new_dx != dx || new_dy != dy) ? 1000 : 0);

            if (new_score <= visited[new_y][new_x] + 1000)
            {
                visited[new_y][new_x] = new_score;
                pq.push({new_score, new_y, new_x, new_dy, new_dx});
            }
        }
    }

    return "Not found";
}

std::string part2(std::stringstream &file_content)
{

    std::vector<std::string> map;
    for (std::string line; std::getline(file_content, line);)
    {
        map.push_back(line);
    }

    auto start_location = getLocation(map, 'S');
    auto end_location = getLocation(map, 'E');

    using loc_t = std::tuple<int, int, int, int, int, std::vector<std::pair<int, int>>>;
    std::priority_queue<loc_t, std::vector<loc_t>, std::greater<loc_t>> pq;
    pq.push({0, start_location.first, start_location.second, 0, 1, {{start_location.first, start_location.second}}});

    std::vector<std::vector<int>> visited(map.size(), std::vector<int>(map[0].size(), INT_MAX - 1000));

    visited[start_location.first][start_location.second] = 0;

    int min_score = INT_MAX;

    std::unordered_set<int> visited_set;

    while (!pq.empty())
    {
        auto el = pq.top();
        auto [score, y, x, dy, dx, list] = el;
        pq.pop();

        if (y == end_location.first && x == end_location.second)
        {
            if (score <= min_score)
            {
                min_score = score;
            }
            else
            {
                continue;
            }

            for (auto [y, x] : list)
            {
                visited_set.insert(y * 1000 + x);
            }

            continue;
        }

        for (int i = 0; i < 4; i++)
        {
            int new_dy = (i == 0) - (i == 1);
            int new_dx = (i == 2) - (i == 3);

            int new_x = x + new_dx;
            int new_y = y + new_dy;

            if (new_x < 0 || new_x >= (int)map[0].size() || new_y < 0 || new_y >= (int)map.size() || map[new_y][new_x] == '#')
            {
                continue;
            }

            if (new_dx && new_dx == -dx)
                continue;
            if (new_dy && new_dy == -dy)
                continue;

            int new_score = score + 1 + ((new_dx != dx || new_dy != dy) ? 1000 : 0);

            if (new_score <= visited[new_y][new_x] + 1000)
            {
                visited[new_y][new_x] = new_score;
                auto new_list = list;
                new_list.push_back({new_y, new_x});
                pq.push({new_score, new_y, new_x, new_dy, new_dx, new_list});
            }
        }
    }

    return std::to_string(visited_set.size());
}
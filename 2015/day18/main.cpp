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

std::string part1(std::stringstream &file_content)
{

    auto lights_map = parseInput(file_content);
    int width = lights_map[0].size();
    int height = lights_map.size();
    std::vector<std::array<int, 2>> neighbours_map = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (int loops = 0; loops < 100; ++loops)
    {
        std::vector<std::vector<int>> neighbours_on(width, std::vector<int>(height, 0));
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                for (auto neighbour : neighbours_map)
                {
                    int neighbour_x = x + neighbour[0];
                    int neighbour_y = y + neighbour[1];
                    if (neighbour_x >= 0 && neighbour_x < width && neighbour_y >= 0 && neighbour_y < height)
                    {
                        if (lights_map[neighbour_y][neighbour_x] == '#')
                        {
                            neighbours_on[y][x]++;
                        }
                    }
                }
            }
        }

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (lights_map[y][x] == '#' && (neighbours_on[y][x] < 2 || neighbours_on[y][x] > 3))
                {
                    lights_map[y][x] = '.';
                }
                else if (neighbours_on[y][x] == 3)
                {
                    lights_map[y][x] = '#';
                }
            }
        }
    }

    int lights_on = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (lights_map[y][x] == '#')
            {
                lights_on++;
            }
        }
    }

    return std::to_string(lights_on);
}

std::string part2(std::stringstream &file_content)
{

    auto lights_map = parseInput(file_content);
    int width = lights_map[0].size();
    int height = lights_map.size();
    std::vector<std::array<int, 2>> neighbours_map = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    lights_map[0][0] = '#';
    lights_map[0][width - 1] = '#';
    lights_map[height - 1][0] = '#';
    lights_map[height - 1][width - 1] = '#';

    for (int loops = 0; loops < 100; ++loops)
    {
        std::vector<std::vector<int>> neighbours_on(width, std::vector<int>(height, 0));
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                for (auto neighbour : neighbours_map)
                {
                    int neighbour_x = x + neighbour[0];
                    int neighbour_y = y + neighbour[1];
                    if (neighbour_x >= 0 && neighbour_x < width && neighbour_y >= 0 && neighbour_y < height)
                    {
                        if (lights_map[neighbour_y][neighbour_x] == '#')
                        {
                            neighbours_on[y][x]++;
                        }
                    }
                }
            }
        }

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (lights_map[y][x] == '#' && (neighbours_on[y][x] < 2 || neighbours_on[y][x] > 3))
                {
                    lights_map[y][x] = '.';
                }
                else if (neighbours_on[y][x] == 3)
                {
                    lights_map[y][x] = '#';
                }
            }
        }

        lights_map[0][0] = '#';
        lights_map[0][width - 1] = '#';
        lights_map[height - 1][0] = '#';
        lights_map[height - 1][width - 1] = '#';
    }

    int lights_on = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (lights_map[y][x] == '#')
            {
                lights_on++;
            }
        }
    }

    return std::to_string(lights_on);
}

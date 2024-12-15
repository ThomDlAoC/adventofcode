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

#define MAP_WIDTH 101
#define MAP_HEIGHT 103

struct coord
{
    int x;
    int y;
};

struct robot
{
    coord pos;
    coord velocity;
};

std::vector<robot> parseInput(std::stringstream &file_content)
{
    std::vector<robot> robots;
    std::string line;
    while (std::getline(file_content, line))
    {
        robot r;
        sscanf(line.c_str(), "p=%d,%d v=%d,%d", &r.pos.x, &r.pos.y, &r.velocity.x, &r.velocity.y);
        robots.push_back(r);
    }
    return robots;
}

robot getRobotAfterSeconds(const robot &r, int seconds)
{
    robot new_robot = r;
    new_robot.pos.x = (r.pos.x + r.velocity.x * seconds) % MAP_WIDTH;
    new_robot.pos.y = (r.pos.y + r.velocity.y * seconds) % MAP_HEIGHT;

    if (new_robot.pos.x < 0)
        new_robot.pos.x += MAP_WIDTH;
    if (new_robot.pos.y < 0)
        new_robot.pos.y += MAP_HEIGHT;
    return new_robot;
}

std::string part1(std::stringstream &file_content)
{

    auto robots = parseInput(file_content);
    std::array<int, 4> quads{};
    for (const auto &r : robots)
    {

        auto robot_moved = getRobotAfterSeconds(r, 100);

        if (robot_moved.pos.x < MAP_WIDTH / 2 && robot_moved.pos.y < MAP_HEIGHT / 2)
        {
            quads[0]++;
        }
        else if (robot_moved.pos.x > MAP_WIDTH / 2 && robot_moved.pos.y < MAP_HEIGHT / 2)
        {
            quads[1]++;
        }
        else if (robot_moved.pos.x < MAP_WIDTH / 2 && robot_moved.pos.y > MAP_HEIGHT / 2)
        {
            quads[2]++;
        }
        else if (robot_moved.pos.x > MAP_WIDTH / 2 && robot_moved.pos.y > MAP_HEIGHT / 2)
        {
            quads[3]++;
        }
    }

    int res = 1;
    for (const auto &q : quads)
    {
        res *= q;
    }

    return std::to_string(res);
}

std::string part2(std::stringstream &file_content)
{

    auto robots = parseInput(file_content);
    for (int i = 1; i < 10000; ++i)
    {
        std::array<int, 4> quads{};
        std::array<std::array<bool, MAP_WIDTH>, MAP_HEIGHT> map{};
        bool treeFound = false;
        for (const auto &r : robots)
        {

            auto robot_moved = getRobotAfterSeconds(r, i);

            if (robot_moved.pos.x < MAP_WIDTH / 2 && robot_moved.pos.y < MAP_HEIGHT / 2)
            {
                quads[0]++;
            }
            else if (robot_moved.pos.x > MAP_WIDTH / 2 && robot_moved.pos.y < MAP_HEIGHT / 2)
            {
                quads[1]++;
            }
            else if (robot_moved.pos.x < MAP_WIDTH / 2 && robot_moved.pos.y > MAP_HEIGHT / 2)
            {
                quads[2]++;
            }
            else if (robot_moved.pos.x > MAP_WIDTH / 2 && robot_moved.pos.y > MAP_HEIGHT / 2)
            {
                quads[3]++;
            }
            map[robot_moved.pos.y][robot_moved.pos.x] = true;
        }

       
        for(const auto &line : map)
        {
            int count = 0;
            for(const auto &v : line)
            {
                if(v)
                {
                    count++;
                    if(count == 10)
                    {
                        treeFound = true;
                        break;
                    }
                }
                else
                {
                    count = 0;
                }
            }
            if(treeFound)
            {
                return std::to_string(i);
            }
        }


    }

   

    return "";
}

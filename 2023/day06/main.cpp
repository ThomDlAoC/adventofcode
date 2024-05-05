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

void part1(std::stringstream &file_content);
void part2(std::stringstream &file_content);

std::optional<std::stringstream> readFileContent(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Could not open file " << path << std::endl;
        return std::nullopt;
    }

    std::stringstream file_content;
    file_content << file.rdbuf();
    file.close();

    return std::move(file_content);
};

void executeFunction(std::stringstream &file_content, std::function<void(std::stringstream &)> function)
{
    file_content.clear();
    file_content.seekg(0, std::ios::beg);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    function(file_content);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::optional<std::stringstream> file_content = readFileContent(path);

    if (!file_content)
    {
        std::cout << "Could not read file " << path << std::endl;
        return 1;
    }

    std::stringstream &file_content_ref = *file_content;

    executeFunction(file_content_ref, part1);
    executeFunction(file_content_ref, part2);

    return 0;
}

/***************************************
 *              Solutions
 **************************************/

std::vector<std::pair<int, int>> parseInput(std::stringstream &file_content)
{
    std::vector<std::pair<int, int>> result;
    std::string line;
    std::string num;

    {
        std::getline(file_content, line);
        std::stringstream ss(line);
        ss >> num;

        while (ss >> num)
        {
            int time = std::stoi(num);
            result.push_back(std::make_pair(time, 0));
        }
    }

    {
        std::getline(file_content, line);
        std::stringstream ss(line);
        int count = 0;
        ss >> num;

        while (ss >> num)
        {
            int duration = std::stoi(num);
            result[count++].second = duration;
        }
    }

    return result;
}

/**
 * Solves a quadratic equation of the form ax^2 + bx + c = 0.
 *
 * @param a The coefficient of x^2.
 * @param b The coefficient of x.
 * @param c The constant term.
 * @return A pair of roots (x1, x2) of the quadratic equation, sorted in descending order.
 */
std::pair<double, double> solveQuadraticEquation(const double a, const double b, const double c)
{
    double delta = b * b - 4 * a * c;
    double x1 = (-b + sqrt(delta)) / (2 * a);
    double x2 = (-b - sqrt(delta)) / (2 * a);

    return x1 > x2 ? std::make_pair(x1, x2) : std::make_pair(x2, x1);
}

void part1(std::stringstream &file_content)
{

    auto time_record_list = parseInput(file_content);
    int total = 1;
    for (const auto &[time, record] : time_record_list)
    {
        auto [x1, x2] = solveQuadraticEquation(-1, time, -record);
        total *= (int(x1 - .1) - (int)(x2 + 1.1) + 1);
    }

    std::cout << "Part 1: " << total << std::endl;
}

void part2(std::stringstream &file_content)
{
    auto time_record_list = parseInput(file_content);

    int total = 1;

    std::string time_s = "";
    std::string record_s = "";

    for (const auto &[time, record] : time_record_list)
    {
        time_s += std::to_string(time);
        record_s += std::to_string(record);
    }

    auto [x1, x2] = solveQuadraticEquation(-1, std::stod(time_s), -std::stod(record_s));
    total *= (int(x1 - .1) - int(x2 + 1.1) + 1);

    std::cout << "Part 2: " << total << std::endl;
}

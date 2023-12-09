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

#define FUEL_TO_STORE 150

std::vector<int> parseInput(std::stringstream &file_content)
{
    std::vector<int> input;
    std::string line;
    while (std::getline(file_content, line))
    {
        input.push_back(std::stoi(line));
    }
    return input;
}

void combinations(std::vector<int> &input, int fuel_to_store, int &count,std::vector<int> &used_fuel,std::vector<std::vector<int>> &used_fuel_combinations)
{
    if (fuel_to_store == 0)
    {
        count++;
        used_fuel_combinations.push_back(used_fuel);
        return;
    }

    if(input.size() == 0)
    {
        return;
    }

    if (fuel_to_store < 0)
    {
        return;
    }
        
        int fuel = input[0];
        input.erase(input.begin());
        used_fuel.push_back(fuel);
        combinations(input, fuel_to_store - fuel, count, used_fuel, used_fuel_combinations);
        used_fuel.pop_back();
        input.insert(input.begin(), fuel);

        input.erase(input.begin());
        combinations(input, fuel_to_store, count, used_fuel, used_fuel_combinations);
        input.insert(input.begin(), fuel);
    
}

std::string part1(std::stringstream &file_content)
{

    auto input = parseInput(file_content);

    int count = 0;
    std::vector<int> used_fuel;
    std::vector<std::vector<int>> used_fuel_combinations;
    combinations(input, FUEL_TO_STORE, count, used_fuel, used_fuel_combinations);

    return std::to_string(count);
}

std::string part2(std::stringstream &file_content)
{

    auto input = parseInput(file_content);

    int count = 0;
    std::vector<int> used_fuel;
    std::vector<std::vector<int>> used_fuel_combinations;
    combinations(input, FUEL_TO_STORE, count, used_fuel, used_fuel_combinations);

    std::sort(used_fuel_combinations.begin(), used_fuel_combinations.end(), [](const std::vector<int> &a, const std::vector<int> &b) {
        return a.size() < b.size();
    });

    auto min_size = used_fuel_combinations[0].size();

    int total_min_size = 0;

    for (auto &combination : used_fuel_combinations)
    {
        if (combination.size() == min_size)
        {
            total_min_size++;
        }
    }

    return std::to_string(total_min_size);
}

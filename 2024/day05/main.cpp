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
#include <bitset>

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

using update_t = std::vector<int>;
using updates_t = std::vector<update_t>;
using rules_t = std::bitset<10000>;

std::pair<rules_t, updates_t> parseInput(std::stringstream &file_content)
{
    rules_t rules;
    updates_t updates;

    for (std::string line; std::getline(file_content, line);)
    {
        if (line.empty())
            break;

        int a, b;
        sscanf(line.c_str(), "%d|%d", &a, &b);
        rules.set(a * 100 + b);
    }

    for (std::string line; std::getline(file_content, line);)
    {
        std::stringstream ss(line);
        updates.emplace_back();
        for (std::string number; std::getline(ss, number, ',');)
            updates.back().push_back(std::stoi(number));
    }

    return {rules, updates};
}

bool updateIsOk(const rules_t &rule_container, const update_t &update)
{
    int sz = update.size();
    for (int i = 1; i < sz; i++)
    {
        int key = update[i - 1] * 100 + update[i];
        if (!rule_container.test(key))
            return false;
    }

    return true;
}

std::string part1(std::stringstream &file_content)
{
    auto [rule_container, updates] = parseInput(file_content);
    int mid_count{};
    for (const auto &update : updates)
    {
        if (updateIsOk(rule_container, update))
            mid_count += update[update.size() / 2];
    }

    return std::to_string(mid_count);
}

std::string part2(std::stringstream &file_content)
{
    auto [rule_container, updates] = parseInput(file_content);
    int mid_count{};
    for (auto &update : updates)
    {
        if (updateIsOk(rule_container, update))
            continue;

        std::sort(update.begin(), update.end(), [&](int a, int b)
                  { return rule_container.test(a * 100 + b); });

        mid_count += update[update.size() / 2];
    }

    return std::to_string(mid_count);
}
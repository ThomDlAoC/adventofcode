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
struct rule_t
{
    char key;
    char op;
    int value;
    std::string next;
};

rule_t parseRule(std::string rule_s)
{
    size_t tok = rule_s.find(':');
    if (tok == rule_s.npos)
    {
        return (rule_t){.key = 0, .op = 0, .value = 0, .next = rule_s};
    }

    rule_t rule;
    rule.key = rule_s[0];
    rule.op = rule_s[1];
    rule.value = std::stoi(rule_s.substr(2));
    rule.next = rule_s.substr(tok + 1);

    return rule;
}

std::pair<std::string, std::vector<rule_t>> parseRules(std::string rule_line)
{
    std::string name;
    std::vector<rule_t> rules;
    std::string rule;
    bool in_name = true;
    bool in_rule = false;
    for (auto c : rule_line)
    {
        if (in_name)
        {
            if (c == '{')
            {
                in_name = false;
                in_rule = true;
            }
            else
            {
                name += c;
            }
        }
        else if (in_rule)
        {
            if (c == ',')
            {
                rules.push_back(parseRule(rule));
                rule = "";
            }
            else if (c == '}')
            {
                rules.push_back(parseRule(rule));
                rule = "";
                in_rule = false;
            }
            else
            {
                rule += c;
            }
        }
    }

    return {name, rules};
}

std::unordered_map<std::string, std::vector<rule_t>> parseInput(std::stringstream &file_content)
{
    std::unordered_map<std::string, std::vector<rule_t>> rules_list;
    std::string line;
    while (std::getline(file_content, line))
    {
        if (line.empty())
            return rules_list;
        auto [name, rules] = parseRules(line);
        rules_list[name] = rules;
    }

    return rules_list;
}

std::vector<std::unordered_map<char, int>> parseParts(std::stringstream &file_content)
{
    std::vector<std::unordered_map<char, int>> parts;
    std::string line;
    while (std::getline(file_content, line))
    {
        if (line.empty())
            return parts;
        std::unordered_map<char, int> part;
        size_t tok = line.find('=');
        while (tok != line.npos)
        {
            char key = line[tok - 1];
            int value = std::stoi(line.substr(tok + 1));
            part[key] = value;
            tok = line.find('=', tok + 1);
        }
        parts.push_back(part);
    }

    return parts;
}

bool isPartAccepted(std::unordered_map<char, int> &part, std::unordered_map<std::string, std::vector<rule_t>> &rules_list)
{
    std::string current_key = "in";

    while (true)
    {
        std::vector<rule_t> rules = rules_list[current_key];
        for (auto rule : rules)
        {
            if (rule.op == 0)
            {
                current_key = rule.next;
                break;
            }
            if (rule.op == '<' && part[rule.key] < rule.value)
            {
                current_key = rule.next;
                break;
            }
            if (rule.op == '>' && part[rule.key] > rule.value)
            {
                current_key = rule.next;
                break;
            }
        }

        if (current_key == "A")
            return true;
        if (current_key == "R")
            return false;
    }

    return true;
}

std::string part1(std::stringstream &file_content)
{

    auto rules_list = parseInput(file_content);
    auto parts = parseParts(file_content);

    std::vector<std::unordered_map<char, int>> accepted_parts;
    std::copy_if(parts.begin(), parts.end(), std::back_inserter(accepted_parts), [&](std::unordered_map<char, int> &part)
                 { return isPartAccepted(part, rules_list); });

    int sum_values = 0;
    for (auto part : accepted_parts)
    {
        for (auto _part : part)
            sum_values += _part.second;
    }

    return std::to_string(sum_values);
}

unsigned long long getTotalAccepted(std::unordered_map<std::string, std::vector<rule_t>> &rules_list, std::array<int, 8> min_max, std::string current_key)
{

    unsigned long long total = 0;
    if (current_key == "A")
        return (unsigned long long)(min_max[1] - min_max[0] + 1) * (unsigned long long)(min_max[3] - min_max[2] + 1) * (unsigned long long)(min_max[5] - min_max[4] + 1) * (unsigned long long)(min_max[7] - min_max[6] + 1);
    if (current_key == "R")
        return 0;

    std::vector<rule_t> rules = rules_list[current_key];
    for (const auto &rule : rules)
    {
        if (rule.op == 0)
            total += getTotalAccepted(rules_list, min_max, rule.next);

        if (rule.op == '<')
        {
            int index = std::string("xmas").find(rule.key);

            int n = std::min(min_max[index * 2 + 1], rule.value - 1);
            if (n >= min_max[index * 2])
            {
                int prev = min_max[index * 2 + 1];
                min_max[index * 2 + 1] = n;
                total += getTotalAccepted(rules_list, min_max, rule.next);
                min_max[index * 2 + 1] = prev;
            }
            min_max[index * 2] = std::max(min_max[index * 2], rule.value);
        }
        if (rule.op == '>')
        {
            int index = std::string("xmas").find(rule.key);

            int n = std::max(min_max[index * 2], rule.value + 1);
            if (n <= min_max[index * 2 + 1])
            {
                int prev = min_max[index * 2];
                min_max[index * 2] = n;
                total += getTotalAccepted(rules_list, min_max, rule.next);
                min_max[index * 2] = prev;
            }
            min_max[index * 2 + 1] = std::min(min_max[index * 2 + 1], rule.value);
        }
    }

    return total;
}

std::string part2(std::stringstream &file_content)
{
    auto rules_list = parseInput(file_content);
    return std::to_string(getTotalAccepted(rules_list, {1, 4000, 1, 4000, 1, 4000, 1, 4000}, "in"));
}
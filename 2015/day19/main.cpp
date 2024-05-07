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

std::unordered_map<std::string, std::vector<std::string>> populateReplaceMap(std::istream &file_content, bool swap)
{
    std::unordered_map<std::string, std::vector<std::string>> replace_map;
    std::string str;

    while (std::getline(file_content, str))
    {
        if (str.empty())
            break;
        std::string in, out;
        std::stringstream ss(str);
        ss >> in >> out >> out;
        if (swap)
            replace_map[out].push_back(in);
        else
            replace_map[in].push_back(out);
    }

    return replace_map;
}

std::string part1(std::stringstream &file_content)
{
    std::unordered_map<std::string, std::vector<std::string>> replace_map = populateReplaceMap(file_content, false);
    std::string instr;
    file_content >> instr;

    std::unordered_set<std::string> uniq;
    uniq.emplace(instr);

    for (int i = 0; i < static_cast<int>(instr.size()); i++)
    {
        std::string pref = instr.substr(0, i);
        for (const auto &[start_str, subst_list] : replace_map)
        {
            if (start_str == instr.substr(i, start_str.size()))
            {
                for (const auto &subst : subst_list)
                {
                    std::string new_instr = pref + subst + instr.substr(i + start_str.size());
                    uniq.emplace(new_instr);
                }
            }
        }
    }

    return std::to_string(uniq.size() - 1);
}

std::string part2(std::stringstream &file_content)
{
    std::unordered_map<std::string, std::vector<std::string>> replace_map = populateReplaceMap(file_content, true);
    std::string instr;
    file_content >> instr;

    std::unordered_set<std::string> visited;
    int min_steps = 0;

    std::priority_queue<std::pair<int, std::string>> min_step_queue;
    min_step_queue.push({0, instr});
    visited.emplace(instr);

    while (!min_step_queue.empty())
    {
        auto [steps, current_str] = min_step_queue.top();
        min_step_queue.pop();
        if (current_str == "e")
        {
            min_steps = steps;
            break;
        }
        int sz = static_cast<int>(current_str.size());
        for (int i = 0; i < sz; i++)
        {
            std::string pref = current_str.substr(0, i);
            for (const auto &[start_str, subst_list] : replace_map)
            {
                if (start_str == current_str.substr(i, start_str.size()))
                {
                    for (const auto &subst : subst_list)
                    {
                        std::string new_instr = pref + subst + current_str.substr(i + start_str.size());
                        if (visited.contains(new_instr))
                            continue;
                        visited.emplace(new_instr);
                        min_step_queue.push({steps + 1, new_instr});
                    }
                }
            }
        }
    }

    return std::to_string(min_steps);
}

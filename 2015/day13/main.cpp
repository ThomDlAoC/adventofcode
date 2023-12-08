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

std::pair<std::vector<std::string>, std::unordered_map<std::string, std::unordered_map<std::string, int>>> parseInput(std::stringstream &file_content)
{
    std::string line;
    std::unordered_set<std::string> attendees;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> happiness;
    while (std::getline(file_content, line))
    {
        std::string name;
        std::string dummy;
        std::string gainLoss;
        int amount;
        std::string neighbor;

        std::stringstream line_stream(line);
        line_stream >> name >> dummy >> gainLoss >> amount >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> neighbor;
        neighbor.pop_back();

        int value = gainLoss == "gain" ? amount : -amount;
        attendees.insert(name);
        attendees.insert(neighbor);

        happiness[name][neighbor] = value;
    }

    return {std::vector<std::string>(attendees.begin(), attendees.end()), happiness};
}

int factorial(int n)
{
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

std::string part1(std::stringstream &file_content)
{

    auto [attendees, happiness] = parseInput(file_content);
    int max_happiness = 0;

    std::sort(attendees.begin(), attendees.end());

    do
    {
        int current_happiness = 0;
        for (size_t i = 0; i < attendees.size(); i++)
        {
            const auto &attendee = attendees[i];
            const auto &neighbor = attendees[(i + 1) % attendees.size()];
            current_happiness += happiness[attendee][neighbor];
            current_happiness += happiness[neighbor][attendee];
        }

        max_happiness = std::max(max_happiness, current_happiness);

    } while (std::next_permutation(attendees.begin(), attendees.end()));

    return std::to_string(max_happiness);
}

std::string part2(std::stringstream &file_content)
{

    auto [attendees, happiness] = parseInput(file_content);
    int max_happiness = 0;

    attendees.push_back("Me");
    for (const auto &attendee : attendees)
    {
        happiness["Me"][attendee] = 0;
        happiness[attendee]["Me"] = 0;
    }

    std::sort(attendees.begin(), attendees.end());

    do
    {
        int current_happiness = 0;
        for (size_t i = 0; i < attendees.size(); i++)
        {
            const auto &attendee = attendees[i];
            const auto &neighbor = attendees[(i + 1) % attendees.size()];
            current_happiness += happiness[attendee][neighbor];
            current_happiness += happiness[neighbor][attendee];
        }

        max_happiness = std::max(max_happiness, current_happiness);

    } while (std::next_permutation(attendees.begin(), attendees.end()));

    return std::to_string(max_happiness);
}

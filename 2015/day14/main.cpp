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
struct Reindeer
{
    std::string name;
    int speed;
    int fly_time;
    int rest_time;
};

int reindeerDistanceAtTime(int speed, int fly_time, int rest_time, int time)
{
    int total_time = fly_time + rest_time;
    int num_cycles = time / total_time;
    return num_cycles * fly_time * speed + std::min(time % total_time, fly_time) * speed;
}

std::vector<Reindeer> parseInput(std::stringstream &file_content)
{
    std::string line;
    std::vector<Reindeer> reindeers;
    while (std::getline(file_content, line))
    {
        std::stringstream line_stream(line);
        std::string word;
        std::vector<std::string> words;
        while (line_stream >> word)
            words.push_back(word);

        std::string name = words[0];
        int speed = std::stoi(words[3]);
        int fly_time = std::stoi(words[6]);
        int rest_time = std::stoi(words[13]);
        reindeers.push_back({name, speed, fly_time, rest_time});
    }

    return reindeers;
}

#define LIMIT_ROUND 2503

std::string part1(std::stringstream &file_content)
{

    auto reindeers = parseInput(file_content);

    int max_distance = std::accumulate(reindeers.begin(), reindeers.end(), 0, [](int max_distance, Reindeer reindeer)
                                       { return std::max(max_distance, reindeerDistanceAtTime(reindeer.speed, reindeer.fly_time, reindeer.rest_time, LIMIT_ROUND)); });

    return std::to_string(max_distance);
}

std::string part2(std::stringstream &file_content)
{

    auto reindeers = parseInput(file_content);
    int max_points = 0;

    std::unordered_map<Reindeer *, int> reindeers_points;
    for (size_t i = 0; i <= LIMIT_ROUND; i++)
    {

        int max_distance = 0;
        std::vector<std::pair<Reindeer *, int>> reindeers_distance;
        for (auto &reindeer : reindeers)
        {
            int current_distance = reindeerDistanceAtTime(reindeer.speed, reindeer.fly_time, reindeer.rest_time, i + 1);
            if (current_distance >= max_distance)
            {
                max_distance = current_distance;
                reindeers_distance.push_back({&reindeer, current_distance});
            }
        }

        for (auto &reindeer_distance : reindeers_distance)
        {
            if (reindeer_distance.second == max_distance)
            {
                reindeers_points[reindeer_distance.first]++;
                max_points = std::max(max_points, reindeers_points[reindeer_distance.first]);
                        }
        }
    }

    return std::to_string(max_points);
}

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
 ************************************/

#define LOCATION_IDX 7
using valuesize = unsigned long long;
using range = std::pair<valuesize, valuesize>;
struct RangeMap
{
    range source;
    range destination;
};

using RangeMaps = std::array<std::vector<RangeMap>, LOCATION_IDX>;

std::pair<RangeMaps, std::vector<valuesize>> parseInput(std::stringstream &file_content)
{

    std::vector<RangeMap> range_map_vector;
    std::string line;
    std::getline(file_content, line);
    std::stringstream line_stream(line);
    std::vector<valuesize> seeds;

    int mapsIndex = 0;

    RangeMaps value_range_maps;

    /************
     * Parse seeds
     *************/
    for (std::string seed; line_stream >> seed;)
    {
        if (!std::isdigit(seed[0]))
            continue;
        seeds.push_back(std::stoull(seed));
    }

    std::getline(file_content, line);

    /************
     * Parse range maps
     * **********/
    while (std::getline(file_content, line))
    {
        if (line.empty() && range_map_vector.size() > 0)
        {
            std::sort(range_map_vector.begin(), range_map_vector.end(), [](const RangeMap &a, const RangeMap &b)
                      { return a.source.first < b.source.first; });
            value_range_maps[mapsIndex] = range_map_vector;
            range_map_vector.clear();
            mapsIndex++;
            continue;
        }
        if (!std::isdigit(line[0]))
            continue;

        std::stringstream line_stream(line);
        valuesize destination, source, length;
        line_stream >> destination >> source >> length;
        range_map_vector.push_back({{source, source + length - 1}, {destination, destination + length - 1}});
    }

    if (range_map_vector.size() > 0)
        value_range_maps[mapsIndex] = range_map_vector;

    return std::make_pair(value_range_maps, seeds);
}

bool isOverlapping(const range &rangeA, const range &rangeB)
{
    return !(rangeA.second < rangeB.first || rangeA.first > rangeB.second);
}

bool isWithinRange(const valuesize value, const range &range)
{
    return value >= range.first && value <= range.second;
}

valuesize getLocationFromSeed(const valuesize &seed, const RangeMaps &value_range_maps, const int index)
{
    if (index == LOCATION_IDX)
        return seed;

    for (const auto &value_range : value_range_maps[index])
    {
        if (isWithinRange(seed, value_range.source))
            return getLocationFromSeed(seed - value_range.source.first + value_range.destination.first, value_range_maps, index + 1);
    }

    return getLocationFromSeed(seed, value_range_maps, index + 1);
}

void part1(std::stringstream &file_content)
{
    valuesize min = ULLONG_MAX;
    auto [value_range_maps, seeds] = parseInput(file_content);

    for (auto seed : seeds)
        min = std::min(min, getLocationFromSeed(seed, value_range_maps, 0));

    std::cout << "Part 1 : " << min << std::endl;
}

valuesize getMinFromRange(const range &initial_seed, const RangeMaps &value_range_maps, const int index, const int range_index = 0)
{
    if (index == LOCATION_IDX)
        return initial_seed.first;

    for (int i = range_index, value_range_size = value_range_maps[index].size(); i < value_range_size; ++i)
    {
        const auto &value_range = value_range_maps[index][i];
        if (isOverlapping(initial_seed, value_range.source))
        {
            valuesize min = ULLONG_MAX;

            range overlapping_seed = {
                std::max(initial_seed.first, value_range.source.first) - value_range.source.first + value_range.destination.first,
                std::min(initial_seed.second, value_range.source.second) - value_range.source.first + value_range.destination.first};
            min = std::min(min, getMinFromRange(overlapping_seed, value_range_maps, index + 1, 0));

            if (initial_seed.second > value_range.source.second)
                min = std::min(min, getMinFromRange({value_range.source.second + 1, initial_seed.second}, value_range_maps, index, i + 1));

            if (initial_seed.first < value_range.source.first)
                min = std::min(min, getMinFromRange({initial_seed.first, value_range.source.first - 1}, value_range_maps, index + 1, 0));

            return min;
        }
    }

    return getMinFromRange(initial_seed, value_range_maps, index + 1, 0);
}

void part2(std::stringstream &file_content)
{
    valuesize min = ULLONG_MAX;
    auto [value_range_maps, seeds] = parseInput(file_content);
    std::vector<range> seed_ranges;
    for (size_t i = 0; i < seeds.size(); i += 2)
        seed_ranges.push_back({seeds[i], seeds[i] + seeds[i + 1] - 1});

    for (auto seed_range : seed_ranges)
        min = std::min(min, getMinFromRange(seed_range, value_range_maps, 0, 0));

    std::cout << "Part 2 : " << min << std::endl;
}

// g++ -g -O3 -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -std=c++17 main.cpp -o app
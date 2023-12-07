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

enum class HandType
{
    None = -1,
    HighCard = 0,
    OnePair = 1,
    TwoPair = 2,
    ThreeOfAKind = 3,
    FullHouse = 4,
    FourOfAKind = 5,
    FiveOfAKind = 6
};

HandType getHandType(int max_occurence, int different_cards)
{

    if (different_cards == 5)
        return HandType::HighCard;

    if (different_cards == 4)
        return HandType::OnePair;

    if (different_cards == 3)
        return max_occurence == 3 ? HandType::ThreeOfAKind : HandType::TwoPair;

    if (different_cards == 2)
        return max_occurence == 4 ? HandType::FourOfAKind : HandType::FullHouse;

    if (max_occurence == 5)
        return HandType::FiveOfAKind;

    return HandType::None;
}

int valueCmp(const std::string &a, const std::string &b, std::unordered_map<char, int> &cardValues)
{
    for (size_t i = 0; i < a.size(); i++)
    {
        if (cardValues[a[i]] < cardValues[b[i]])
            return -1;
        else if (cardValues[a[i]] > cardValues[b[i]])
            return 1;
    }
    return 0;
}

std::vector<std::tuple<std::string, int, int, int, int>> parsInput(std::stringstream &file_content)
{
    std::vector<std::tuple<std::string, int, int, int, int>> result;
    std::string line;
    while (std::getline(file_content, line))
    {
        std::stringstream line_stream(line);
        std::string hand_string;
        int value;
        int different_card = 0;
        int max_occurence = 0;
        int jcount = 0;
        std::unordered_map<char, int> valcount;

        line_stream >> hand_string >> value;
        for (auto c : hand_string)
        {
            if (c == 'J')
            {
                jcount++;
                continue;
            }
            if (valcount[c] == 0)
                different_card++;
            valcount[c]++;
            max_occurence = std::max(max_occurence, valcount[c]);
        }

        result.push_back({hand_string, value, different_card, jcount, max_occurence});
    }
    return result;
}

#define D_HAND_TYPE 0
#define D_HAND_STRING 1
#define D_VALUE 2

std::string part1(std::stringstream &file_content)
{
    std::unordered_map<char, int> cardValues = {
        {'2', 0}, {'3', 1}, {'4', 2}, {'5', 3}, {'6', 4}, {'7', 5}, {'8', 6}, {'9', 7}, {'T', 8}, {'J', 9}, {'Q', 10}, {'K', 11}, {'A', 12}};

    auto parseResultList = parsInput(file_content);
    std::vector<std::tuple<HandType, std::string, int>> list_hand_bid;

    for (const auto &[hand_string, value, different, jcount, max] : parseResultList)
        list_hand_bid.push_back({getHandType(std::max(max, jcount), different + (jcount != 0)), hand_string, value});

    std::sort(list_hand_bid.begin(), list_hand_bid.end(), [&](const auto &a, const auto &b)
              { return (std::get<D_HAND_TYPE>(a) == std::get<D_HAND_TYPE>(b)) ? valueCmp(std::get<D_HAND_STRING>(a), std::get<D_HAND_STRING>(b), cardValues) < 0 : std::get<D_HAND_TYPE>(a) < std::get<D_HAND_TYPE>(b); });
    
    int result = 0, k = 1;
    for (const auto &a : list_hand_bid)
        result += (k++) * std::get<D_VALUE>(a);
    return std::to_string(result);
}

std::string part2(std::stringstream &file_content)
{
    std::unordered_map<char, int> cardValues = {
        {'2', 0}, {'3', 1}, {'4', 2}, {'5', 3}, {'6', 4}, {'7', 5}, {'8', 6}, {'9', 7}, {'T', 8}, {'J', -1}, {'Q', 10}, {'K', 11}, {'A', 12}};

    auto parseResultList = parsInput(file_content);
    std::vector<std::tuple<HandType, std::string, int>> list_hand_bid;

    for (const auto &[hand_string, value, different, jcount, max] : parseResultList)
        list_hand_bid.push_back({getHandType(max + jcount, different), hand_string, value});

    std::sort(list_hand_bid.begin(), list_hand_bid.end(), [&](const auto &a, const auto &b)
              { return (std::get<D_HAND_TYPE>(a) == std::get<D_HAND_TYPE>(b)) ? valueCmp(std::get<D_HAND_STRING>(a), std::get<D_HAND_STRING>(b), cardValues) < 0 : std::get<D_HAND_TYPE>(a) < std::get<D_HAND_TYPE>(b); });

    int result = 0, k = 1;
    for (const auto &a : list_hand_bid)
        result += (k++) * std::get<D_VALUE>(a);

    return std::to_string(result);
}

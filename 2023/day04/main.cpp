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
        std::cerr << "Could not open file " << path << std::endl;
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
    std::cerr << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
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
        std::cerr << "Could not read file " << path << std::endl;
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

void part1(std::stringstream &file_content)
{
    int total_points = 0;

    for (std::string card_line; std::getline(file_content, card_line);)
    {

        bool winning_set[100] = {0};
        bool winning_set_done = false;
        int winning_points = 0;
        std::stringstream current_line_stream(card_line);

        for (std::string current_word = ""; current_line_stream >> current_word;)
        {
            if (current_word[0] == '|')
            {
                winning_set_done = true;
            }
            if (!std::isdigit(current_word.back()))
                continue;

            int value = std::stoi(current_word);

            if (!winning_set_done)
            {
                winning_set[value] = true;
            }
            else if (winning_set[value])
            {
                winning_points++;
            }
        }

        total_points += winning_points ? 1 << (winning_points - 1) : 0;
    }

    std::cout << total_points << std::endl;
}

void part2(std::stringstream &file_content)
{

    int total_scratchcards = 0;
    int cards_count_map[300] = {0};

    for (std::string card_line; std::getline(file_content, card_line);)
    {

        bool winning_set[100] = {0};
        bool winning_set_done = false;
        std::stringstream current_line_stream(card_line);

        int winning_card_count = 0;
        int current_card = 0;

        for (std::string current_word = ""; current_line_stream >> current_word;)
        {
            if (current_word.back() == ':')
            {
                current_card = std::stoi(current_word);
                cards_count_map[current_card]++;
                total_scratchcards++;
            }
            if (current_word[0] == '|')
            {
                winning_set_done = true;
            }
            if (!std::isdigit(current_word.back()))
                continue;

            int value = std::stoi(current_word);

            if (!winning_set_done)
            {
                winning_set[value] = true;
            }
            else if (winning_set[value])
            {
                winning_card_count = winning_card_count ? winning_card_count + 1 : current_card + 1;
                cards_count_map[winning_card_count] += cards_count_map[current_card];
                total_scratchcards += cards_count_map[current_card];
            }
        }
    }

    std::cout << total_scratchcards << std::endl;
}
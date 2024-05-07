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

/*

~~~~~~~~~<< WELCOME TO THE STORE !!! >>~~~~~~~~~

Weapons:    Cost  Damage  Armor
Dagger        8     4       0
Shortsword   10     5       0
Warhammer    25     6       0
Longsword    40     7       0
Greataxe     74     8       0

Armor:      Cost  Damage  Armor
Leather      13     0       1
Chainmail    31     0       2
Splintmail   53     0       3
Bandedmail   75     0       4
Platemail   102     0       5

Rings:      Cost  Damage  Armor
Damage +1    25     1       0
Damage +2    50     2       0
Damage +3   100     3       0
Defense +1   20     0       1
Defense +2   40     0       2
Defense +3   80     0       3
*/

using item_t = std::tuple<int, int, int>;

const std::vector<item_t> weapons = {
    {8, 4, 0},
    {10, 5, 0},
    {25, 6, 0},
    {40, 7, 0},
    {74, 8, 0}};

const std::vector<item_t> armor = {
    {13, 0, 1},
    {31, 0, 2},
    {53, 0, 3},
    {75, 0, 4},
    {102, 0, 5}};

const std::vector<item_t> rings = {
    {25, 1, 0},
    {50, 2, 0},
    {100, 3, 0},
    {20, 0, 1},
    {40, 0, 2},
    {80, 0, 3}};

void pickWeapon(item_t current_setup, std::vector<item_t> &setups);
void pickArmor(item_t current_setup, std::vector<item_t> &setups);
void pickRing(item_t current_setup, std::vector<item_t> &setups, int left, std::unordered_set<int> used_rings = {});

void pickWeapon(item_t current_setup, std::vector<item_t> &setups)
{

    for (auto &[cost, damage, armor] : weapons)
    {
        setups.push_back({cost, damage, armor});
        item_t next_setup = {cost, damage, armor};
        pickArmor(next_setup, setups);
        pickRing(next_setup, setups, 2);
    }
}

void pickArmor(item_t current_setup, std::vector<item_t> &setups)
{
    auto &[c_cost, c_damage, c_armor] = current_setup;
    for (auto &[cost, damage, armor] : armor)
    {
        setups.push_back({cost + c_cost, damage + c_damage, armor + c_armor});
        item_t next_setup = {cost + c_cost, damage + c_damage, armor + c_armor};
        pickRing(next_setup, setups, 2);
    }
}

void pickRing(item_t current_setup, std::vector<item_t> &setups, int left, std::unordered_set<int> used_rings)
{

    if (!left)
        return;

    auto &[c_cost, c_damage, c_armor] = current_setup;
    for (auto &[cost, damage, armor] : rings)
    {
        if (used_rings.contains(cost))
            continue;
        setups.push_back({cost + c_cost, damage + c_damage, armor + c_armor});
        item_t next_setup = {cost + c_cost, damage + c_damage, armor + c_armor};
        used_rings.insert(cost);
        pickRing(next_setup, setups, left - 1, used_rings);
    }
}

std::string part1(std::stringstream &file_content)
{
    std::string boss_hit, boss_damage, boss_armor;
    file_content >> boss_hit >> boss_hit >> boss_hit;
    file_content >> boss_damage >> boss_damage;
    file_content >> boss_armor >> boss_armor;

    int boss_hit_int = std::stoi(boss_hit);
    int boss_damage_int = std::stoi(boss_damage);
    int boss_armor_int = std::stoi(boss_armor);
    std::vector<item_t> setups;

    pickWeapon({0, 0, 0}, setups);

    std::sort(setups.begin(), setups.end(), [](const item_t &a, const item_t &b)
              {
        auto &[cost_a,_1,_2] = a;
        auto &[cost_b,_3,_4] = b;
        return cost_a < cost_b; });

    for (auto &[cost, damage, armor] : setups)
    {
        int player_damage = damage;
        int player_armor = armor;

        int player_life = 100;
        int boss_life = boss_hit_int;

        int player_deal = std::max(1, player_damage - boss_armor_int);
        int boss_deal = std::max(1, boss_damage_int - player_armor);

        player_deal = std::max(1, player_damage - boss_armor_int);
        boss_deal = std::max(1, boss_damage_int - player_armor);

        while (player_life > 0 && boss_life > 0)
        {
            boss_life -= player_deal;
            if (boss_life <= 0)
                return std::to_string(cost);
            player_life -= boss_deal;
        }
    }

    return "";
}

std::string part2(std::stringstream &file_content)
{
    std::string boss_hit, boss_damage, boss_armor;
    file_content >> boss_hit >> boss_hit >> boss_hit;
    file_content >> boss_damage >> boss_damage;
    file_content >> boss_armor >> boss_armor;

    int boss_hit_int = std::stoi(boss_hit);
    int boss_damage_int = std::stoi(boss_damage);
    int boss_armor_int = std::stoi(boss_armor);
    std::vector<item_t> setups;

    pickWeapon({0, 0, 0}, setups);

    std::sort(setups.begin(), setups.end(), [](const item_t &a, const item_t &b)
              {
        auto &[cost_a,_1,_2] = a;
        auto &[cost_b,_3,_4] = b;
        return cost_a > cost_b; });

    for (auto &[cost, damage, armor] : setups)
    {
        int player_damage = damage;
        int player_armor = armor;

        int player_life = 100;
        int boss_life = boss_hit_int;

        int player_deal = std::max(1, player_damage - boss_armor_int);
        int boss_deal = std::max(1, boss_damage_int - player_armor);

        player_deal = std::max(1, player_damage - boss_armor_int);
        boss_deal = std::max(1, boss_damage_int - player_armor);

        while (player_life > 0 && boss_life > 0)
        {
            boss_life -= player_deal;            
            player_life -= boss_deal;
            if (player_life <= 0)
                return std::to_string(cost);
        }
    }

    return "";
}

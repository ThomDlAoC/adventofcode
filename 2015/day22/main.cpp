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
#include <queue>
#include <array>
#include <variant>

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

class GameState;
class Spell;

class Spell
{
public:
    int mana_cost = 0;
    int timer = 0;
    int current_timer = 0;
    int damage = 0;
    int heal = 0;
    int armor = 0;
    int mana_gain = 0;
    Spell(int mana_cost, int timer, int damage, int heal, int armor, int mana_gain) : mana_cost(mana_cost), timer(timer), damage(damage), heal(heal), armor(armor), mana_gain(mana_gain) {}
    void apply(GameState &state);
    void cast(GameState &state);
};

class GameState
{

public:   
    int mana_used=0;
    int player_hp=0;
    int player_mana=0;
    int player_armor=0;
    int boss_hp=0;
    int boss_damage=0;

    std::array<Spell, 5> spells = {
        Spell(53, 0, 4, 0, 0, 0),
        Spell(73, 0, 2, 2, 0, 0),
        Spell(113, 6, 0, 0, 7, 0),
        Spell(173, 6, 3, 0, 0, 0),
        Spell(229, 5, 0, 0, 0, 101)
    };
};

void Spell::apply(GameState &state)
{
    if (current_timer == 0)
    {
        if (armor)
            state.player_armor = 0;
        return;
    }
    if (armor)
        state.player_armor = armor;
    if (mana_gain)
        state.player_mana += mana_gain;
    if (damage)
        state.boss_hp -= damage;
    current_timer--;
}

void Spell::cast(GameState &state)
{
    state.mana_used += mana_cost;
    state.player_mana -= mana_cost;
    if (timer)
    {
        current_timer = timer;
        return;
    }

    if (damage)
        state.boss_hp -= damage;
    if (heal)
        state.player_hp += heal;
}

std::string play(std::stringstream &file_content, bool hard_mode)
{

    GameState starting_state;
    starting_state.player_hp = 50;
    starting_state.player_mana = 500;
    file_content >> starting_state.boss_hp;
    file_content >> starting_state.boss_damage;

    auto state_cmp = [](const GameState &a, const GameState &b)
    {
        return a.mana_used > b.mana_used;
    };

    std::priority_queue<GameState, std::vector<GameState>, decltype(state_cmp)> states(state_cmp);

    states.push(starting_state);

    while (!states.empty())
    {
        auto state = states.top();
        states.pop();

        if (hard_mode)
            state.player_hp--;

        if (state.player_hp <= 0)
            continue;

        for (auto &spell : state.spells)
            spell.apply(state);

        if (state.boss_hp <= 0)
            return std::to_string(state.mana_used);

        for (int spell_index = 0; spell_index < 5; spell_index++)
        {
            auto next_state = state;
            auto &spell = next_state.spells[spell_index];
            if (spell.current_timer > 0 || spell.mana_cost > next_state.player_mana)
                continue;
            spell.cast(next_state);

            for (auto &spell : next_state.spells)
                spell.apply(next_state);

            if (next_state.boss_hp <= 0)
                return std::to_string(next_state.mana_used);

            int damage = std::max(1, next_state.boss_damage - next_state.player_armor);
            next_state.player_hp -= damage;

            if (next_state.player_hp <= 0)
                continue;

            states.push(next_state);
        }
    }
    return "something went wrong";
}

std::string part1(std::stringstream &file_content)
{
    return play(file_content, false);
}

std::string part2(std::stringstream &file_content)
{
    return play(file_content, true);
}

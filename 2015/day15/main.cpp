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
struct Ingredient
{
    std::string name;
    int capacity;
    int durability;
    int flavor;
    int texture;
    int calories;
};

std::vector<Ingredient> parseIngredients(std::stringstream &file_content)
{
    std::vector<Ingredient> ingredients;
    std::string line;
    line.reserve(1000);
    
    while (std::getline(file_content, line))
    {
        Ingredient ingredient;
        char name[100];
        std::sscanf(line.c_str(), "%s capacity %d, durability %d, flavor %d, texture %d, calories %d", name, &ingredient.capacity, &ingredient.durability, &ingredient.flavor, &ingredient.texture, &ingredient.calories);
        ingredient.name = name;
        ingredients.push_back(ingredient);
    }

    return ingredients;

}


int explore(int count, int idx, int total, std::vector<int> &amounts, std::vector<Ingredient> &ingredients, int max_calories=INT_MAX){
    if(idx==count-1){
        amounts[idx]=total;

        int capacity=0;
        int durability=0;
        int flavor=0;
        int texture=0;
        int calories=0;

        for(int i=0;i<count;i++){
            capacity+=amounts[i]*ingredients[i].capacity;
            durability+=amounts[i]*ingredients[i].durability;
            flavor+=amounts[i]*ingredients[i].flavor;
            texture+=amounts[i]*ingredients[i].texture;
            calories+=amounts[i]*ingredients[i].calories;
        }

        if(capacity<0) capacity=0;
        if(durability<0) durability=0;
        if(flavor<0) flavor=0;
        if(texture<0) texture=0;

        return calories>max_calories?0:capacity*durability*flavor*texture;
    }

    int res=0;
    for(int i=1;(i<=total-1-count+idx);i++){
        amounts[idx]=i;
        res=std::max(explore(count,idx+1,total-i,amounts,ingredients,max_calories),res);
    }

    return res;

}

std::string part1(std::stringstream &file_content)
{
    auto ingredients = parseIngredients(file_content);

    std::vector<int> amounts(ingredients.size(),0);
    int maxvalue = explore(ingredients.size(),0,100,amounts,ingredients,INT_MAX);
   

    return std::to_string(maxvalue);
}

std::string part2(std::stringstream &file_content)
{

     auto ingredients = parseIngredients(file_content);

    std::vector<int> amounts(ingredients.size(),0);
    int maxvalue = explore(ingredients.size(),0,100,amounts,ingredients,500);
   

    return std::to_string(maxvalue); 
}

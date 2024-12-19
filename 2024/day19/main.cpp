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

struct Trie
{
    bool word{};
    std::unordered_map<char, Trie> child;
};


Trie lineToTrie(const std::string &line){

    std::stringstream ss(line);
    Trie res;
    for(std::string sub; std::getline(ss,sub,',');ss.ignore(1)){
        
        Trie *root = &res;

        for(const char c : sub){

            if(!root->child.count(c)){
                root->child[c];
            }

            root=&root->child[c];


        }

        root->word=true;
        
    }

    return res;
}

std::pair<Trie,std::vector<std::string>> parseInput(std::stringstream &file_content){

    int linecount=0;
    Trie t;
    std::vector<std::string> lines;
    for(std::string line; std::getline(file_content,line);linecount++){
        if(linecount==1)continue;
        if(linecount==0){
            t = lineToTrie(line);
        }else{
            lines.push_back(line);
        }

    }


    return {t,lines};
}

bool isPossible(const std::string &s,Trie &t, size_t idx,std::unordered_set<size_t> &memo){

    if(idx == s.size()){
        return true;
    }

    if(memo.count(idx))return false;

    Trie *next = &t;
    for(size_t i=idx;i<s.size();++i){
        if(!next->child.count(s[i]))break;
        next = &next->child[s[i]];
        if(next->word && isPossible(s,t,i+1,memo))return true;
    }

    memo.insert(idx);
    return false;
}



std::string part1(std::stringstream &file_content)
{

    auto [trie, list] = parseInput(file_content);   

  

    return std::to_string(std::count_if(list.begin(),list.end(),[&](const auto &s){
        std::unordered_set<size_t> memo{};
        return isPossible(s,trie,0,memo);
    }));
}

unsigned long long countPossible(const std::string &s,Trie &t, size_t idx,std::unordered_map<size_t,unsigned long long> &memo){

    if(idx == s.size()){
        return 1;
    }

    if(memo.count(idx))return memo[idx];

    unsigned long long res=0;

    Trie *next = &t;
    for(size_t i=idx;i<s.size();++i){
        if(!next->child.count(s[i]))break;
        next = &next->child[s[i]];
        if(next->word)res+=countPossible(s,t,i+1,memo);
    }

    memo.emplace(idx,res);
    return res;
}

std::string part2(std::stringstream &file_content)
{

    auto [trie, list] = parseInput(file_content);   

  

    return std::to_string(std::accumulate(list.begin(),list.end(),0llu,[&](unsigned long long c, const auto &s){
        std::unordered_map<size_t,unsigned long long> memo{};
        return c+countPossible(s,trie,0,memo);
    }));

}

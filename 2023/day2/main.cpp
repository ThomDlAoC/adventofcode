#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>
#include <memory>

void part1(std::stringstream &file_content);
void part2(std::stringstream &file_content);

std::optional<std::stringstream> readFileContent(std::string path){
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

void executeFunction(std::stringstream &file_content, std::function<void(std::stringstream&)> function){
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
        std::cerr << "Could not read file " << path << std::endl;
        return 1;
    }

    std::stringstream &file_content_ref = *file_content;

    executeFunction(file_content_ref, part1);
    executeFunction(file_content_ref, part2);

    return 0;
}



struct Trie {
    std::unique_ptr<Trie> children[26];
    int count = 0;
    Trie* childOrNew(char c) {
        if(c<'a' || c>'z') return nullptr;
        if (children[c - 'a'] == nullptr) {
            children[c - 'a'] = std::make_unique<Trie>();
        }
        return children[c - 'a'].get();
    }

    Trie *child(char c) {
        if(c<'a' || c>'z') return nullptr;
        return children[c - 'a'].get();
    }

};

void part1(std::stringstream &file_content){
    std::array<std::pair<std::string,int>, 3> colors = {std::make_pair("red",12), std::make_pair("green",13), std::make_pair("blue",14)};
    std::unique_ptr<Trie> root = std::make_unique<Trie>();

    for(auto &color : colors){
        Trie *node = root.get();
        for(auto &c : color.first){
            node = node->childOrNew(c);
        }
        node->count = color.second;
    }
    int id=0;
    int s=0;
    for(std::string line;std::getline(file_content,line);){
        Trie *node = root.get();
        int val = 0;
        int current=0;
        id++;
        int valid=1;
        for(auto &c : line){

            if(c >='0' && c <= '9'){
                current = current*10 + (c - '0');
                continue;
            }else if(current != 0){
                val = current;
                current = 0;
            }

         
            node = node->child(c);
            if(node == nullptr){
                node = root.get();
            }else if(node->count != 0){
                int nodeval = node->count;
                node = root.get();
                if(val > nodeval){
                    valid = 0;
                    break;
                } 
            }
        }
        if(valid) s+=id;
    }

    std::cout << "sum " << s << std::endl;
    
    
}


void part2(std::stringstream &file_content){

    std::array<std::pair<std::string,int>, 3> colors = {std::make_pair("red",1), std::make_pair("green",2), std::make_pair("blue",3)};
    std::unique_ptr<Trie> root = std::make_unique<Trie>();

    for(auto &color : colors){
        Trie *node = root.get();
        for(auto &c : color.first){
            node = node->childOrNew(c);
        }
        node->count = color.second;
    }
    int id=0;
    int sum=0;
     for(std::string line;std::getline(file_content,line);){
        Trie *node = root.get();
        int val = 0;
        int current=0;
        int arr[4]={0,0,0,0};
        id++;
        for(auto &c : line){

            if(c >='0' && c <= '9'){
                current = current*10 + (c - '0');
                continue;
            }else if(current != 0){
                val = current;
                current = 0;
            }

         
            node = node->child(c);
            if(node == nullptr){
                node = root.get();
            }else if(node->count != 0){
                int nodeval = node->count;
                node = root.get();
                arr[nodeval] = std::max(arr[nodeval],val);
            }
        }
        
        int m=1;
        for(int i=1;i<4;i++){
            m*=arr[i];
        }
        sum+=m;

    }

    std::cout << "sum " << sum << std::endl;

    
}
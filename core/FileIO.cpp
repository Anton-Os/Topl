#include "FileIO.hpp"

std::string readFile(const char* source){
    std::ifstream file(source);
    if(! file) return std::string();

    // file.ignore(std::numeric_limits<std::streamsize>::max());
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::stringstream strStream;
    strStream << file.rdbuf();
    file.close();

    return strStream.str();
}
#include "IO.hpp"

std::string readFile(const char* source){
    std::ifstream file = std::ifstream(source, std::ios::in);
	if(!file.is_open()) return std::string(); // error occured, empty string returned

    // file.ignore(std::numeric_limits<std::streamsize>::max());
    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::stringstream strStream;
    strStream << file.rdbuf();
    file.close();

    return strStream.str();
}

std::string readFileBinary(const char* source) {
	std::ifstream file = std::ifstream(source, std::ios::in | std::ios::binary);
	if (!file.is_open()) return std::string(); // error occured, empty string returned

	// file.ignore(std::numeric_limits<std::streamsize>::max());
	file.clear();
	file.seekg(0, std::ios_base::beg);

	std::stringstream strStream;
	strStream << file.rdbuf();
	file.close();

	return strStream.str();
}

std::string getParentDir(const char* str){
    const char* strEnd = str + strlen(str); // Traverse to the end of the string
    while(strEnd != str && nullptr == strchr("\\/", *(--strEnd)));

    std::size_t length = strEnd - str; // Gets length of string
    std::string parentDir(str, length); // Gets a subset of the string

    return parentDir;
}

/* void logMessage(enum MESSAGE_Type type, std::string message){
    std::ofstream file(MESSAGE_LOG_FILE, std::ofstream::out | std::ofstream::app);
    switch(type){
        case MESSAGE_Comment: file << " * "; break;
        case MESSAGE_Question: file << " (?) "; break;
        case MESSAGE_Exclaim: file << " (!) "; break;
    }
    file << message << "\n\n";
    file.close();
} */

void logMessage(enum MESSAGE_Type type, std::string message) {
	switch (type) {
	case MESSAGE_Comment: std::cout << message ; break;
	case MESSAGE_Question: std::clog << message << " (?) "; break;
	case MESSAGE_Exclaim: std::cerr << message << " (!) "; break;
	}
}

void logMessage(std::string message){
    logMessage(MESSAGE_Comment, message);
}

void cleanupNumStr(std::string* str){
    for(std::string::iterator c = str->begin(); c != str->end(); c++)
        if(isalpha((int)*c) || *c == '/' || *c == '\n') *c = ' '; // replace unwanted characters with whitespace
    /* std::unique_copy(str->begin(), str->end(), std::back_insert_iterator<std::string>(*str),
        [](char a,char b){ return isspace(a) && isspace(b);});  */
}

/* unsigned getValsCountFromStr(const std::string& source){ return getValsCountFromStr(source, 1); }

unsigned getValsCountFromStr(const std::string& source, unsigned short div){
    unsigned valsCount = 0;
    bool isValReading = false;
    
	for (std::string::const_iterator ci = source.cbegin(); ci < source.end(); ci++)
	if (!isspace(*ci) && isValReading == false) {
		valsCount++;
		isValReading = true; // indicates that a value is being read
	}
	else if (isspace(*ci) && isValReading) isValReading = false; // indicates that value is done reading
	else continue;

    return valsCount / div;
} */

float getFloatFromStr(const std::string& source, size_t startOffset){
    if(!isdigit(source.at(startOffset)) && source.at(startOffset) != '-'){ // minus symbol is permitted
        fprintf(stderr, "Invalid offset provided obtaining float value. Location: %d", (int)startOffset);
        return FLT_MAX;
    }

    std::string currentStrVal;
    while(isdigit(source.at(startOffset)) || source.at(startOffset) == '.' || source.at((int)startOffset) == '-'){ // minus and dot symbol permitted
        currentStrVal += source.at(startOffset);
        startOffset++;
    }

    return (float)atof(currentStrVal.c_str());
}

int getIntFromStr(const std::string& source, size_t startOffset){
    if(!isdigit(source.at(startOffset)) && source.at(startOffset) != '-'){ // minus symbol is permitted
        fprintf(stderr, "Invalid offset provided obtaining float value. Location: %d", (int)startOffset);
        return INT_MAX;
        // return FLT_MAX;
    }

    std::string currentStrVal;
    while(isdigit(source.at(startOffset)) || source.at((int)startOffset) == '-'){ // minus and permitted
        currentStrVal += source.at(startOffset);
        startOffset++;
    }

    return (int)atoi(currentStrVal.c_str());
}

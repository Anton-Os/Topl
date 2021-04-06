#include "Platform.hpp"


int main(int argc, char** argv){
    if(argc != 2) return 1; // Improper number of arguments

    Platform platform(argv[0]);
    char* fbxFileInput = argv[1];

    if(! checkFormat_FBX(fbxFileInput)) return 2; // File provided is not FBX
    else printf("Reading FBX File: %s", fbxFileInput);

    FBX_DocumentTree fbxDocTree(fbxFileInput); // TODO: Test to see if working
}
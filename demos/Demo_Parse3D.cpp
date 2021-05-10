#include "Platform.hpp"
#include "File3D.hpp"

int main(int argc, char** argv){
    if(argc != 2) return 1; // Improper number of arguments

    Platform platform(argv[0]);
	char* fileInput = argv[1];

	printf("Reading 3D File: %s", fileInput);
    File3D_DocumentTree docTree(fileInput); // TODO: Test to see if working

	return 0;
}
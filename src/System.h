#ifndef UDEN_SYSTEM
#define UDEN_SYSTEM

#include <string>
#include <iostream>
#include <fstream>

void writeToFile(std::string path, std::string content);

std::string readFromFile(std::string path);

bool fileExists(std::string path);

#endif

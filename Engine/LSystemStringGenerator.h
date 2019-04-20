#pragma once

#include <iostream>
#include <string>

using namespace std;

class LSystemStringGenerator
{
public:

	LSystemStringGenerator();
	~LSystemStringGenerator();

	string generateString(string initialString, int passes);

};


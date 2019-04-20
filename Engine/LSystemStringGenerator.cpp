#include "LSystemStringGenerator.h"

LSystemStringGenerator::LSystemStringGenerator()
{
}

LSystemStringGenerator::~LSystemStringGenerator()
{
}

//Start = 'FA'
//Rule: 'A' -> "[&FA][&/FA][&\\FA]"

string LSystemStringGenerator::generateString(string initialString, int passes)
{
	std::string outputString = "";

	for (int j = 0; j < passes; j++)
	{
		outputString = "";

		for (int i = 0; i < initialString.size(); i++)
		{
			if (initialString[i] == 'A')
			{
				outputString += "[&FA][&/FA][&\\FA]";
			}
			else
			{
				outputString += initialString[i];
			}
		}
		initialString = outputString;
	}

	return outputString;
}

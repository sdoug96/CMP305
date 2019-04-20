#include "LSystemStringGenerator.h"

LSystemStringGenerator::LSystemStringGenerator()
{
}

LSystemStringGenerator::~LSystemStringGenerator()
{
}

//Start = 'FA'
//Rule: 'A' -> "[&FA][&/FA][&\\FA]"
// [ = store the state
// ] = revert the state
// F = create tree segment
// A = Terminator (create a leaf / do nothing)
// & = Pitch out by X degrees
// / & \\ = rotate around the Y axis by -X or +X degrees

string LSystemStringGenerator::generateString(string initialString)
{
	std::string outputString = "";
	for (int j = 0; j < 5; j++)
	{
		outputString = "";
		for (int i = 0; i < initialString.size(); i++)
		{
			if (initialString[i] == 'B')
			{
				//initialString.string::replace(i, 1, "AB");
				outputString += "BB";
			}
			else if (initialString[i] == 'A')
			{
				//initialString.string::replace(i, 1, "A");
				outputString += "B[A]A";
			}
			else
			{
				//initialString[i] = initialString[i];
				outputString += initialString[i];
			}
		}
		initialString = outputString;
	}

	return outputString;
}

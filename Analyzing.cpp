#include "Analyzing.h"

class Analyzer
{
public:
	Analyzer() {}
	void VerifyRawValue(int Value, int& CheckedValue)
	{
		mVerifyRawValue(Value, CheckedValue);
	}


private:
	void mVerifyRawValue(int Value, int& CheckedValue)
	{
		if ((Value >= 0) && (Value <= MAX_ADCRESOLUTION))
		{
			CheckedValue = Value;
		}
		else // ignore value
		{
		}
	}

};

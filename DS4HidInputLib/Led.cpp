#include "pch.h"


#include "Led.h"

LED::LED(uchar red, uchar green, uchar blue)  : red( red), green( green), blue(blue)
{
}

LED::LED(uint ui)  : ui( ui)
{
}

LED LED::Red() 
{
	static LED red(0x00ff0000);
	return red;
}

LED LED::Blue() 
{
	static LED blue(0x000000ff);
	return blue;
}

LED LED::Green() 
{
	static LED green(0x0000ff00);
	return green;
}

LED LED::Purple() 
{
	static LED purple(0x00ff00ff);
	return purple;

}

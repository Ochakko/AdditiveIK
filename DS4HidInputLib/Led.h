#pragma once

typedef unsigned char uchar;
typedef unsigned int uint;

struct LED
{
	union
	{
		struct
		{
			uchar blue;
			uchar green;
			uchar red;
		};
		uint ui;
	};
	LED(uchar red, uchar green, uchar blue) ;
	explicit LED(uint ui) ;
	static LED Red() ;
	static LED Blue() ;
	static LED Green() ;
	static LED Purple() ;
};

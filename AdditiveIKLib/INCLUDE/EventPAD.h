#ifndef EVENTPADH
#define EVENTPADH

#include <coef.h>

class CEventPad
{
public:
	CEventPad();
	~CEventPad();

	int InitParams();
	int DestroyObjs();

	int AddEPad( EPAD srcek );
	int DelEPadByIndex( int srcindex );

	int GetEventNo( int srcpad, int srccnt );

	int CheckSamePad( int srcpad, int* indexptr );

public:
	int GetPadNum() {
		return m_padnum;
	};
	int GetEventNo(int kindex) {
		if ((kindex >= 0) && (kindex < m_padnum)) {
			return m_epad[kindex].eventno;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};
	int GetPad(int kindex) {
		if ((kindex >= 0) && (kindex < m_padnum)) {
			return m_epad[kindex].pad;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};
	int GetComboNo(int kindex) {
		if ((kindex >= 0) && (kindex < m_padnum)) {
			return m_epad[kindex].combono;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};
	int GetSingleEvent(int kindex) {
		if ((kindex >= 0) && (kindex < m_padnum)) {
			return m_epad[kindex].singleevent;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};




private:
	EPAD m_epad[MOA_PADNUM];
	int m_padnum;

	int m_savemotid;
	int m_saveframeno;
};

#endif

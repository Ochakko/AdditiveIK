#ifndef EVENTKEYH
#define EVENTKEYH

#include <coef.h>

class CEventKey
{
public:
	CEventKey();
	~CEventKey();

	int InitParams();
	int DestroyObjs();

	int AddEKey( EKEY srcek );
	int DelEKeyByIndex( int srcindex );

	int GetEventNo( int srckey, int srccnt );

	int CheckSameKey( int srckey, int* indexptr );

public:
	int GetKeyNum() {
		return m_keynum;
	};
	int GetEventNo(int kindex) {
		if ((kindex >= 0) && (kindex < m_keynum)) {
			return m_ekey[kindex].eventno;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};
	int GetKey(int kindex) {
		if ((kindex >= 0) && (kindex < m_keynum)) {
			return m_ekey[kindex].key;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};
	int GetComboNo(int kindex) {
		if ((kindex >= 0) && (kindex < m_keynum)) {
			return m_ekey[kindex].combono;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};
	int GetSingleEvent(int kindex) {
		if ((kindex >= 0) && (kindex < m_keynum)) {
			return m_ekey[kindex].singleevent;
		}
		else {
			_ASSERT(0);
			return 0;
		}
	};




private:
	EKEY m_ekey[256];
	int m_keynum;

	int m_savemotid;
	int m_saveframeno;
};

#endif

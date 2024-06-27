#ifndef MOTFILTERH
#define MOTFILTERH


class CMotFilter
{
public:
	CMotFilter();
	~CMotFilter();

	int Filter(bool edgesmp, bool limitdegflag, CModel* srcmodel, CBone* srcbone, 
		int srcopekind, int srcmotid, int srcstartframe, int srcendframe);

	int FilterNoDlg(bool edgesmp, bool limitdegflag, CModel* srcmodel, CBone* srcbone,
		int srcopekind, int srcmotid, int srcstartframe, int srcendframe);

private:
	void InitParams();
	void DestroyObjs();

	int GetFilterType();
	int Combi(int N, int rp);

	int CallFilterFunc(bool edgesmp, bool limitdegflag, CModel* srcmodel, CBone* srcbone,
		int srcopekind, int srcmotid, int srcstartframe, int srcendframe);
	void FilterReq(bool edgesmp, bool limitdegflag, CModel* srcmodel, CBone* curbone, 
		int srcmotid, int srcstartframe, int srcendframe, bool broflag);
	int FilterFunc(bool edgesmp, bool limitdegflag, CModel* srcmodel, CBone* curbone, 
		int srcmotid, int srcstartframe, int srcendframe);


private:
	int m_filtertype;
	int m_filtersize;

	ChaVector3* m_eul;
	ChaVector3* m_smootheul;
	ChaVector3* m_tra;
	ChaVector3* m_smoothtra;

};


#endif

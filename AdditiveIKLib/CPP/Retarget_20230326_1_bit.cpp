#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#define RETARGETCPP
#include <Retarget.h>

#include <GlobalVar.h>
#include <Model.h>
#include <Bone.h>
//#include <ChaVecCalc.h>

#define DBGH
#include <dbg.h>

extern LONG g_retargetbatchflag;

namespace MameBake3DLibRetarget {

	static void RetargetReq(CModel* srcmodel, CModel* srcbvhmodel, CBone* modelbone, double srcframe, CBone* befbvhbone, float hrate, ChaMatrix& firsthipbvhmat, ChaMatrix& firsthipmodelmat, std::map<CBone*, CBone*>& sconvbonemap);
	static int ConvBoneRotation(CModel* srcmodel, CModel* srcbvhmodel, int selfflag, CBone* srcbone, CBone* bvhbone, double srcframe, CBone* befbvhbone, float hrate, ChaMatrix& firsthipbvhmat, ChaMatrix& firsthipmodelmat);


	int Retarget(CModel* srcmodel, CModel* srcbvhmodel, ChaMatrix smatVP, std::map<CBone*, CBone*>& sconvbonemap, int (*srcAddMotionFunc)(const WCHAR* wfilename, double srcmotleng), int (*srcInitCurMotionFunc)(int selectflag, double expandmotion))
	{

		//retarget�́@unlimited�ɑ΂��čs���@unlimited�ɃZ�b�g����
		bool limitdegflag = false;

		if (!srcmodel || !srcbvhmodel || !srcAddMotionFunc || !srcInitCurMotionFunc) {
			return 0;
		}

		g_underRetargetFlag = true;//!!!!!!!!!!!!

		MOTINFO* bvhmi = srcbvhmodel->GetMotInfoBegin()->second;
		if (!bvhmi) {
			::MessageBox(NULL, L"motion of bvh is not found error.", L"error!!!", MB_OK);
			g_underRetargetFlag = false;
			return 1;
		}
		double motleng = bvhmi->frameleng;//2022/11/01
		//double motleng = bvhmi->frameleng - 1.0;//2021/10/13
		(srcAddMotionFunc)(0, motleng);
		//(srcInitCurMotionFunc)(0, 0);//CModel::AddMotion�ŏ��������邱�Ƃɂ����̂ŃR�����g�A�E�g�@2022/08/28


		MOTINFO* modelmi = srcmodel->GetCurMotInfo();
		CBone* modelbone;
		if (modelmi) {
			CBone* modeltopbone = srcmodel->GetTopBone();
			CBone* modelhipsbone = 0;
			if (!modeltopbone) {
				::MessageBox(NULL, L"modelside bone is not found error.", L"error!!!", MB_OK);
				g_underRetargetFlag = false;
				return 1;
			}
			else {
				srcmodel->GetHipsBoneReq(modeltopbone, &modelhipsbone);
				if (modelhipsbone) {
					modelbone = modelhipsbone;
				}
				else {
					modelbone = modeltopbone;
				}
			}
			//modelbone = srcmodel->GetTopBone();
		}
		else {
			::MessageBox(NULL, L"modelside motion is not found error.", L"error!!!", MB_OK);
			g_underRetargetFlag = false;
			return 1;
		}


		CBone* bvhtopbone = 0;
		CBone* bvhhipsbone = 0;
		CBone* befbvhbone = 0;
		bvhtopbone = srcbvhmodel->GetTopBone();
		if (bvhtopbone) {
			srcbvhmodel->GetHipsBoneReq(bvhtopbone, &bvhhipsbone);
			if (bvhhipsbone) {
				befbvhbone = bvhhipsbone;
			}
			else {
				befbvhbone = bvhtopbone;
			}
		}
		//befbvhbone = srcbvhmodel->GetTopBone();

		HINFO bvhhi;
		bvhhi.minh = 1e7;
		bvhhi.maxh = -1e7;
		bvhhi.height = 0.0f;
		srcbvhmodel->SetFirstFrameBonePos(&bvhhi, befbvhbone);//hips�w��

		HINFO modelhi;
		modelhi.minh = 1e7;
		modelhi.maxh = -1e7;
		modelhi.height = 0.0f;
		srcmodel->SetFirstFrameBonePos(&modelhi, modelbone);//hips�w��

		float hrate;
		if (bvhhi.height != 0.0f) {
			hrate = modelhi.height / bvhhi.height;
		}
		else {
			//hrate = 0.0f;
			hrate = 1.0f;
			_ASSERT(0);
		}

		//2023/02/08
		if (fabs(hrate) <= 0.0001f) {
			hrate = 1.0f;
		}


		double frame;
		for (frame = 0.0; frame < motleng; frame += 1.0) {
			//s_sethipstra = 0;

			if (modelbone) {
				ChaMatrix dummyvpmat;
				ChaMatrixIdentity(&dummyvpmat);
				srcbvhmodel->SetMotionFrame(frame);
				ChaMatrix tmpbvhwm = srcbvhmodel->GetWorldMat();
				ChaMatrix tmpwm = srcmodel->GetWorldMat();
				srcbvhmodel->UpdateMatrix(limitdegflag, &tmpbvhwm, &dummyvpmat);
				srcmodel->SetMotionFrame(frame);
				srcmodel->UpdateMatrix(limitdegflag, &tmpwm, &dummyvpmat);

				CBone* befbvhbone2 = srcbvhmodel->GetTopBone();

				ChaMatrix firsthipbvhmat;
				ChaMatrix firsthipmodelmat;
				ChaMatrixIdentity(&firsthipbvhmat);
				ChaMatrixIdentity(&firsthipmodelmat);

				if (befbvhbone2) {
					RetargetReq(srcmodel, srcbvhmodel, modelbone, frame, befbvhbone2, hrate, firsthipbvhmat, firsthipmodelmat, sconvbonemap);
				}
			}
		}

		ChaMatrix tmpwm = srcmodel->GetWorldMat();
		srcmodel->UpdateMatrix(limitdegflag, &tmpwm, &smatVP);

		g_underRetargetFlag = false;//!!!!!!!!!!!!

		//if (!g_retargetbatchflag) {
		if (InterlockedAdd(&g_retargetbatchflag, 0) == 0) {
			::MessageBox(NULL, L"Finish of convertion.", L"check!!!", MB_OK);
		}

		return 0;
	}




	void RetargetReq(CModel* srcmodel, CModel* srcbvhmodel, CBone* modelbone, double srcframe, CBone* befbvhbone, float hrate, ChaMatrix& firsthipbvhmat, ChaMatrix& firsthipmodelmat, std::map<CBone*, CBone*>& sconvbonemap)
	{
		if (!srcmodel || !srcbvhmodel) {
			return;
		}

		if (!modelbone) {
			_ASSERT(0);
			return;
		}

		CBone* bvhbone = sconvbonemap[modelbone];
		if (bvhbone) {
			ConvBoneRotation(srcmodel, srcbvhmodel, 1, modelbone, bvhbone, srcframe, befbvhbone, hrate, firsthipbvhmat, firsthipmodelmat);
		}
		else {
			ConvBoneRotation(srcmodel, srcbvhmodel, 0, modelbone, 0, srcframe, befbvhbone, hrate, firsthipbvhmat, firsthipmodelmat);
		}


		if (modelbone->GetChild()) {
			if (bvhbone) {
				RetargetReq(srcmodel, srcbvhmodel, modelbone->GetChild(), srcframe, bvhbone, hrate, firsthipbvhmat, firsthipmodelmat, sconvbonemap);
			}
			else {
				RetargetReq(srcmodel, srcbvhmodel, modelbone->GetChild(), srcframe, befbvhbone, hrate, firsthipbvhmat, firsthipmodelmat, sconvbonemap);
			}
		}
		if (modelbone->GetBrother()) {
			//if (bvhbone){
			//	ConvBoneConvertReq(modelbone->GetBrother(), srcframe, bvhbone, hrate);
			//}
			//else{
			RetargetReq(srcmodel, srcbvhmodel, modelbone->GetBrother(), srcframe, befbvhbone, hrate, firsthipbvhmat, firsthipmodelmat, sconvbonemap);
			//}
		}

	}

	int ConvBoneRotation(CModel* srcmodel, CModel* srcbvhmodel, int selfflag, CBone* srcbone, CBone* bvhbone, double srcframe, CBone* befbvhbone, float hrate, ChaMatrix& firsthipbvhmat, ChaMatrix& firsthipmodelmat)
	{

		//retarget�́@unlimited�ɑ΂��čs���@unlimited�ɃZ�b�g����
		bool limitdegflag = false;



		if (selfflag && !bvhbone) {
			_ASSERT(0);
			return 1;
		}
		if ((selfflag == 0) && !befbvhbone) {
			_ASSERT(0);
			return 1;
		}

		if (!srcmodel || !srcbvhmodel || !srcbone) {
			_ASSERT(0);
			return 1;
		}


		//###################################################################
		//2023/02/02
		//GetCurMp().GetWorldMat�ɂ́@��O�I�Ƀ��f����worldmat���|�����Ă���
		//�A�j���p���̌v�Z�ɂ́@GetCurMp().GetAnimMat()���g�p
		//###################################################################


		double roundingframe = (double)((int)(srcframe + 0.0001));

		//static ChaMatrix s_firsthipmat;
		//static ChaMatrix s_invfirsthipmat;

		MOTINFO* bvhmi;
		int bvhmotid;

		if (srcbvhmodel->GetMotInfoBegin() != srcbvhmodel->GetMotInfoEnd()) {
			bvhmi = srcbvhmodel->GetMotInfoBegin()->second;
			if (bvhmi) {
				bvhmotid = bvhmi->motid;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}
			else {
				_ASSERT(0);
				return 1;
			}
		}
		else {
			_ASSERT(0);
			return 1;
		}


		bool onaddmotion = true;//for getbychain
		CMotionPoint bvhmp;
		if (bvhbone) {
			//CMotionPoint* pbvhmp = 0;
			//pbvhmp = bvhbone->GetMotionPoint(bvhmotid, roundingframe, onaddmotion);
			//if (pbvhmp) {
			//	bvhmp = *pbvhmp;
			//}
			//else {
			//	_ASSERT(0);
			//	return 1;
			//}
			bvhmp = bvhbone->GetCurMp();
		}
		else {
			//CMotionPoint* pbvhmp = 0;
			//pbvhmp = befbvhbone->GetMotionPoint(bvhmotid, roundingframe, onaddmotion);
			//if (pbvhmp) {
			//	bvhmp = *pbvhmp;
			//}
			//else {
			//	_ASSERT(0);
			//	return 1;
			//}
			bvhmp = befbvhbone->GetCurMp();
		}


		MOTINFO* modelmi = srcmodel->GetCurMotInfo();
		if (modelmi) {
			int modelmotid = modelmi->motid;
			CMotionPoint modelmp;
			CMotionPoint* pmodelmp = 0;
			pmodelmp = srcbone->GetMotionPoint(modelmotid, roundingframe, onaddmotion);
			if (pmodelmp) {
				modelmp = *pmodelmp;
			}
			else {
				_ASSERT(0);
				return 1;
			}

			CMotionPoint modelparmp;
			CMotionPoint* pmodelparmp = 0;
			if (srcbone->GetParent()) {
				pmodelparmp = srcbone->GetParent()->GetMotionPoint(modelmotid, roundingframe, onaddmotion);
				if (pmodelparmp) {
					modelparmp = *pmodelparmp;
				}
			}


			int curboneno = srcbone->GetBoneNo();


			CQuaternion rotq;
			ChaVector3 traanim;

			if (bvhbone) {
				//ChaMatrix curbvhmat;
				//ChaMatrix bvhmat;
				//bvhmat = bvhmp.GetWorldMat();

				//ChaMatrix modelinit, invmodelinit;
				//modelinit = modelmp.GetWorldMat();
				//invmodelinit = modelmp.GetInvWorldMat();

				CBone* modelfirstbone = 0;
				CBone* modeltopbone = srcmodel->GetTopBone();
				CBone* modelhipsbone = 0;
				if (modeltopbone) {
					srcmodel->GetHipsBoneReq(modeltopbone, &modelhipsbone);
					if (modelhipsbone) {
						modelfirstbone = modelhipsbone;
					}
					else {
						modelfirstbone = modeltopbone;
					}
				}

				CBone* bvhfirstbone = 0;
				CBone* bvhtopbone = srcbvhmodel->GetTopBone();
				CBone* bvhhipsbone = 0;
				if (bvhtopbone) {
					srcbvhmodel->GetHipsBoneReq(bvhtopbone, &bvhhipsbone);
					if (bvhhipsbone) {
						bvhfirstbone = bvhhipsbone;
					}
					else {
						bvhfirstbone = bvhtopbone;
					}
				}


				//if (srcbone == srcmodel->GetTopBone()) {//���f�����̍ŏ��̃{�[���̏�����
				//if (modelfirstbone && (srcbone == modelfirstbone)) {//���f�����̍ŏ��̃{�[���̏�����

				//	//firsthipbvhmat��firsthipmodelmat�́@���̊֐��̎Q�ƈ����@��x�Z�b�g���Ďg���܂킷
				//	
				//	//#######################################################################################
				//	//2022/12/21 ver1.1.0.10�֌�����
				//	//��10033�ƑO����������킹��
				//	//bvh����0�t���[���p����Identity�ɂȂ�悤�Ɂ@InvFirstMat * NodeMat ���|����
				//	//#######################################################################################
				//	firsthipbvhmat = ChaMatrixInv(bvhbone->GetFirstMat()) * bvhbone->GetNodeMat() * bvhmp.GetAnimMat();
				//	firsthipbvhmat.data[MATI_41] = 0.0f;
				//	firsthipbvhmat.data[MATI_42] = 0.0f;
				//	firsthipbvhmat.data[MATI_43] = 0.0f;

				//	firsthipmodelmat = modelmp.GetWorldMat();
				//	firsthipmodelmat.data[MATI_41] = 0.0f;
				//	firsthipmodelmat.data[MATI_42] = 0.0f;
				//	firsthipmodelmat.data[MATI_43] = 0.0f;
				//}

				if (bvhbone->GetParent()) {
					firsthipbvhmat = bvhbone->GetParent()->GetWorldMat(false, bvhmotid, roundingframe, 0);
				}
				else {
					firsthipbvhmat.SetIdentity();
				}
				if (srcbone->GetParent()) {
					firsthipmodelmat = srcbone->GetParent()->GetWorldMat(false, modelmotid, roundingframe, 0);
				}
				else {
					firsthipmodelmat.SetIdentity();
				}

				//firsthipbvhmat = bvhbone->GetWorldMat(false, bvhmotid, roundingframe, 0);
				//firsthipmodelmat = srcbone->GetWorldMat(false, modelmotid, roundingframe, 0);



				//curbvhmat = bvhbone->GetInvFirstMat() * invmodelinit * bvhmat;
				//curbvhmat = bvhbone->GetInvFirstMat() * sinvfirsthipmat * invmodelinit * bvhmat;
				//curbvhmat = sinvfirsthipmat * bvhbone->GetInvFirstMat() * sfirsthipmat * invmodelinit * bvhmat;//1.0.0.26�ɂȂ�O�܂ł̎��B�����p���̕ϊ���bvh�̑S�̉�]sfirsthipmat���l������B

				//#############################################################################################################################
				//1.0.0.26�����
				//bvh�͓ǂݍ��ݎ��ɂO�t���[���A�j����Identity�ɂȂ�悤�ɓǂݍ��ށBmodel����InvJonitPos * AnimMat�̂悤�ɓǂݍ��ނ悤�ɂ����B
				//model���͂O�t���[���ҏW�ɑΉ������B
				//�ȏ�̕ύX�ɑΉ����邽�߂�retarget�̐������C���B
				//#############################################################################################################################

				//###################################################################################################################
				//1.0.0.27����͂O�t���[���A�j���̕ҏW�ɑΉ��B
				//�O�t���[���ɑΉ��\�Ȃ͔̂�bvh�̃��f���B��bvh�̏ꍇ�A�O�t���[���A�j����Identity�ɂȂ�悤�ɂ͓ǂ܂Ȃ��B
				//��bvh�̏ꍇ�ɂ�BindPose��0�t���[���A�j���̗��������݂���B����ĂO�t���[���A�j���̕ҏW�����ď����o���Ă�����B
				//����Abvh�̏ꍇ�A�O�t���[���A�j����Identity�ɂȂ�悤�ɓǂݍ��ށB���̂��߃��^�[�Q�b�g�̐������ȗ��������B
				//###################################################################################################################
				//curbvhmat = sinvfirsthipmat * srcbone->GetFirstMat() * sfirsthipmat * invmodelinit * bvhmat;//��10027_1 ���܂��s��

				////####################################################################################
				////��10027_1�̍s��|���Z�������N�H�[�^�j�I���ɂ��ăW���o�����b�N���N����ɂ������Ă݂�
				////####################################################################################


				//FirstMat�ɂ���
				//SetFirstMat�́@CBone::InitMP�@�ōs���BInitMP��CModel::AddMotion����Ă΂��B
				//InitMP�͍ŏ��̃��[�V�����̂O�t���[���A�j���ŐV�K���[�V�����̑S�t���[��������������B


				////##############################################################################################################################
				////��10032(1033��)  bvh���̂O�t���[���Ή���model���̂O�t���[���Ή����C�����ā@���́I�I
				//// �O��P�F���^�[�Q�b�g������ bvh����model���̌�������̃|�[�Y�������ł��邱��
				//// �O��Q�Fbvh���͂O�t���[���p����identity(�O�t���[���ɃA�j�����t����ꍇ�̓W���C���g�ʒu�ɗ��Ƃ����ݎp����identity). 
				//// �O��R�Fmodel���͂O�t���[���ɃA�j���������c���Ă���
				//// �O��Q�ƑO��R�ɂ��Ắ@fbx�̓ǂݍ��ݕ������̂悤�ɂ��Ă���(bvh���ɂ̓o�C���h�|�[�Y���������Ƃ��������炱�̂悤�ɂ��Ă���)
				////##############################################################################################################################

				if (modelfirstbone && bvhfirstbone) {

					//#######################################################################################
					//2022/12/21 ver1.1.0.10�֌�����
					//��10033�ƑO����������킹��
					//bvh����0�t���[���p����Identity�ɂȂ�悤�Ɂ@InvFirstMat * NodeMat ���|����
					//#######################################################################################
					ChaMatrix offsetforbvhmat, offsetformodelmat;
					//offsetforbvhmat = ChaMatrixInv(bvhbone->GetFirstMat()) * bvhbone->GetNodeMat();
					//offsetforbvhmat = ChaMatrixInv((ChaMatrixInv(bvhbone->GetNodeMat()) * bvhbone->GetFirstMat()));
					//offsetformodelmat.SetIdentity();
					//offsetformodelmat = ChaMatrixInv((ChaMatrixInv(srcbone->GetNodeMat()) * srcbone->GetFirstMat()));
					
					
					//offsetforbvhmat = bvhbone->GetNodeMat() * ChaMatrixInv(bvhbone->GetWorldMat(false, bvhmotid, 0.0, 0));
					//offsetformodelmat = srcbone->GetNodeMat() * ChaMatrixInv(srcbone->GetWorldMat(false, modelmotid, 0.0, 0));
					offsetforbvhmat = ChaMatrixInv(bvhbone->GetWorldMat(false, bvhmotid, 0.0, 0));
					offsetformodelmat = ChaMatrixInv(srcbone->GetWorldMat(false, modelmotid, 0.0, 0));

					//######
					//model
					//######
						//model firsthip
					ChaMatrix firsthipmodelS, firsthipmodelR, firsthipmodelT;
					ChaMatrix invfirsthipmodelS, invfirsthipmodelR, invfirsthipmodelT;
					CQuaternion firsthipmodelQ, invfirsthipmodelQ;
					GetSRTMatrix2(firsthipmodelmat, &firsthipmodelS, &firsthipmodelR, &firsthipmodelT);
					GetSRTMatrix2(ChaMatrixInv(firsthipmodelmat), &invfirsthipmodelS, &invfirsthipmodelR, &invfirsthipmodelT);
					firsthipmodelQ.RotationMatrix(firsthipmodelR);
					invfirsthipmodelQ.RotationMatrix(invfirsthipmodelR);

					//model firstframe globalposition
					//ChaMatrix invfirstmodelS, invfirstmodelR, invfirstmodelT;
					//CQuaternion invfirstmodelQ;
					//GetSRTMatrix2(ChaMatrixInv(bvhbone->GetFirstMat()), &invfirstmodelS, &invfirstmodelR, &invfirstmodelT);
					//invfirstmodelQ.RotationMatrix(invfirstmodelR);

					//model current
					ChaMatrix invmodelcurrentmat;
					ChaMatrix invmodelS, invmodelR, invmodelT;
					CQuaternion invmodelQ;
					//invmodelcurrentmat = ChaMatrixInv(srcbone->GetNodeMat() * modelmp.GetWorldMat());
					invmodelcurrentmat = ChaMatrixInv(offsetformodelmat * modelmp.GetWorldMat());
					GetSRTMatrix2(invmodelcurrentmat, &invmodelS, &invmodelR, &invmodelT);
					invmodelQ.RotationMatrix(invmodelR);

					//model zeroframe anim
					ChaMatrix zeroframemodelmat;
					CQuaternion zeroframemodelQ;
					//zeroframemodelmat = srcbone->GetNodeMat() * srcbone->GetCurrentZeroFrameMat(1);
					zeroframemodelmat = offsetformodelmat * srcbone->GetCurrentZeroFrameMat(limitdegflag, 1);
					zeroframemodelQ.RotationMatrix(zeroframemodelmat);


					//######
					//bvh
					//######
						//bvh firsthip
					ChaMatrix firsthipbvhS, firsthipbvhR, firsthipbvhT;
					ChaMatrix invfirsthipbvhS, invfirsthipbvhR, invfirsthipbvhT;
					CQuaternion firsthipbvhQ, invfirsthipbvhQ;
					GetSRTMatrix2(firsthipbvhmat, &firsthipbvhS, &firsthipbvhR, &firsthipbvhT);
					GetSRTMatrix2(ChaMatrixInv(firsthipbvhmat), &invfirsthipbvhS, &invfirsthipbvhR, &invfirsthipbvhT);
					firsthipbvhQ.RotationMatrix(firsthipbvhR);
					invfirsthipbvhQ.RotationMatrix(invfirsthipbvhR);

					////bvh firstframe globalposition
					ChaMatrix firstbvhS, firstbvhR, firstbvhT;
					CQuaternion firstbvhQ;
					ChaMatrix invfirstbvhS, invfirstbvhR, invfirstbvhT;
					CQuaternion invfirstbvhQ;
					GetSRTMatrix2(bvhbone->GetFirstMat(), &firstbvhS, &firstbvhR, &firstbvhT);
					firstbvhQ.RotationMatrix(firstbvhR);
					GetSRTMatrix2(ChaMatrixInv(bvhbone->GetFirstMat()), &invfirstbvhS, &invfirstbvhR, &invfirstbvhT);
					invfirstbvhQ.RotationMatrix(invfirstbvhR);

					////bvh zeroframe anim
					//ChaMatrix zeroframebvhmat;
					//CQuaternion zeroframebvhQ;
					//zeroframebvhmat = bvhbone->GetCurrentZeroFrameMat(1);
					//zeroframebvhQ.RotationMatrix(zeroframebvhmat);
					//bvh invzeroframe anim
					ChaMatrix zeroframebvhmat;
					ChaMatrix invzeroframebvhmat;
					CQuaternion invzeroframebvhQ;
					zeroframebvhmat = offsetforbvhmat * bvhbone->GetCurrentZeroFrameMat(limitdegflag, 1);
					//invzeroframebvhmat = ChaMatrixInv(bvhbone->GetNodeMat() * bvhbone->GetCurrentZeroFrameMat(1));
					invzeroframebvhmat = ChaMatrixInv(zeroframebvhmat);
					invzeroframebvhQ.RotationMatrix(invzeroframebvhmat);

					//bvh current
					ChaMatrix bvhcurrentmat, invbvhcurrentmat;
					ChaMatrix bvhS, bvhR, bvhT;
					CQuaternion bvhQ;
					//bvhcurrentmat = bvhbone->GetNodeMat() * bvhmp.GetWorldMat();
					bvhcurrentmat = offsetforbvhmat * bvhmp.GetAnimMat();
					invbvhcurrentmat = ChaMatrixInv(bvhcurrentmat);
					GetSRTMatrix2(bvhcurrentmat, &bvhS, &bvhR, &bvhT);
					bvhQ.RotationMatrix(bvhR);
					////bvh inv current
					//ChaMatrix invbvhS, invbvhR, invbvhT;
					//CQuaternion invbvhQ;
					//GetSRTMatrix2(ChaMatrixInv(bvhmp.GetWorldMat()), &invbvhS, &invbvhR, &invbvhT);
					//invbvhQ.RotationMatrix(invbvhR);


				//10033�����̎�
					//ChaMatrix curbvhmat;
					//curbvhmat =
					//	(ChaMatrixInv(firsthipbvhmat) * ChaMatrixInv(bvhbone->GetCurrentZeroFrameMat(1)) * firsthipbvhmat) *
					//	(ChaMatrixInv(firsthipmodelmat) * (ChaMatrixInv(modelmp.GetWorldMat()) * zeroframemodelmat) * firsthipmodelmat) *
					//	bvhmp.GetWorldMat();//2022/10/30 �e�X�g(bvh120, bvh121, Rokoko)�ρ@OK
					//
					//�⑫�Finvhips * (inv)zeroframemat * hips�́@model���W�n�Ƃ�����hips���W�n��zeroframe�p���̌v�Z

				//��10033 �ȉ��U�s
					ChaMatrix curbvhmat;
					//CQuaternion convQ;
					//convQ = bvhQ *
					//	(invfirsthipmodelQ * (zeroframemodelQ * invmodelQ) * firsthipmodelQ) *
					//	(invfirsthipbvhQ * invzeroframebvhQ * firsthipbvhQ);
					//curbvhmat = convQ.MakeRotMatX();//2022/12/21 �e�X�g(bvh121, Rokoko�o�C���h�|�[�Y����, Rokoko�o�C���h�|�[�Y�L��)�ρ@OK
					//��10033
					//2022/10/30�e�X�g�̎����N�H�[�^�j�I��(�y�уN�H�[�^�j�I���̊|���Z�̏���)�ɂ��ā@�W���o�����b�N���N�����悤��


					curbvhmat = offsetformodelmat * ChaMatrixInv(offsetforbvhmat) * bvhmp.GetWorldMat();



					rotq.RotationMatrix(curbvhmat);//��]�����̗p����


					//traanim = bvhbone->CalcLocalTraAnim(bvhmotid, roundingframe);//�ړ��͂����炩��擾
					//if (!bvhbone->GetParent()) {
					//	ChaVector3 bvhbonepos = bvhbone->GetJointFPos();
					//	ChaVector3 firstframebonepos = bvhbone->GetFirstFrameBonePos();
					//	ChaVector3 firstdiff = firstframebonepos - bvhbonepos;
					//	traanim -= firstdiff;
					//}
					//traanim = traanim * hrate;




					//################################################################################
					//2023/01/08
					//Hips�W���C���g�ȊO��TraAnim���L����
					// 
					//���^�[�Q�b�g�����́@model��bvh�̂O�t���[���̌�������̎p�����������Ƃł��邩��
					//���^�[�Q�b�g����TraAnim�Ƃ��Čv�Z���ׂ��́@�O�t���[������̕ω����ł���
					//################################################################################

					ChaMatrix bvhsmat, bvhrmat, bvhtmat, bvhtanimmat;
					ChaMatrix bvhsmat0, bvhrmat0, bvhtmat0, bvhtanimmat0;

					//GetWorldMat() : limitedflag���[���ɂ��Ă����K�v�L !!!!
					if (bvhbone->GetParent()) {
						ChaMatrix parentwm = bvhbone->GetParent()->GetWorldMat(limitdegflag, bvhmotid, roundingframe, 0);
						GetSRTandTraAnim(bvhmp.GetAnimMat() * ChaMatrixInv(parentwm), bvhbone->GetNodeMat(), 
							&bvhsmat, &bvhrmat, &bvhtmat, &bvhtanimmat);

						//calc 0 frame
						ChaMatrix parentwm0 = bvhbone->GetParent()->GetWorldMat(limitdegflag, bvhmotid, 0.0, 0);
						GetSRTandTraAnim(bvhbone->GetWorldMat(limitdegflag, bvhmotid, 0.0, 0) * ChaMatrixInv(parentwm0), bvhbone->GetNodeMat(),
							&bvhsmat0, &bvhrmat0, &bvhtmat0, &bvhtanimmat0);
					}
					else {
						GetSRTandTraAnim(bvhmp.GetAnimMat(), bvhbone->GetNodeMat(), 
							&bvhsmat, &bvhrmat, &bvhtmat, &bvhtanimmat);

						//calc 0 frame
						GetSRTandTraAnim(bvhbone->GetWorldMat(limitdegflag, bvhmotid, 0.0, 0), bvhbone->GetNodeMat(),
							&bvhsmat0, &bvhrmat0, &bvhtmat0, &bvhtanimmat0);
					}
					//traanim = ChaMatrixTraVec(bvhtanimmat);
					traanim = ChaMatrixTraVec(bvhtanimmat) - ChaMatrixTraVec(bvhtanimmat0);//2023/01/08
					traanim = traanim * hrate;
				}
				else {
					rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
					traanim = ChaVector3(0.0f, 0.0f, 0.0f);
				}
			}
			else {
				rotq.SetParams(1.0f, 0.0f, 0.0f, 0.0f);
				traanim = ChaVector3(0.0f, 0.0f, 0.0f);
			}

			bool onretarget = true;
			if (bvhbone) {
				srcmodel->FKRotate(limitdegflag, onretarget, 1, bvhbone, 1, traanim, roundingframe, curboneno, rotq);
			}
			else {
				srcmodel->FKRotate(limitdegflag, onretarget, 0, befbvhbone, 0, traanim, roundingframe, curboneno, rotq);
			}
		}

		return 0;
	}

}
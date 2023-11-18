#ifndef DISPOBJH
#define DISPOBJH

#include <ChaVecCalc.h>


class CMQOObject;
class CMQOMaterial;
class CPolyMesh3;
class CPolyMesh4;
class CExtLine;


class ConstantBuffer;//���b�V�����ʂ̒萔�o�b�t�@�B
class StructuredBuffer;//�{�[���s��̍\�����o�b�t�@�B
class DescriptorHeap;//�f�B�X�N���v�^�q�[�v�B


struct SConstantBuffer {
	Matrix mWorld;		//���[���h�s��B
	Matrix mView;		//�r���[�s��B
	Matrix mProj;		//�v���W�F�N�V�����s��B
};



class CDispObj
{
public:

/**
 * @fn
 * CDispObj
 * @breaf �R���X�g���N�^�B
 * @return �Ȃ��B
 */
	CDispObj();

/**
 * @fn
 * ~CDispObj
 * @breaf �f�X�g���N�^�B
 * @return �Ȃ��B
 */
	~CDispObj();

/**
 * @fn
 * CreateDispObj
 * @breaf ���^�Z�R�C�A����̃f�[�^�p�̕\���p�I�u�W�F�N�g���쐬����B
 * @param (ID3D12Device* pdev) IN Direct3D�̃f�o�C�X�B
 * @param (CPolyMesh3* pm3) IN �`��f�[�^�ւ̃|�C���^�B
 * @param (int hasbone) IN �{�[���ό`�L�̏ꍇ�͂P�A�����̏ꍇ�͂O�BMameBake3D�ł̓��^�Z�R�C�A�f�[�^�̓{�[�������B
 * @return ����������O�B
 */
	int CreateDispObj( ID3D12Device* pdev, CPolyMesh3* pm3, int hasbone );


/**
 * @fn
 * CreateDispObj
 * @breaf FBX����̃f�[�^�p�̕\���p�I�u�W�F�N�g���쐬����B
 * @param (ID3D12Device* pdev) IN Direct3D�̃f�o�C�X�B
 * @param (CPolyMesh4* pm4) IN �`��f�[�^�ւ̃|�C���^�B
 * @param (int hasbone) IN �{�[���ό`�L�̏ꍇ�͂P�A�����̏ꍇ�͂O�B * @return �߂�l�̐���
 * @return ����������O�B
 */
	int CreateDispObj( ID3D12Device* pdev, CPolyMesh4* pm4, int hasbone );

/**
 * @fn
 * CreateDispObj
 * @breaf �����f�[�^�p�̕\���p�I�u�W�F�N�g���쐬����B
 * @param (ID3D12Device* pdev) IN Direct3D�̃f�o�C�X�B
 * @param (CExtLine* extline) IN ���̌`��f�[�^�ւ̃|�C���^�B
 * @return ����������O�B
 */
	int CreateDispObj( ID3D12Device* pdev, CExtLine* extline );


/**
 * @fn
 * RenderNormal
 * @breaf �ʏ�̃x�[�V�b�N�ȕ��@�łRD�`�������B
 * @param (CMQOMaterial* rmaterial) IN �ގ��ւ̃|�C���^�B
 * @param (int lightflag) IN �ƌ���������Ƃ��͂P�A���Ȃ��Ƃ��͂O�B
 * @param (ChaVector4 diffusemult) IN �f�B�t���[�Y�F�Ɋ|���Z����䗦�B
 * @return ����������O�B
 */
	int RenderNormal(bool withalpha, 
		RenderContext& rc, int lightflag, 
		ChaVector4 diffusemult, ChaVector4 materialdisprate, CMQOObject* pmqoobj, Matrix mWorld);

	int RenderNormalMaterial(bool laterflag, bool withalpha,
		RenderContext& rc,
		CMQOMaterial* rmaterial, int curoffset, int curtrinum,
		int lightflag, ChaVector4 diffusemult, ChaVector4 materialdisprate);

	void DrawCommon(RenderContext& rc, const Matrix& mWorld, const Matrix& mView, const Matrix& mProj);

/**
 * @fn
 * RenderNormalPM3
 * @breaf �ʏ�̃x�[�V�b�N�ȕ��@�Ń��^�Z�R�C�A�f�[�^���RD�\������B
 * @param (int lightflag) IN �ƌ���������Ƃ��͂P�A���Ȃ��Ƃ��͂O�B
 * @param (ChaVector4 diffusemult) IN �f�B�t���[�Y�F�Ɋ|���Z����䗦�B
 * @return ����������O�B
 * @detail FBX�f�[�^�͂P�I�u�W�F�N�g�ɂ��P�}�e���A��(�ގ�)�����A���^�Z�R�C�A�f�[�^�͂P�I�u�W�F�N�g�ɕ����}�e���A�����ݒ肳��Ă��邱�Ƃ������B
 */
	int RenderNormalPM3(bool withalpha, 
		RenderContext& rc, int lightflag,
		ChaVector4 diffusemult, ChaVector4 materialdisprate, CMQOObject* pmqoobj, Matrix mWorld);

	int RenderNormalPM3Material(bool laterflag, bool withalpha,
		RenderContext& rc,
		CMQOMaterial* rmaterial, int curoffset, int curtrinum,
		int lightflag, ChaVector4 diffusemult, ChaVector4 materialdisprate);


/**
 * @fn
 * RenderLine
 * @breaf �����f�[�^��\������B
 * @param (ChaVector4 diffusemult) IN �f�B�t���[�Y�F�Ɋ|���Z����䗦�B
 * @return ����������O�B
 */
	int RenderLine(bool withalpha,
		RenderContext* pRenderContext, 
		ChaVector4 diffusemult, ChaVector4 materialdisprate);

/**
 * @fn
 * CopyDispV
 * @breaf FBX�̒��_�f�[�^��\���o�b�t�@�ɃR�s�[����B
 * @param (CPolyMesh4* pm4) IN FBX�̌`��f�[�^�B
 * @return ����������O�B
 */
	int CopyDispV( CPolyMesh4* pm4 );

/**
 * @fn
 * CopyDispV
 * @breaf ���^�Z�R�C�A�̒��_�f�[�^��\���o�b�t�@�ɃR�s�[����B
 * @param (CPolyMesh3* pm3) IN ���^�Z�R�C�A�̌`��f�[�^�B
 * @param (������) �����̐���
 * @return ����������O�B
 */
	int CopyDispV( CPolyMesh3* pm3 );


	void SetScale(ChaVector3 srcscale, ChaVector3 srcoffset)
	{
		m_scale = srcscale;
		m_scaleoffset = srcoffset;
	};

	void UpdateBoneMatrix(int srcdatanum, void* srcdata);

private:

/**
 * @fn
 * InitParams
 * @breaf �����o������������B
 * @return �O�B
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf �A���P�[�g�������������J������B
 * @return �O�B
 */
	int DestroyObjs();

/**
 * @fn
 * CreateDecl
 * @breaf �\���p���_�f�[�^�̃t�H�[�}�b�g(�錾)���쐬����B
 * @return ����������O�B
 */
	int CreateDecl(ID3D12Device* pdev);

/**
 * @fn
 * CreateVBandIB
 * @breaf �RD�\���p�̒��_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@���쐬����B
 * @return ����������O�B
 */
	int CreateVBandIB(ID3D12Device* pdev);

/**
 * @fn
 * CreateVBandIBLine
 * @breaf �����p�̒��_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@���쐬����B
 * @return ����������O�B
 */
	int CreateVBandIBLine(ID3D12Device* pdev);


	void CreateDescriptorHeaps();

private:
	//�g��SRV���ݒ肳��郌�W�X�^�̊J�n�ԍ��B
	const int EXPAND_SRV_REG__START_NO = 10;
	//const int EXPAND_SRV_REG__START_NO = 4;
	//�P�̃}�e���A���Ŏg�p�����SRV�̐��B
	const int NUM_SRV_ONE_MATERIAL = EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV;
	//�P�̃}�e���A���Ŏg�p�����CBV�̐��B
	const int NUM_CBV_ONE_MATERIAL = 2;

	int m_hasbone;//�{�[���ό`�p�̃I�u�W�F�N�g�ł���Ƃ��P�A����ȊO�̎��͂O�B

	ID3D12Device* m_pdev;//�O���������ADirect3D�̃f�o�C�X�B
	CPolyMesh3* m_pm3;//�O���������A���^�Z�R�C�A�t�@�C������쐬�����RD�f�[�^�B
	CPolyMesh4* m_pm4;//�O���������AFBX�t�@�C������쐬�����RD�f�[�^�B
	CExtLine* m_extline;//�O���������A���f�[�^�B

	//D3D11_BUFFER_DESC m_BufferDescBone;
	//D3D11_BUFFER_DESC m_BufferDescNoBone;
	//D3D11_BUFFER_DESC m_BufferDescInf;
	//D3D11_BUFFER_DESC m_BufferDescLine;

	//ID3D11InputLayout* m_layoutBoneL0;
	//ID3D11InputLayout* m_layoutBoneL1;
	//ID3D11InputLayout* m_layoutBoneL2;
	//ID3D11InputLayout* m_layoutBoneL3;
	//ID3D11InputLayout* m_layoutBoneL4;
	//ID3D11InputLayout* m_layoutBoneL5;
	//ID3D11InputLayout* m_layoutBoneL6;
	//ID3D11InputLayout* m_layoutBoneL7;
	//ID3D11InputLayout* m_layoutBoneL8;
	//ID3D11InputLayout* m_layoutNoBoneL0;
	//ID3D11InputLayout* m_layoutNoBoneL1;
	//ID3D11InputLayout* m_layoutNoBoneL2;
	//ID3D11InputLayout* m_layoutNoBoneL3;
	//ID3D11InputLayout* m_layoutNoBoneL4;
	//ID3D11InputLayout* m_layoutNoBoneL5;
	//ID3D11InputLayout* m_layoutNoBoneL6;
	//ID3D11InputLayout* m_layoutNoBoneL7;
	//ID3D11InputLayout* m_layoutNoBoneL8;
	//ID3D11InputLayout* m_layoutLine;

 //   ID3D11Buffer* m_VB;//�\���p���_�o�b�t�@�B
	//ID3D11Buffer* m_InfB;//�\���p�{�[���e���x�o�b�t�@�B
	//ID3D11Buffer* m_IB;//�\���p�O�p�̃C���f�b�N�X�o�b�t�@�B


	ID3D12Resource* m_vertexBuffer;		//���_�o�b�t�@�B
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;	//���_�o�b�t�@�r���[�B

	ID3D12Resource* m_indexBuffer;	//�C���f�b�N�X�o�b�t�@�B
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;	//�C���f�b�N�X�o�b�t�@�r���[�B


	ConstantBuffer m_commonConstantBuffer;					//���b�V�����ʂ̒萔�o�b�t�@�B
	ConstantBuffer m_expandConstantBuffer;					//���[�U�[�g���p�̒萔�o�b�t�@
	std::array<IShaderResource*, MAX_MODEL_EXPAND_SRV> m_expandShaderResourceView = { nullptr };	//���[�U�[�g���V�F�[�_�[���\�[�X�r���[�B
	void* m_expandData = nullptr;
	StructuredBuffer m_boneMatricesStructureBuffer;	//�{�[���s��̍\�����o�b�t�@�B
	//std::vector< SMesh* > m_meshs;						//���b�V���B
	bool m_createdescriptorflag;
	//////std::vector< DescriptorHeap > m_descriptorHeap;	//�f�B�X�N���v�^�q�[�v�B
	DescriptorHeap m_descriptorHeap;					//�f�B�X�N���v�^�q�[�v�B
	//Skeleton* m_skeleton = nullptr;						//�X�P���g���B
	//void* m_expandData = nullptr;						//���[�U�[�g���f�[�^�B


	//Shader�̃|�C���g��new�����ꍇ��ShaderBank�Ɋi�[����
	//Shader�̔j����ShaderBank���s���̂Ł@CDispObj�̃f�X�g���N�^�ł͔j�����Ȃ�
	Shader* m_vsNonSkinModel;//�X�L���Ȃ����f���p�̒��_�V�F�[�_�[�B
	Shader* m_vsSkinModel;//�X�L�����胂�f���p�̒��_�V�F�[�_�[�B
	Shader* m_psModel;//���f���p�̃s�N�Z���V�F�[�_�[�B




	ChaVector3 m_scale;
	ChaVector3 m_scaleoffset;

	int* m_tmpindexLH;
};

#endif
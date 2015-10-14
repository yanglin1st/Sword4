#pragma once


#include "KG3DInterface.h"

enum EKG3DPROCESSOR
{
	PROCESSOR_CULLER_STAND,
	PROCESSOR_CULLER_BOX,
	PROCESSOR_CULLER_RECT,
	PROCESSOR_RENDER,
	PROCESSOR_SOUND,
	PROCESSOR_POSTRENDER,
	PROCESSOR_PRESENT,
};

interface IKG3DCullResult; //�ü�������ƶ���

interface IKG3DSceneManger;//����������
interface IEKG3DSceneManger;
interface IKG3DSceneItem;

interface IKG3DProcessor
{
	virtual BOOL CanDoMultiThread() = 0;//�Ƿ���Զ��߳�
	virtual HRESULT DoProcess() = 0;
	virtual EKG3DPROCESSOR GetType() = 0;
};

interface IKG3DCamera;
interface IKG3DCuller : public IKG3DProcessor//�ü���
{
	virtual HRESULT SetInputFromScene(IKG3DSceneManger* pScene) = 0;
	virtual HRESULT SetInputFromCullResult(IKG3DCullResult* pInput) = 0;
	virtual HRESULT SetCamera(IKG3DCamera* pCamera) = 0;

	virtual HRESULT SetOutput(IKG3DCullResult* pOutput) = 0; 
};

interface IKG3DSceneNode;

interface IKG3DCullResult //�ü�������ƶ���
{
	virtual HRESULT StartInputData() = 0;
	virtual HRESULT EndInputData() = 0;

	virtual HRESULT InputSceneNode(IKG3DSceneNode* pNode) = 0;

	virtual HRESULT InputSceneItem(IKG3DSceneItem* pBox) = 0;
};

interface IKG3DCullResultProcesor : public IKG3DProcessor//�ü����������
{
	virtual HRESULT SetData(IKG3DCullResult* pResult) = 0;
};

interface IKG3DDepthStencilBuffer //���ͼ
{
	virtual HRESULT AddRef() = 0;
	virtual HRESULT Release() = 0;

	virtual LPDIRECT3DTEXTURE9 GetParentTexture() = 0;
	virtual BOOL IsBackBuffer() = 0;

};

interface IKG3DRenderTarget //��ɫͼ
{
	virtual HRESULT AddRef() = 0;
	virtual HRESULT Release() = 0;

	virtual LPDIRECT3DTEXTURE9 GetParentTexture() = 0;
	virtual BOOL IsBackBuffer() = 0;
};

interface IKG3DBufferFactory//ͼƬ���๤��
{
	virtual HRESULT GetBackBuffer(IKG3DRenderTarget** ppRenderTarget) = 0;
	virtual HRESULT GetDepthStencilBuffer(IKG3DDepthStencilBuffer** ppDepth) = 0;

	virtual HRESULT CreateRenderTarget(IKG3DRenderTarget** ppRenderTarget,int nWidth,int nHeight,D3DFORMAT fmt,BOOL bTexture) = 0;
	virtual HRESULT CreateDepthStencilBuffer(IKG3DDepthStencilBuffer** ppDepth,int nWidth,int nHeight,D3DFORMAT fmt,BOOL bTexture) = 0;
	virtual HRESULT DeleteRenderTarget(IKG3DRenderTarget* pRenderTarget) = 0;
	virtual HRESULT DeleteDepthStencilBuffer(IKG3DDepthStencilBuffer* pDepth) = 0;

};

interface IKG3DProcessorFactory//�������๤��
{
	virtual HRESULT AddProcessor(IKG3DProcessor** ppNewProcessor,EKG3DPROCESSOR eType) = 0;
	virtual HRESULT DeleteProcessor(IKG3DProcessor* pProcessor) = 0;
};

interface IKG3DSoundTool : public IKG3DCullResultProcesor//��Ч������ 
{
	;
};

interface IKG3DRenderTool : public IKG3DCullResultProcesor//������
{
	virtual HRESULT SetDepthStencilBuffer(IKG3DDepthStencilBuffer* pDepth) = 0;
	virtual HRESULT SetRenderTarget(int nIndex,IKG3DRenderTarget* pTarget) = 0;
	virtual HRESULT SetNumRenderTarget(int nNum) = 0;
};

interface IKG3DPostRender : public IKG3DProcessor
{
	;
};

interface IKG3DPresentTool : public IKG3DProcessor
{
	virtual HRESULT SetOutputWnd(HWND) = 0;
};

interface IKG3DRenderPass //����PASS
{
	virtual HRESULT AddProcessor(IKG3DProcessor** ppNewProcessor,EKG3DPROCESSOR eType) = 0;
	virtual HRESULT DeleteProcessor(IKG3DProcessor* pProcessor) = 0;
	
	virtual BOOL CanDoMultiThread() = 0;//�Ƿ���Զ��߳�

	virtual HRESULT Process() = 0;
};

interface IKG3DRenderFlow //һ����Ⱦ����
{
	virtual HRESULT AddRenderPass(IKG3DRenderPass** ppNewRenderPass,BOOL bMultiThread) = 0;//���ӻ���PASS
	virtual HRESULT DeleteRenderPass(IKG3DRenderPass* pRenderPass) = 0;//���ӻ���PASS

	virtual HRESULT SetOuputWnd(HWND hOutput) = 0;

	virtual HRESULT AddCullResult(IKG3DCullResult** ppResult) = 0;
	virtual HRESULT DeleteCullResult(IKG3DCullResult* pResult) = 0;

	virtual HRESULT LoadFromFile(LPSTR pFileName) = 0;
	virtual HRESULT SaveToFile(LPSTR pFileName) = 0;

	virtual HRESULT ProcessMainThread() = 0;//���߳����
	virtual HRESULT ProcessMultiThread(int nIndex) = 0;//���߳����

};

interface IKG3DModelRenderTool
{
	virtual HRESULT RenderSceneItem(IKG3DSceneItem* pActor) = 0;
};

interface IKG3DRenderFlowSystem//������Ⱦ����ϵͳ
{
	virtual HRESULT Init(LPDIRECT3DDEVICE9 pDevice,IKG3DModelRenderTool* pModelRenderTool) = 0;
	virtual HRESULT Uninit() = 0;

	virtual IKG3DBufferFactory* GetBufferFactory() = 0;

	virtual HRESULT AddRenderFlow(IKG3DRenderFlow** ppFlow) = 0;
	virtual HRESULT DeleteRenderFlow(IKG3DRenderFlow* pFlow) = 0;

	virtual HRESULT ProcessMainThread() = 0;//���߳����
	virtual HRESULT ProcessMultiThread(int nIndex) = 0;//���߳����

	virtual HRESULT FrameMove() = 0;

	virtual IKG3DSceneManger* NewSceneSpaceManager(DWORD dwType) = 0;
	virtual HRESULT DeleteSceneSpaceManager(IKG3DSceneManger* pMgr) = 0;

	virtual IEKG3DSceneManger* GetIESceneSpaceManager(IKG3DSceneManger* pMgr) = 0;
};

//////////////////////////////////////////////////////////////////////////
//���溯����KG3DENGINE��ʵ��
IKG3DRenderFlowSystem *g_NewRenderFlowSystem();//��ȡ��Ⱦ����ϵͳ
HRESULT g_DeleteRenderFlowSystem();//ɾ����Ⱦ����ϵͳ
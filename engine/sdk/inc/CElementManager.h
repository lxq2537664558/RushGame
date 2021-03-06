#pragma once
#include "CSceneClassDefine.h"
#include "SyncSystem.h"
#include "CSceneMallocObject.h"

namespace sqr
{
	class CElementNode : public CSceneMallocObject
	{
		friend class CElementManager;
		friend class CRegion;
	public:
		CElementNode(CElementManager* pParent , URegionID id);
		CElementManager*	GetParentManager(void);
		URegionID			GetID(void);
		bool				IsValid(void);
		void				SetValid(bool isValid);
		void				SetCoder(CElementCoder* pCoder);
		CElementCoder*		GetCoder(void);
	protected:
		virtual void Destroy(void);	
		bool DeCode(bool bSync);
	protected:
		virtual ~CElementNode();
		void	StopDeCode(void);
	protected:
		CElementManager*	m_pParentManager;
		CElementCoder*		m_pCoder;
		const URegionID		m_ID;
		bool				m_isValid;
	};

	inline CElementCoder* 
		CElementNode::GetCoder(void)
	{
		return m_pCoder;
	}

	inline CElementManager* 
		CElementNode::GetParentManager(void)
	{
		return m_pParentManager;
	}

	inline URegionID
		CElementNode::GetID(void)
	{
		return m_ID;
	}

	inline bool 
		CElementNode::IsValid(void)
	{
		return m_isValid;
	}

	inline void 
		CElementNode::SetValid(bool isValid)
	{
		m_isValid = isValid;
	}

	class CElementCoder 
		: public SyncCoder
		, public CSceneMallocObject
	{
		//DECLARE_REF();
	public:
		CElementCoder(): m_pLockNode(NULL)  { /*CLEAR_REF_COUNT();*/ };
		virtual ~CElementCoder(){};
		virtual bool CElementDeCode( CElementNode* pNode, bool bSync ) { return true; };
		virtual	void StopDeCode(CElementNode* pNode);
		virtual void SyncCode(CElementNode* pNode){};

	protected:
		bool			Lock(CElementNode* pNode);
		void			UnLock(void);
		CElementNode*	m_pLockNode;
	};

	//inline DEFAULT_GETREFCOUNT(CElementCoder);
	//inline DEFAULT_ADDREF(CElementCoder);
	//inline DEFAULT_RELEASEREF(CElementCoder);

	class CElementCoderEx : public CElementCoder
	{
	public:
		CElementCoderEx() {};
		virtual ~CElementCoderEx() {};
		virtual bool CElementDeCode( CElementNode* pNode, bool bSync );
		virtual	void StopDeCode(CElementNode* pNode);
		virtual bool AutoDeCode(void);
				void SyncCode(CElementNode* pNode);
		virtual void Clear(void);

	protected:
		struct DeCodeParams : public CSceneMallocObject
		{
			DeCodeParams():m_pNode(NULL)
			{
			}

			DeCodeParams(CElementNode* pNode)
				: m_pNode(pNode)
			{
			}

			CElementNode*		m_pNode;
		};
		typedef SList<DeCodeParams> DeCodeList;
		DeCodeList		m_listDeCodeNode;
		virtual	bool	_CElementDeCode( const DeCodeParams& Params ) = 0;

	};
	class CElementManager 
	{
		friend class CElementNode;
		typedef SMap<URegionID,CElementNode*> ElementPool; 
	public:
		CElementManager();
		virtual ~CElementManager();
		CElementCoder*	SetCoder( CElementCoder* pCoder );	//返回原来的Coder
		CElementCoder*	GetCoder(void);	
		
		CElementNode*	GetNode(URegionID ID);
		CElementNode*	CreateNode( URegionID ID );
		bool			DestroyNode( URegionID ID );

		virtual void	Destroy(void);
	protected:
		virtual CElementNode*	_CreateNode( URegionID ID ) = 0;	
	protected:
		ElementPool		m_ElePool;
		CElementCoder*	m_pCoder;
	};

	inline CElementCoder* CElementManager::GetCoder(void)
	{
		return m_pCoder;
	}

}
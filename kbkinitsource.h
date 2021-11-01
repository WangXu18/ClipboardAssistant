#ifndef __KBkInitSource__h__
#define __KBkInitSource__h__
//
///////////////////////////////////////////////////////////////////////////

#define IDR_DEF_EXTERNAL_SKIN       50000
#define IDR_DEF_EXTERNAL_STYLE      50001
#define IDR_DEF_EXTERNAL_DLG_CLOSE  50100

#define IDR_DEF_COMMON_SKIN      60000
#define IDR_DEF_COMMON_STYLE     60001
#define IDR_DEF_COMMON_STRING    60002
#define IDR_DEF_COMMON_COLOR    60003

class KBkInitSource
{
public:

	static KBkInitSource& Instance()
	{
		static KBkInitSource inst;

		return inst;
	}

	BOOL InitResource(const CString& strResourcePath)
	{
    ATL::CComCritSecLock<ATL::CComAutoCriticalSection> locker(lock_);

		if (!m_bInit)
		{
			if (!strResourcePath.IsEmpty())
			{
				BkResManager::SetResourcePath(strResourcePath);
			}

			BkPngPool::Reload();
			BkBmpPool::Reload();
			BkJpgPool::Reload();
			BkSkinPool::Reload();

			BkString::Load(IDR_DEF_COMMON_STRING);
			BkSkin::LoadSkins(IDR_DEF_COMMON_SKIN);
			BkStyle::LoadStyles(IDR_DEF_COMMON_STYLE);

		 	BkSkin::LoadSkins2(IDR_DEF_EXTERNAL_SKIN);
			BkStyle::LoadStyles2(IDR_DEF_EXTERNAL_STYLE);

			m_bInit = TRUE;
		}

		return TRUE;
	}

    BOOL LoadStyle(LPCSTR pszXml)
    {
      ATL::CComCritSecLock<ATL::CComAutoCriticalSection> locker(lock_);
      BkStyle::LoadStyles(pszXml);
    }

	int DoFunctionNull()
	{
		return 0;
	}

protected:

	KBkInitSource()
	{
		m_bInit		  = FALSE;
	}

private:
  ATL::CComAutoCriticalSection lock_;
	BOOL					m_bInit;
};


///////////////////////////////////////////////////////////////////////////
//
#endif // __KBkInitSource__h__
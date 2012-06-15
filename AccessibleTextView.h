// AccessibleTextView.h

#ifndef AccessibleTextView_h
#define AccessibleTextView_h

#include "TextView.h"
#include <Windows.h>
#include <OleAcc.h>

class AccessibleTextView : public IAccessible
{
public:
	AccessibleTextView( TextView& textView );

	// IUknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void** ppvObject );
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	// IDispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT *pctinfo );
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo );
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId );

	virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember,
	                                          REFIID riid,
	                                          LCID lcid,
	                                          WORD wFlags,
	                                          DISPPARAMS* pDispParams,
	                                          VARIANT* pVarResult,
	                                          EXCEPINFO* pExcepInfo,
	                                          UINT* puArgErr );

	// IAccessible
	virtual HRESULT STDMETHODCALLTYPE get_accParent( IDispatch** ppdispParent );

	virtual HRESULT STDMETHODCALLTYPE get_accChildCount( long* pcountChildren );

	virtual HRESULT STDMETHODCALLTYPE get_accChild( VARIANT varChild, IDispatch** ppdispChild );

	virtual HRESULT STDMETHODCALLTYPE get_accName( VARIANT varChild, BSTR* pszName );

	virtual HRESULT STDMETHODCALLTYPE get_accValue( VARIANT varChild, BSTR* pszValue );

	virtual HRESULT STDMETHODCALLTYPE get_accDescription( VARIANT varChild, BSTR* pszDescription );

	virtual HRESULT STDMETHODCALLTYPE get_accRole( VARIANT varChild, VARIANT* pvarRole );

	virtual HRESULT STDMETHODCALLTYPE get_accState( VARIANT varChild,VARIANT* pvarState );

	virtual HRESULT STDMETHODCALLTYPE get_accHelp( VARIANT varChild, BSTR* pszHelp );

	virtual HRESULT STDMETHODCALLTYPE get_accHelpTopic( BSTR* pszHelpFile, VARIANT varChild, long* pidTopic );

	virtual HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut( VARIANT varChild, BSTR* pszKeyboardShortcut );

	virtual HRESULT STDMETHODCALLTYPE get_accFocus( VARIANT* pvarChild );

	virtual HRESULT STDMETHODCALLTYPE get_accSelection( VARIANT* pvarChildren );

	virtual HRESULT STDMETHODCALLTYPE get_accDefaultAction( VARIANT varChild, BSTR* pszDefaultAction );

	virtual HRESULT STDMETHODCALLTYPE accSelect( long flagsSelect, VARIANT varChild );

	virtual HRESULT STDMETHODCALLTYPE accLocation( long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild );

	virtual HRESULT STDMETHODCALLTYPE accNavigate( long navDir, VARIANT varStart, VARIANT* pvarEndUpAt );

	virtual HRESULT STDMETHODCALLTYPE accHitTest( long xLeft, long yTop, VARIANT* pvarChild );

	virtual HRESULT STDMETHODCALLTYPE accDoDefaultAction( VARIANT varChild );

	virtual HRESULT STDMETHODCALLTYPE put_accName( VARIANT varChild, BSTR szName );

	virtual HRESULT STDMETHODCALLTYPE put_accValue( VARIANT varChild, BSTR szValue );

private:
	ULONG m_refCount;
	TextView& m_textView;
};

#endif

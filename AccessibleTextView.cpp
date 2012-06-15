// AccessibleTextView.cpp

#include "AccessibleTextView.h"

AccessibleTextView::AccessibleTextView( TextView& textView )
	: m_refCount( 1 )
	, m_textView( textView )
{
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::QueryInterface( REFIID riid, void** ppvObject )
{
	if ( ppvObject == 0 )
		return E_POINTER;

	if ( riid == __uuidof( IUnknown ) )
	{
		*ppvObject = static_cast<IUnknown*>( this );
	}
	else if ( riid == __uuidof( IDispatch ) )
	{
		*ppvObject = static_cast<IDispatch*>( this );
	}
	else if ( riid == __uuidof( IAccessible ) )
	{
		*ppvObject = static_cast<IDispatch*>( this );
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE AccessibleTextView::AddRef()
{
	return ++m_refCount;
}

ULONG STDMETHODCALLTYPE AccessibleTextView::Release()
{
	ULONG refCount = --m_refCount;

	if ( refCount == 0 )
		delete this;

	return refCount;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::GetTypeInfoCount( UINT* pctinfo )
{
	*pctinfo = 0;
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo )
{
	*ppTInfo = 0;
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId )
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::Invoke( DISPID dispIdMember,
                                                      REFIID riid,
                                                      LCID lcid,
                                                      WORD wFlags,
                                                      DISPPARAMS* pDispParams,
                                                      VARIANT* pVarResult,
                                                      EXCEPINFO* pExcepInfo,
                                                      UINT* puArgErr )
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accParent( IDispatch** ppdispParent )
{
	HWND parent = GetParent( m_textView.WindowHandle() );

	if ( parent == 0 )
	{
		*ppdispParent = 0;
		return S_FALSE;
	}

	return AccessibleObjectFromWindow( parent, OBJID_WINDOW, __uuidof( IDispatch ), reinterpret_cast<void**>( ppdispParent ) );
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accChildCount( long* pcountChildren )
{
	*pcountChildren = 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accChild( VARIANT varChild, IDispatch** ppdispChild )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
	{
		*ppdispChild = 0;
		return E_INVALIDARG;
	}

	return QueryInterface( __uuidof( IDispatch ), reinterpret_cast<void**>( ppdispChild ) );
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accName( VARIANT varChild, BSTR* pszName )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
	{
		*pszName = 0;
		return E_INVALIDARG;
	}

	// TODO: delegate to parent somehow?
	*pszName = SysAllocString( L"Text edit" );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accValue( VARIANT varChild, BSTR* pszValue )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
	{
		*pszValue = 0;
		return E_INVALIDARG;
	}

	*pszValue = SysAllocString( m_textView.GetText().c_str() );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accDescription( VARIANT varChild, BSTR* pszDescription )
{
	*pszDescription = 0;
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accRole( VARIANT varChild, VARIANT* pvarRole )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
	{
		pvarRole->vt = VT_EMPTY;
		return E_INVALIDARG;
	}

	pvarRole->vt   = VT_I4;
	pvarRole->lVal = ROLE_SYSTEM_TEXT;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accState( VARIANT varChild,VARIANT* pvarState )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
	{
		pvarState->vt = VT_EMPTY;
		return E_INVALIDARG;
	}

	pvarState->vt   = VT_I4;
	pvarState->lVal = ( GetFocus() == m_textView.WindowHandle() )
	                     ? STATE_SYSTEM_FOCUSED
	                     : STATE_SYSTEM_FOCUSABLE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accHelp( VARIANT varChild, BSTR* pszHelp )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
	{
		*pszHelp = 0;
		return E_INVALIDARG;
	}

	// TODO: localize somehow?
	*pszHelp = SysAllocString( L"Text editor" );
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accHelpTopic( BSTR* pszHelpFile, VARIANT varChild, long* pidTopic )
{
	*pszHelpFile = 0;
	*pidTopic    = 0;

	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
		return E_INVALIDARG;

	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accKeyboardShortcut( VARIANT varChild, BSTR* pszKeyboardShortcut )
{
	*pszKeyboardShortcut = 0;

	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
		return E_INVALIDARG;

	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accFocus( VARIANT* pvarChild )
{
	if ( GetFocus() == m_textView.WindowHandle() )
	{
		pvarChild->vt   = VT_I4;
		pvarChild->lVal = CHILDID_SELF;
	}
	else
	{
		pvarChild->vt = VT_EMPTY;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accSelection( VARIANT* pvarChildren )
{
	pvarChildren->vt = VT_EMPTY;
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::get_accDefaultAction( VARIANT varChild, BSTR* pszDefaultAction )
{
	*pszDefaultAction = 0;

	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
		return E_INVALIDARG;

	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::accSelect( long flagsSelect, VARIANT varChild )
{
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::accLocation( long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
		return E_INVALIDARG;

	RECT rect;
	GetWindowRect( m_textView.WindowHandle(), &rect );

	*pxLeft    = rect.left;
	*pyTop     = rect.top;
	*pcxWidth  = rect.right - rect.left;
	*pcyHeight = rect.bottom - rect.top;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::accNavigate( long navDir, VARIANT varStart, VARIANT* pvarEndUpAt )
{
	pvarEndUpAt->vt = VT_EMPTY;
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::accHitTest( long xLeft, long yTop, VARIANT* pvarChild )
{
	POINT point = { xLeft, yTop };
	if ( WindowFromPoint( point ) != m_textView.WindowHandle() )
	{
		pvarChild->vt = VT_EMPTY;
		return S_FALSE;
	}

	pvarChild->vt   = VT_I4;
	pvarChild->lVal = CHILDID_SELF;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::accDoDefaultAction( VARIANT varChild )
{
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::put_accName( VARIANT varChild, BSTR szName )
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AccessibleTextView::put_accValue( VARIANT varChild, BSTR szValue )
{
	if ( varChild.vt != VT_I4 || varChild.lVal != CHILDID_SELF )
		return E_INVALIDARG;

	m_textView.SetText( UTF16Ref( szValue, SysStringLen( szValue ) ) );
	return S_OK;
}

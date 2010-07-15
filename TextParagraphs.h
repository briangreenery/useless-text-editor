// TextParagraphs.h

#ifndef TextParagraphs_h
#define TextParagraphs_h

#include "TextParagraph.h"
#include "TextChange.h"
#include <Windows.h>
#include <list>

class TextDocument;
class DocumentReader;

class TextParagraphs
{
public:
	TextParagraphs( const TextDocument& );

	void Draw( HDC hdc, RECT rect, TextSelection );

	size_t LineStart( LONG y ) const;
	size_t LineEnd  ( LONG y ) const;

	POINT PointFromChar( size_t, bool advancing ) const;
	size_t CharFromPoint( POINT* ) const;

	int Height() const          { return m_height; }
	int VisualLineCount() const { return m_visualLineCount; }

	void ProcessTextChanges( TextChange );
	void WidthChanged();

	void Validate( HDC hdc, int maxWidth );

private:
	void TextInserted( size_t start, size_t count, DocumentReader& );
	void TextDeleted ( size_t start, size_t count );

	typedef std::list<TextParagraph> ParagraphList;
	typedef ParagraphList::iterator iterator;

	struct ParagraphInfo
	{
		size_t textStart;
		LONG   yStart;
	};

	enum BreakType { nextChar, nextWord, prevChar, prevWord };
	size_t FindBreak( size_t pos, BreakType type ) const;

	iterator ParagraphContaining( size_t pos, ParagraphInfo* ) const;
	iterator ParagraphContaining( LONG   y,   ParagraphInfo* ) const;

	iterator CreateAfter( iterator );
	iterator Delete( iterator );
	bool IsLastParagraph( iterator );

	static const int m_lineHeight = 20;
	HFONT m_font;
	SCRIPT_CACHE m_fontCache;

	const TextDocument& m_doc;

	int m_height;
	int m_visualLineCount;

	mutable ParagraphList m_paragraphs;
};

#endif

// TextParagraphs.cpp

#include "TextParagraphs.h"
#include "DocumentReader.h"
#include "TextDocument.h"
#include "LayoutEngine.h"
#include "Require.h"

#undef min
#undef max

TextParagraphs::TextParagraphs( const TextDocument& doc )
	: m_doc( doc )
	, m_height( m_lineHeight )
	, m_visualLineCount( 1 )
{
	NONCLIENTMETRICS ncm = { sizeof ncm };
	SystemParametersInfo( SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0 );
	ncm.lfMessageFont.lfHeight = 15;
	wcscpy_s( ncm.lfMessageFont.lfFaceName, L"Consolas" );

	m_font = CreateFontIndirect( &ncm.lfMessageFont );
	m_fontCache = 0;

	m_paragraphs.push_back( TextParagraph() );
	m_paragraphs.back().Validate();
}

void TextParagraphs::Draw( HDC hdc, RECT rect, TextSelection selection )
{
	if ( IsRectEmpty( &rect ) )
		return;

	SelectObject( hdc, m_font );
	SetBkMode( hdc, TRANSPARENT );

	POINT oldOrigin;
	GetWindowOrgEx( hdc, &oldOrigin );

	SelectionRanges topSelection;

	int yStart = 0;
	iterator it = m_paragraphs.begin();

	for ( ; it != m_paragraphs.end(); ++it )
	{
		if ( yStart + it->Height() >= rect.top )
			break;

		yStart += it->Height();
		selection.start -= std::min( it->Length(), selection.start );
		selection.end   -= std::min( it->Length(), selection.end   );
	}

	// FIXME: topSelection isn't right here. If we're starting somewhere in the middle, we need to get the previous selection.

	OffsetRect( &rect, 0, -yStart );

	for ( ; it != m_paragraphs.end(); ++it )
	{
		SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y - yStart, NULL );

		topSelection = it->Draw( hdc, rect, m_fontCache, selection, topSelection, IsLastParagraph( it ) );

		rect.top = it->Height();
		OffsetRect( &rect, 0, -rect.top );

		yStart += it->Height();
		selection.start -= std::min( it->Length(), selection.start );
		selection.end   -= std::min( it->Length(), selection.end   );

		if ( IsRectEmpty( &rect ) )
			break;
	}

	SetWindowOrgEx( hdc, oldOrigin.x, oldOrigin.y, NULL );
}

void TextParagraphs::Validate( HDC hdc, int maxWidth )
{
	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );

	LayoutEngine layoutEngine( m_font, m_fontCache, hdc, maxWidth, 4 * tm.tmAveCharWidth );
	DocumentReader reader( m_doc );

	m_height          = 0;
	m_visualLineCount = 0;
	size_t start      = 0;

	for ( iterator it = m_paragraphs.begin(); it != m_paragraphs.end(); ++it )
	{
		if ( !it->Valid() )
		{
			bool endsWithNewline = ( it->Length() > 0 ) && ( m_doc.CharAt( start + it->Length() - 1 ) == 0x0A );

			if ( it->Length() == 0 || it->Length() == 1 && endsWithNewline )
			{
				it->Validate();
			}
			else
			{
				size_t lengthWithoutNewline = endsWithNewline
				                                 ? it->Length() - 1
				                                 : it->Length();

				StyleRun dummyStyles[2];
				dummyStyles[0].start = 0;
				dummyStyles[1].start = lengthWithoutNewline;

				layoutEngine.LayoutParagraph( reader.StrictRange( start, lengthWithoutNewline ), ArrayOf<StyleRun>( dummyStyles, 1 ) );
				it->Validate( layoutEngine.Result() );
			}
		}

		m_height          += it->Height();
		m_visualLineCount += it->LineCount();
		start             += it->Length();
	}
}

void TextParagraphs::ProcessTextChanges( TextChange change )
{
	DocumentReader reader( m_doc );

	switch ( change.type )
	{
	case TextChange::insertion:
		TextInserted( change.pos, change.count, reader );
		break;

	case TextChange::deletion:
		TextDeleted( change.pos, change.count );
		break;
	}
}

void TextParagraphs::TextInserted( size_t start, size_t count, DocumentReader& reader )
{
	ParagraphInfo para;
	iterator paragraph = ParagraphContaining( start, &para );

	for ( UTF16Ref text = reader.StrictRange( start, count ); !text.empty(); )
	{
		const UTF16::Unit* lineEnd = std::find( text.begin(), text.end(), UTF16::Unit( 0x0A ) );

		// There's no line break in this run; we're just making the paragraph a little bigger.
		if ( lineEnd == text.end() )
		{
			paragraph->ResetLength( paragraph->Length() + text.size() );
			break;
		}

		size_t lineEndPos = start + ( lineEnd - text.begin() );

		// Terminate the paragraph at the line break, and note how much text needs to move to a new paragraph.
		size_t fragmentLength = ( para.textStart + paragraph->Length() ) - start;
		paragraph->ResetLength( ( lineEndPos + 1 ) - para.textStart );

		// Move text after the line break to a new paragraph.
		para.textStart += paragraph->Length();
		paragraph = CreateAfter( paragraph );
		paragraph->ResetLength( fragmentLength );

		start = lineEndPos + 1;
		text  = UTF16Ref( lineEnd + 1, text.end() );
	}
}

void TextParagraphs::TextDeleted( size_t start, size_t count )
{
	ParagraphInfo para;
	iterator paragraph = ParagraphContaining( start, &para );

	while ( start + count >= ( para.textStart + paragraph->Length() ) && !IsLastParagraph( paragraph ) )
	{
		size_t fragmentLength = start - para.textStart;
		size_t deletedLength  = ( para.textStart + paragraph->Length() ) - start;

		paragraph = Delete( paragraph );
		paragraph->ResetLength( paragraph->Length() + fragmentLength );
		count -= deletedLength;
	}

	Require( count <= paragraph->Length() );
	paragraph->ResetLength( paragraph->Length() - count );
}

TextParagraphs::iterator TextParagraphs::CreateAfter( iterator it )
{
	++it;
	return m_paragraphs.insert( it, TextParagraph() );
}

TextParagraphs::iterator TextParagraphs::Delete( iterator it )
{
	m_paragraphs.erase( it++ );
	return it;
}

bool TextParagraphs::IsLastParagraph( iterator it )
{
	return ++it == m_paragraphs.end();
}

size_t TextParagraphs::FindBreak( size_t pos, BreakType type ) const
{
	ParagraphInfo para;
	iterator paragraph = ParagraphContaining( pos, &para );

	DocumentReader reader( m_doc, para.textStart, paragraph->Length() );
	pos -= para.textStart;

	switch ( type )
	{
	case nextChar:  pos = paragraph->NextCharStop( pos, reader ); break;
	case nextWord:  pos = paragraph->NextWordStop( pos, reader ); break;
	case prevChar:  pos = paragraph->PrevCharStop( pos, reader ); break;
	case prevWord:  pos = paragraph->PrevWordStop( pos, reader ); break;
	}

	return pos + para.textStart;
}

size_t TextParagraphs::LineStart( LONG y ) const
{
	ParagraphInfo para;
	iterator paragraph = ParagraphContaining( y, &para );
	Require( paragraph != m_paragraphs.end() );

	return para.textStart + paragraph->LineStart( y - para.yStart );
}

size_t TextParagraphs::LineEnd( LONG y ) const
{
	ParagraphInfo para;
	iterator paragraph = ParagraphContaining( y, &para );
	Require( paragraph != m_paragraphs.end() );

	return para.textStart + paragraph->LineEnd( y - para.yStart );
}

POINT TextParagraphs::PointFromChar( size_t pos, bool advancing ) const
{
	ParagraphInfo para;
	iterator paragraph = ParagraphContaining( pos, &para );

	POINT result = paragraph->PointFromChar( pos - para.textStart, advancing );
	result.y += para.yStart;
	return result;
}

size_t TextParagraphs::CharFromPoint( POINT* point ) const
{
	point->y = (std::max)( point->y, LONG( 0 ) );

	ParagraphInfo para;
	iterator paragraph = ParagraphContaining( point->y, &para );

	if ( paragraph == m_paragraphs.end() )
	{
		Require( paragraph != m_paragraphs.begin() );

		--paragraph;
		para.textStart -= paragraph->Length();
		para.yStart    -= paragraph->Height();
	}

	point->y -= para.yStart;
	size_t result = para.textStart + paragraph->CharFromPoint( point );
	point->y += para.yStart;
	return result;
}

TextParagraphs::iterator TextParagraphs::ParagraphContaining( size_t pos, ParagraphInfo* para ) const
{
	para->textStart = 0;
	para->yStart    = 0;

	for ( iterator it = m_paragraphs.begin(); it != m_paragraphs.end(); ++it )
	{
		if ( pos < para->textStart + it->Length() )
			return it;

		para->textStart += it->Length();
		para->yStart    += it->Height();
	}

	iterator lastParagraph = m_paragraphs.end();
	--lastParagraph;

	para->textStart -= lastParagraph->Length();
	para->yStart    -= lastParagraph->Height();
	return lastParagraph;
}

TextParagraphs::iterator TextParagraphs::ParagraphContaining( LONG y, ParagraphInfo* para ) const
{
	para->textStart = 0;
	para->yStart    = 0;

	for ( iterator it = m_paragraphs.begin(); it != m_paragraphs.end(); ++it )
	{
		if ( y < para->yStart + it->Height() )
			return it;

		para->textStart += it->Length();
		para->yStart    += it->Height();
	}

	return m_paragraphs.end();
}

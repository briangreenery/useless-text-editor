// VisualDocument.cpp

#include "VisualDocument.h"
#include "DocumentReader.h"
#include "TextDocument.h"
#include "TextStyle.h"
#include "TextSelection.h"
#include "VisualPainter.h"
#include "TextLayoutArgs.h"
#include "UniscribeLayout.h"
#include "SimpleLayout.h"
#include "EmptyTextBlock.h"
#include "Assert.h"

#undef min
#undef max

VisualDocument::VisualDocument( const TextDocument& doc, TextStyle& style )
	: m_doc( doc )
	, m_style( style )
	, m_lineCount( 1 )
{
	m_blocks.push_back( TextBlockPtr( new EmptyTextBlock( false, m_style ) ) );
}

void VisualDocument::Draw( HDC hdc, RECT rect, TextSelection selection )
{
	VisualPainter painter( hdc, m_doc, m_style, selection );

	BlockContaining_Result block = BlockContaining( rect.top );

	OffsetRect( &rect, 0, -block.yStart );

	while ( block.it != m_blocks.end() && !IsRectEmpty( &rect ) )
	{
		painter.SetOrigin( block.textStart, block.yStart );

		block->Draw( painter, rect );

		rect.bottom -= block->Height() - rect.top;
		rect.top = 0;

		block.yStart += block->Height();
		block.textStart += block->Length();
		++block.it;
	}
}

void VisualDocument::Update( HDC hdc, int maxWidth, TextChange change )
{
	BlockContaining_Result block = BlockContaining( change.start );

	size_t start = block.textStart;
	size_t count = 0;
	size_t modifiedCount = change.end + block.textStart;

	do
	{
		m_lineCount -= block->LineCount();
		count += block->Length();
		block.it = m_blocks.erase( block.it );
	}
	while ( count <= modifiedCount && block.it != m_blocks.end() );

	LayoutText( block.it, start, count + change.delta, hdc, maxWidth );
}

bool VisualDocument::IsSimpleText( UTF16Ref text ) const
{
	for ( const UTF16::Unit* it = text.begin(); it != text.end(); ++it )
		if ( *it >= 128 )
			return ScriptIsComplex( text.begin(), text.size(), SIC_COMPLEX ) != S_OK;

	return true;
}

void VisualDocument::LayoutText( TextBlockList::const_iterator it, size_t start, size_t count, HDC hdc, int maxWidth )
{
	if ( maxWidth != 0 )
		maxWidth = std::max( maxWidth, m_style.avgCharWidth * 10 );

	TextLayoutArgs layoutArgs( m_doc, m_style, hdc, maxWidth );
	DocumentReader reader( m_doc );

	for ( size_t end = start + count; start < end; )
	{
		size_t lineEnd;

		for ( lineEnd = start; lineEnd < end; ++lineEnd )
			if ( m_doc[lineEnd] == UTF16::Unit( 0x0A ) )
				break;

		if ( lineEnd == start )
		{
			m_lineCount++;
			m_blocks.insert( it, TextBlockPtr( new EmptyTextBlock( true, m_style ) ) );
		}
		else
		{
			layoutArgs.text = reader.StrictRange( start, lineEnd - start );
			layoutArgs.endsWithNewline = lineEnd != end;
			layoutArgs.textStart = start;

			TextBlockPtr block = IsSimpleText( layoutArgs.text )
			                        ? SimpleLayoutParagraph( layoutArgs )
			                        : UniscribeLayoutParagraph( layoutArgs );

			m_lineCount += block->LineCount();
			m_blocks.insert( it, std::move( block ) );
		}

		start = lineEnd + 1;
	}

	if ( it == m_blocks.end() && m_blocks.empty() || m_blocks.back()->EndsWithNewline() )
	{
		m_lineCount++;
		m_blocks.push_back( TextBlockPtr( new EmptyTextBlock( false, m_style ) ) );
	}
}

size_t VisualDocument::LineCount() const
{
	return m_lineCount;
}

size_t VisualDocument::LineStart( int y ) const
{
	BlockContaining_Result block = BlockContaining( y );
	Assert( block.it != m_blocks.end() );

	return block.textStart + block->LineStart( y - block.yStart );
}

size_t VisualDocument::LineEnd( int y ) const
{
	BlockContaining_Result block = BlockContaining( y );
	Assert( block.it != m_blocks.end() );

	return block.textStart + block->LineEnd( y - block.yStart );
}

POINT VisualDocument::PointFromChar( size_t pos, bool advancing ) const
{
	BlockContaining_Result block = BlockContaining( pos );
	Assert( block.it != m_blocks.end() );

	POINT result = block->PointFromChar( pos - block.textStart, advancing );
	result.y += block.yStart;

	return result;
}

size_t VisualDocument::CharFromPoint( POINT* point ) const
{
	point->y = std::max( point->y, LONG( 0 ) );

	BlockContaining_Result block = BlockContaining( point->y );

	if ( block.it == m_blocks.end() )
	{
		Assert( block.it != m_blocks.begin() );

		--block.it;
		block.textStart -= block->Length();
		block.yStart    -= block->Height();
	}

	point->y -= block.yStart;
	size_t result = block.textStart + block->CharFromPoint( point );
	point->y += block.yStart;

	return result;
}

int VisualDocument::Height() const
{
	return m_lineCount * m_style.lineHeight;
}

VisualDocument::BlockContaining_Result VisualDocument::BlockContaining( size_t pos ) const
{
	BlockContaining_Result result;

	result.textStart = 0;
	result.yStart    = 0;

	for ( result.it = m_blocks.begin(); result.it != m_blocks.end(); ++result.it )
	{
		if ( pos < result.textStart + result->Length() )
			return result;

		result.textStart += result->Length();
		result.yStart    += result->Height();
	}

	--result.it;
	result.textStart -= result->Length();
	result.yStart    -= result->Height();

	return result;
}

VisualDocument::BlockContaining_Result VisualDocument::BlockContaining( int y ) const
{
	BlockContaining_Result result;

	result.textStart = 0;
	result.yStart    = 0;

	for ( result.it = m_blocks.begin(); result.it != m_blocks.end(); ++result.it )
	{
		if ( y < result.yStart + result->Height() )
			return result;

		result.textStart += result->Length();
		result.yStart    += result->Height();
	}

	return result;
}

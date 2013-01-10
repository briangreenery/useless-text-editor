// VisualDocument.cpp

#include "VisualDocument.h"
#include "TextStyleReader.h"
#include "Document.h"
#include "TextStyleRegistry.h"
#include "TextSelection.h"
#include "VisualPainter.h"
#include "TextLayoutArgs.h"
#include "UniscribeLayout.h"
#include "SimpleLayout.h"
#include "EmptyTextBlock.h"
#include <cassert>
#include <stdio.h>

VisualDocument::VisualDocument( const Document& doc, TextStyleRegistry& styleRegistry )
	: m_doc( doc )
	, m_styleRegistry( styleRegistry )
	, m_lineCount( 1 )
{
	m_blocks.push_back( TextBlockPtr( new EmptyTextBlock( false, m_styleRegistry ) ) );
}

void VisualDocument::DrawText( HDC hdc, RECT rect, CharSelection selection ) const
{
	VisualPainter painter( hdc, m_doc.Chars(), m_styleRegistry, m_squiggle, selection );

	BlockContaining_Result block = BlockContaining( rect.top );
	OffsetRect( &rect, 0, -block.yStart );

	Draw( block, painter, rect, &TextBlock::DrawBackground );
	Draw( block, painter, rect, &TextBlock::DrawText );
}

void VisualDocument::DrawLineNumbers( HDC hdc, RECT rect ) const
{
	VisualPainter painter( hdc, m_doc.Chars(), m_styleRegistry, m_squiggle, CharSelection() );

	BlockContaining_Result block = BlockContaining( rect.top );

	COLORREF oldTextColor = SetTextColor( hdc, m_styleRegistry.Theme().gutterTextColor );
	UINT oldAlign = SetTextAlign( hdc, TA_RIGHT );

	while ( block.it != m_blocks.end() && block.yStart < rect.bottom )
	{
		wchar_t buffer[20];
		int length = swprintf_s( buffer, L"%d", block.logicalLine );

		if ( length > 0 )
			ExtTextOutW( painter.hdc, rect.right - m_styleRegistry.AvgCharWidth(), block.yStart, ETO_CLIPPED, &rect, buffer, length, NULL );

		block.yStart += block->Height();
		block.textStart += block->Length();
		block.logicalLine++;
		++block.it;
	}

	SetTextAlign( hdc, oldAlign );
	SetTextColor( hdc, oldTextColor );
}

void VisualDocument::Draw( BlockContaining_Result block, VisualPainter& painter, RECT rect, DrawFunction drawFunction ) const
{
	while ( block.it != m_blocks.end() && !IsRectEmpty( &rect ) )
	{
		painter.SetOrigin( block.textStart, block.yStart );

		const TextBlock& textBlock = **block.it;
		(textBlock.*drawFunction)( painter, rect );

		rect.bottom -= block->Height() - rect.top;
		rect.top = 0;

		block.yStart += block->Height();
		block.textStart += block->Length();
		++block.it;
	}
}

void VisualDocument::Update( HDC hdc, int maxWidth, CharChange change )
{
	BlockContaining_Result block = BlockContaining( change.start );

	size_t start = block.textStart;
	size_t count = 0;
	size_t modifiedCount = change.end - block.textStart;

	do
	{
		m_lineCount -= block->LineCount();
		count += block->Length();
		block.it = m_blocks.erase( block.it );
	}
	while ( count <= modifiedCount && block.it != m_blocks.end() );

	LayoutText( block.it, start, count + change.delta, hdc, maxWidth );
}

bool VisualDocument::IsSimpleText( ArrayRef<const wchar_t> text ) const
{
	for ( const wchar_t* it = text.begin(); it != text.end(); ++it )
		if ( *it >= 128 )
			return ScriptIsComplex( text.begin(), text.size(), SIC_COMPLEX ) != S_OK;

	return true;
}

void VisualDocument::LayoutText( TextBlockList::const_iterator it, size_t start, size_t count, HDC hdc, int maxWidth )
{
	if ( maxWidth != 0 )
		maxWidth = (std::max)( maxWidth, m_styleRegistry.AvgCharWidth() * 10 );

	TextLayoutArgs layoutArgs( m_doc.Chars(), m_styleRegistry, hdc, maxWidth );

	TextStyleReader styleReader( m_styleRegistry );

	for ( size_t end = start + count; start < end; )
	{
		size_t lineEnd;

		for ( lineEnd = start; lineEnd < end; ++lineEnd )
			if ( m_doc.Chars()[lineEnd] == wchar_t( 0x0A ) )
				break;

		if ( lineEnd == start )
		{
			m_lineCount++;
			m_blocks.insert( it, TextBlockPtr( new EmptyTextBlock( true, m_styleRegistry ) ) );
		}
		else
		{
			layoutArgs.text = m_doc.Read( start, lineEnd - start );
			layoutArgs.fonts = styleReader.Fonts( start, lineEnd - start );
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

	if ( m_blocks.empty() || m_blocks.back()->EndsWithNewline() )
	{
		m_lineCount++;
		m_blocks.push_back( TextBlockPtr( new EmptyTextBlock( false, m_styleRegistry ) ) );
	}
}

size_t VisualDocument::LineCount() const
{
	return m_lineCount;
}

size_t VisualDocument::LineStart( size_t pos ) const
{
	BlockContaining_Result block = BlockContaining( pos );
	assert( block.it != m_blocks.end() );

	return block.textStart + block->LineStart( y - block.yStart );
}

size_t VisualDocument::LineEnd( int y ) const
{
	BlockContaining_Result block = BlockContaining( y );
	assert( block.it != m_blocks.end() );

	return block.textStart + block->LineEnd( y - block.yStart );
}

POINT VisualDocument::PointFromChar( size_t pos, bool advancing ) const
{
	BlockContaining_Result block = BlockContaining( pos );
	assert( block.it != m_blocks.end() );

	POINT result = block->PointFromChar( pos - block.textStart, advancing );
	result.y += block.yStart;

	return result;
}

size_t VisualDocument::CharFromPoint( POINT* point ) const
{
	point->y = (std::max)( point->y, LONG( 0 ) );

	BlockContaining_Result block = BlockContaining( point->y );

	if ( block.it == m_blocks.end() )
	{
		assert( block.it != m_blocks.begin() );

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
	return m_lineCount * m_styleRegistry.LineHeight();
}

VisualDocument::BlockContaining_Result VisualDocument::BlockContaining( size_t pos ) const
{
	BlockContaining_Result result;

	result.textStart   = 0;
	result.yStart      = 0;
	result.logicalLine = 1;

	for ( result.it = m_blocks.begin(); result.it != m_blocks.end(); ++result.it )
	{
		if ( pos < result.textStart + result->Length() )
			return result;

		result.textStart += result->Length();
		result.yStart    += result->Height();
		result.logicalLine++;
	}

	--result.it;
	result.textStart -= result->Length();
	result.yStart    -= result->Height();

	return result;
}

VisualDocument::BlockContaining_Result VisualDocument::BlockContaining( int y ) const
{
	BlockContaining_Result result;

	result.textStart   = 0;
	result.yStart      = 0;
	result.logicalLine = 1;

	for ( result.it = m_blocks.begin(); result.it != m_blocks.end(); ++result.it )
	{
		if ( y < result.yStart + result->Height() )
			return result;

		result.textStart += result->Length();
		result.yStart    += result->Height();
		result.logicalLine++;
	}

	return result;
}

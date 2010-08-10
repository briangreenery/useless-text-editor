// VisualDocument.cpp

#include "VisualDocument.h"
#include "DocumentReader.h"
#include "TextDocument.h"
#include "TextSelection.h"
#include "VisualPainter.h"
#include "LayoutEngine.h"
#include "Assert.h"

#undef min
#undef max

VisualDocument::VisualDocument( const TextDocument& doc, TextStyle& style )
	: m_doc( doc )
	, m_style( style )
	, m_lineCount( 1 )
{
	m_blocks.push_back( VisualBlock( 0 ) );
}

void VisualDocument::Draw( HDC hdc, RECT rect, TextSelection selection )
{
	VisualPainter painter( hdc, m_style, selection );

	BlockContaining_Result block = BlockContaining( rect.top );

	OffsetRect( &rect, 0, -block.yStart );

	while ( block.it != m_blocks.end() && !IsRectEmpty( &rect ) )
	{
		painter.SetOrigin( block.textStart, block.yStart );

		block->Draw( painter, rect );

		rect.bottom -= block->Height( m_style ) - rect.top;
		rect.top = 0;

		block.yStart += block->Height( m_style );
		block.textStart += block->Length();
		++block.it;
	}
}

void VisualDocument::Update( HDC hdc, int maxWidth, TextChange change )
{
	BlockContaining_Result block = BlockContaining( change.pos );

	size_t start = block.textStart;
	size_t count = 0;

	size_t modifiedCount = 0;
	if ( change.type != TextChange::insertion )
		modifiedCount = change.count + ( change.pos - block.textStart );

	do
	{
		m_lineCount -= block->LineCount();
		count += block->Length();
		block.it = m_blocks.erase( block.it );
	}
	while ( count < modifiedCount );

	if ( change.type == TextChange::insertion )
		count += change.count;
	else if ( change.type == TextChange::deletion )
		count -= change.count;

	LayoutEngine layout( m_doc, m_style, hdc, maxWidth );

	VisualBlockList& blocks = layout.LayoutText( start, count );
	for ( VisualBlockList::const_iterator it = blocks.begin(); it != blocks.end(); ++it )
		m_lineCount += it->LineCount();

	m_blocks.splice( block.it, blocks );

	if ( block.it == m_blocks.end() && m_blocks.empty() || m_blocks.back().EndsWithNewline() )
	{
		m_blocks.push_back( VisualBlock( 0 ) );
		m_lineCount++;
	}
}

void VisualDocument::UpdateAll( HDC hdc, int maxWidth )
{
	Update( hdc, maxWidth, TextChange::Modification( 0, m_doc.Length() ) );
}

size_t VisualDocument::LineCount() const
{
	return m_lineCount;
}

size_t VisualDocument::LineStart( LONG y ) const
{
	BlockContaining_Result block = BlockContaining( y );
	Assert( block.it != m_blocks.end() );

	return block.textStart + block->LineStart( y - block.yStart, m_style );
}

size_t VisualDocument::LineEnd( LONG y ) const
{
	BlockContaining_Result block = BlockContaining( y );
	Assert( block.it != m_blocks.end() );

	return block.textStart + block->LineEnd( y - block.yStart, m_style );
}

POINT VisualDocument::PointFromChar( size_t pos, bool advancing ) const
{
	BlockContaining_Result block = BlockContaining( pos );
	Assert( block.it != m_blocks.end() );

	POINT result = block->PointFromChar( pos - block.textStart, advancing, m_style );
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
		block.yStart    -= block->Height( m_style );
	}

	point->y -= block.yStart;
	size_t result = block.textStart + block->CharFromPoint( point, m_style );
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
		result.yStart    += result->Height( m_style );
	}

	--result.it;
	result.textStart -= result->Length();
	result.yStart    -= result->Height( m_style );

	return result;
}

VisualDocument::BlockContaining_Result VisualDocument::BlockContaining( LONG y ) const
{
	BlockContaining_Result result;

	result.textStart = 0;
	result.yStart    = 0;

	for ( result.it = m_blocks.begin(); result.it != m_blocks.end(); ++result.it )
	{
		if ( y < result.yStart + result->Height( m_style ) )
			return result;

		result.textStart += result->Length();
		result.yStart    += result->Height( m_style );
	}

	return result;
}
// TextDocumentUndo.cpp

#include "TextDocumentUndo.h"
#include "TextDocument.h"
#include <cassert>

TextUndoAction::TextUndoAction( Type type, size_t pos, size_t length, size_t savedTextPos )
	: type( type )
	, pos( pos )
	, length( length )
	, savedTextPos( savedTextPos )
{
}

TextUndoGroup::TextUndoGroup( const TextSelection& beforeSelection )
	: m_beforeSelection( beforeSelection )
{
}

void TextUndoGroup::RecordInsertion( TextDocument&, size_t pos, size_t length, size_t savedTextPos )
{
	if ( m_actions.empty()
	  || m_actions.back().type != TextUndoAction::insertion
	  || m_actions.back().pos + m_actions.back().length != pos )
	{
		m_actions.push_back( TextUndoAction( TextUndoAction::insertion, pos, length, savedTextPos ) );
	}
	else
	{
		assert( m_actions.back().savedTextPos + m_actions.back().length == savedTextPos );
		m_actions.back().length += length;
	}
}

void TextUndoGroup::RecordDeletion( TextDocument&, size_t pos, size_t length, size_t savedTextPos )
{
	m_actions.push_back( TextUndoAction( TextUndoAction::deletion, pos, length, savedTextPos ) );
}

std::pair<TextChange,TextSelection> TextUndoGroup::Undo( TextDocument& doc, UTF16Ref savedText ) const
{
	TextChange change;

	for ( auto it = m_actions.rbegin(); it != m_actions.rend(); ++it )
		change.AddChange( it->type == TextUndoAction::insertion
		                     ? doc.Delete( it->pos, it->length )
		                     : doc.Insert( it->pos, UTF16Ref( savedText.begin() + it->savedTextPos, it->length ) ) );

	return std::make_pair( change, m_beforeSelection );
}

TextChange TextUndoGroup::Redo( TextDocument& doc, UTF16Ref savedText ) const
{
	TextChange change;

	for ( auto it = m_actions.begin(); it != m_actions.end(); ++it )
		change.AddChange( it->type == TextUndoAction::insertion
		                     ? doc.Insert( it->pos, UTF16Ref( savedText.begin() + it->savedTextPos, it->length ) )
		                     : doc.Delete( it->pos, it->length ) );

	return change;
}

TextDocumentUndo::TextDocumentUndo()
	: m_endGroup( false )
	, m_index( 0 )
	, m_undoingOrRedoing( false )
{
}

size_t TextDocumentUndo::SaveText( TextDocument& doc, size_t pos, size_t length )
{
	size_t savedTextPos = m_savedText.size();
	m_savedText.resize( savedTextPos + length );
	doc.Read( pos, length, &m_savedText[savedTextPos] );
	return savedTextPos;
}

void TextDocumentUndo::RecordInsertion( TextDocument& doc, size_t pos, size_t length )
{
	if ( m_undoingOrRedoing )
		return;

	RemoveUnreachableGroups();

	if ( m_endGroup || m_groups.empty() )
		m_groups.push_back( TextUndoGroup( m_beforeSelection ) );

	m_groups.back().RecordInsertion( doc, pos, length, SaveText( doc, pos, length ) );
	m_index = m_groups.size();

	m_endGroup = false;
}

void TextDocumentUndo::RecordDeletion( TextDocument& doc, size_t pos, size_t length )
{
	if ( m_undoingOrRedoing )
		return;

	RemoveUnreachableGroups();

	if ( m_endGroup || m_groups.empty() )
		m_groups.push_back( TextUndoGroup( m_beforeSelection ) );

	m_groups.back().RecordDeletion( doc, pos, length, SaveText( doc, pos, length ) );
	m_index = m_groups.size();

	m_endGroup = false;
}

bool TextDocumentUndo::CanUndo() const
{
	return m_index > 0;
}

bool TextDocumentUndo::CanRedo() const
{
	return m_index < m_groups.size();
}

std::pair<TextChange,TextSelection> TextDocumentUndo::Undo( TextDocument& doc )
{
	if ( !CanUndo() )
		return std::pair<TextChange,TextSelection>();

	m_endGroup = true;
	const TextUndoGroup& group = m_groups[--m_index];

	m_undoingOrRedoing = true;
	std::pair<TextChange,TextSelection> change = group.Undo( doc, UTF16Ref( &m_savedText[0], m_savedText.size() ) );
	m_undoingOrRedoing = false;

	return change;
}

TextChange TextDocumentUndo::Redo( TextDocument& doc )
{
	if ( !CanRedo() )
		return TextChange();

	m_endGroup = true;
	const TextUndoGroup& group = m_groups[m_index++];

	m_undoingOrRedoing = true;
	TextChange change = group.Redo( doc, UTF16Ref( &m_savedText[0], m_savedText.size() ) );
	m_undoingOrRedoing = false;

	return change;
}

void TextDocumentUndo::SetBeforeSelection( const TextSelection& beforeSelection )
{
	m_beforeSelection = beforeSelection;
}

void TextDocumentUndo::EndGroup()
{
	m_endGroup = true;
}

void TextDocumentUndo::RemoveUnreachableGroups()
{
	if ( !CanRedo() )
		return;

	m_groups.erase( m_groups.begin() + m_index, m_groups.end() );

	if ( m_groups.empty() )
	{
		m_savedText.clear();
	}
	else
	{
		size_t savedTextEnd = m_groups.back().m_actions.back().savedTextPos + m_groups.back().m_actions.back().length;
		m_savedText.erase( m_savedText.begin() + savedTextEnd, m_savedText.end() );
	}
}

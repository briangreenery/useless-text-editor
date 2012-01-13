// TextDocumentUndo.cpp

#include "TextDocumentUndo.h"
#include "TextDocument.h"
#include "Assert.h"

TextDocumentUndo::TextDocumentUndo()
	: m_stopGrouping( false )
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

UTF16Ref TextDocumentUndo::SavedText( size_t pos, size_t length ) const
{
	const UTF16::Unit* start = &m_savedText[pos];
	return UTF16Ref( start, start + length );
}

void TextDocumentUndo::RecordInsertion( TextDocument& doc, size_t pos, size_t length )
{
	if ( m_undoingOrRedoing )
		return;

	RemoveUnreachableActions();
	size_t savedTextPos = SaveText( doc, pos, length );

	if ( m_stopGrouping
	  || m_actions.empty()
	  || m_actions.back().type != TextUndoAction::insertion
	  || m_actions.back().pos + m_actions.back().length != pos )
	{
		TextUndoAction action = { TextUndoAction::insertion, pos, length, savedTextPos };
		m_actions.push_back( action );
		m_index++;
	}
	else
	{
		Assert( m_actions.back().savedTextPos + m_actions.back().length == savedTextPos );
		m_actions.back().length += length;
	}

	m_stopGrouping = false;
}

void TextDocumentUndo::RecordDeletion( TextDocument& doc, size_t pos, size_t length )
{
	if ( m_undoingOrRedoing )
		return;

	RemoveUnreachableActions();

	TextUndoAction action = { TextUndoAction::deletion, pos, length, SaveText( doc, pos, length ) };
	m_actions.push_back( action );
	m_index++;

	m_stopGrouping = false;
}

bool TextDocumentUndo::CanUndo() const
{
	return m_index > 0;
}

bool TextDocumentUndo::CanRedo() const
{
	return m_index < m_actions.size();
}

TextChange TextDocumentUndo::Undo( TextDocument& doc )
{
	if ( !CanUndo() )
		return TextChange();

	m_stopGrouping = true;
	const TextUndoAction& action = m_actions[--m_index];

	m_undoingOrRedoing = true;
	TextChange change = ( action.type == TextUndoAction::insertion )
	                       ? doc.Delete( action.pos, action.length )
	                       : doc.Insert( action.pos, SavedText( action.savedTextPos, action.length ) );
	m_undoingOrRedoing = false;

	return change;
}

TextChange TextDocumentUndo::Redo( TextDocument& doc )
{
	if ( !CanRedo() )
		return TextChange();

	m_stopGrouping = true;
	const TextUndoAction& action = m_actions[m_index++];

	m_undoingOrRedoing = true;
	TextChange change = ( action.type == TextUndoAction::insertion )
	                       ? doc.Insert( action.pos, SavedText( action.savedTextPos, action.length ) )
	                       : doc.Delete( action.pos, action.length );
	m_undoingOrRedoing = false;

	return change;
}

void TextDocumentUndo::RemoveUnreachableActions()
{
	if ( !CanRedo() )
		return;

	m_actions.erase( m_actions.begin() + m_index, m_actions.end() );

	if ( m_actions.empty() )
	{
		m_savedText.clear();
	}
	else
	{
		size_t savedTextEnd = m_actions.back().savedTextPos + m_actions.back().length;
		m_savedText.erase( m_savedText.begin() + savedTextEnd, m_savedText.end() );
	}
}

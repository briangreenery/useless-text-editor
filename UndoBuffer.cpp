// UndoBuffer.cpp

#include "UndoBuffer.h"
#include <cassert>

//TextUndoAction::TextUndoAction( Type type, size_t pos, size_t length, size_t savedTextPos )
//	: type( type )
//	, pos( pos )
//	, length( length )
//	, savedTextPos( savedTextPos )
//{
//}
//
//TextUndoGroup::TextUndoGroup( const TextSelection& beforeSelection )
//	: m_beforeSelection( beforeSelection )
//{
//}
//
//void TextUndoGroup::RecordInsertion( TextDocument&, size_t pos, size_t length, size_t savedTextPos )
//{
//	if ( m_actions.empty()
//	  || m_actions.back().type != TextUndoAction::insertion
//	  || m_actions.back().pos + m_actions.back().length != pos )
//	{
//		m_actions.push_back( TextUndoAction( TextUndoAction::insertion, pos, length, savedTextPos ) );
//	}
//	else
//	{
//		assert( m_actions.back().savedTextPos + m_actions.back().length == savedTextPos );
//		m_actions.back().length += length;
//	}
//}
//
//void TextUndoGroup::RecordDeletion( TextDocument&, size_t pos, size_t length, size_t savedTextPos )
//{
//	m_actions.push_back( TextUndoAction( TextUndoAction::deletion, pos, length, savedTextPos ) );
//}
//
//UndoChange TextUndoGroup::Undo( CharBuffer& charBuffer, UTF16Ref savedText ) const
//{
//	UndoChange change;
//
//	for ( auto it = m_actions.rbegin(); it != m_actions.rend(); ++it )
//		change += ( it->type == TextUndoAction::insertion )
//		        ? charBuffer.Delete( it->pos, it->length )
//		        : charBuffer.Insert( it->pos, savedTextStart, it->length );
//
//	return std::make_pair( change, m_beforeSelection );
//}
//
//TextChange TextUndoGroup::Redo( TextDocument& doc, UTF16Ref savedText ) const
//{
//	TextChange change;
//
//	for ( auto it = m_actions.begin(); it != m_actions.end(); ++it )
//		change.AddChange( it->type == TextUndoAction::insertion
//		                     ? doc.Insert( it->pos, UTF16Ref( savedText.begin() + it->savedTextPos, it->length ) )
//		                     : doc.Delete( it->pos, it->length ) );
//
//	return change;
//}
//
//UndoBuffer::UndoBuffer( CharBuffer& charBuffer )
//	: m_charBuffer( charBuffer )
//{
//}
//
//void UndoBuffer::Update( CharBuffer& charBuffer, CharChange change )
//{
//	m_groups.erase( m_groups.begin() + m_index, m_groups.end() );
//
//	if ( m_endCurrentGroup || m_groups.empty() )
//		m_groups.push_back( UndoGroup( m_selection ) );
//
//	m_groups.back().Update( charBuffer, change );
//	m_index = m_groups.size();
//
//	m_endCurrentGroup = false;
//}
//
//bool UndoBuffer::CanUndo() const
//{
//	return m_index > 0;
//}
//
//bool UndoBuffer::CanRedo() const
//{
//	return m_index < m_groups.size();
//}
//
//UndoChange UndoBuffer::Undo( CharBuffer& charBuffer )
//{
//	if ( !CanUndo() )
//		return UndoChange();
//
//	m_endCurrentGroup = true;
//	return m_groups[--m_index].Undo( charBuffer );
//}
//
//CharChange UndoBuffer::Redo( CharBuffer& charBuffer )
//{
//	if ( !CanRedo() )
//		return CharChange();
//
//	m_endCurrentGroup = true;
//	return m_groups[m_index++].Redo( charBuffer );
//}
//
//void UndoBuffer::SetSelection( CharRange selection )
//{
//	m_selection = selection;
//}
//
//void UndoBuffer::EndGroup()
//{
//	m_endCurrentGroup = true;
//}

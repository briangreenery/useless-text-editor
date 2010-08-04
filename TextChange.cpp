// TextChange.cpp

#include "TextChange.h"

TextChange TextChange::NoChange()
{
	TextChange change = { 0, 0, noChange };
	return change;
}

TextChange TextChange::Insertion( size_t pos, size_t count )
{
	TextChange change = { pos, count, insertion };
	return change;
}

TextChange TextChange::Deletion( size_t pos, size_t count )
{
	TextChange change = { pos, count, deletion };
	return change;
}

TextChange TextChange::Modification( size_t pos, size_t count )
{
	TextChange change = { pos, count, modification };
	return change;
}

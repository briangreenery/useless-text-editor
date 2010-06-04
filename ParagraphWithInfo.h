// ParagraphWithInfo.h

#ifndef ParagraphWithInfo_h
#define ParagraphWithInfo_h

class TextParagraph;

struct ParagraphWithInfo
{
	size_t         length;
	int            height;
	TextParagraph* paragraph;
};

#endif

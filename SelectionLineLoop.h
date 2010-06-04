// SelectionLineLoop.h

#ifndef SelectionLineLoop_h
#define SelectionLineLoop_h

#include "VisualLine.h"

class SelectionLineLoop
{
public:
	SelectionLineLoop( const SelectionRanges& top, const SelectionRanges& bottom );

	int  Start() const { return m_start; }
	int  End() const   { return m_end; }
	bool IsTop() const { return m_isTop; }

	bool Unfinished() const;
	void operator++();
	
private:
	int TopStart() const;
	int TopEnd() const;

	int BottomStart() const;
	int BottomEnd() const;

	void AdvanceTop( int amount );
	void AdvanceBottom( int amount );

	void Set();
	void NextDifference();

	const SelectionRanges& m_top;
	const SelectionRanges& m_bottom;

	SelectionRanges::const_iterator m_t;
	SelectionRanges::const_iterator m_b;

	int m_topUsed;
	int m_bottomUsed;

	int  m_start;
	int  m_end;
	bool m_isTop;
};

inline bool SelectionLineLoop::Unfinished() const
{
	return m_t != m_top.end()
		|| m_b != m_bottom.end();
}

#endif

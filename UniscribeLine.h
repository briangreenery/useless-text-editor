// UniscribeLine.h

#ifndef UniscribeLine_h
#define UniscribeLine_h

#include "UniscribeRun.h"
#include "UniscribeData.h"
#include "ArrayOf.h"

class VisualPainter;
class VisualSelection;
class TextSelection;

class UniscribeLine
{
public:
	UniscribeLine();
	UniscribeLine( ArrayOf<UniscribeRun>, const UniscribeDataPtr& );

	void Draw( VisualPainter&, RECT ) const;

	VisualSelection MakeVisualSelection( TextSelection ) const;

	int    CPtoX( size_t, bool trailingEdge ) const;
	size_t XtoCP( LONG* x ) const;

	size_t TextStart() const { return m_textStart; }
	size_t TextEnd() const   { return m_textEnd; }
	int    Width() const     { return m_width; }

private:
	SizedAutoArray<int> VisualToLogicalMapping() const;

	int RunCPtoX( UniscribeRun* run, size_t cp, bool trailingEdge ) const;

	UniscribeRun* RunContaining( size_t pos, int* xStart ) const;
	UniscribeRun* RunContaining( int    x,   int* xStart ) const;

	int    m_width;
	size_t m_textStart;
	size_t m_textEnd;

	UniscribeDataPtr      m_data;
	ArrayOf<UniscribeRun> m_runs;
};

#endif

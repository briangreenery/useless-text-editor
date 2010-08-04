// VisualSelection.h

#ifndef VisualSelection_h
#define VisualSelection_h

#include <Windows.h>
#include <vector>

class VisualPainter;

class VisualSelection
{
public:
	void Add( LONG xStart, LONG xEnd );

	void Draw( VisualPainter&, RECT rect );

	void Swap( VisualSelection& );

private:
	typedef std::vector<LONG> Ranges;

	void DrawHighlight( VisualPainter&, LONG start, LONG end, const Ranges&, LONG y );

	Ranges m_ranges;
};

#endif

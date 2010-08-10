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

private:
	typedef std::vector<LONG> Ranges;
	Ranges m_ranges;
};

#endif

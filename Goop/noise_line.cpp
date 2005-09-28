#include "noise_line.h"
#include "vec.h"
#include "math_func.h"

#include <allegro.h>
#include <vector>

using namespace std;

NoiseLine::NoiseLine()
{
}

NoiseLine::~NoiseLine()
{
}

void NoiseLine::createPath( int iterations, float variation )
{
	m_nodes.clear();
	int N = ( 1 << iterations ) + 1;
	m_nodes.insert(m_nodes.begin(), N, 0);
	for ( int i = iterations; i > 0; --i )
	{
		int offset = ( 1 << (i - 1) );
		int step = ( 1 << i );
		
		for ( int x = offset; x < N; x += step )
		{
			//m_nodes[x] = ( m_nodes[x-offset] + m_nodes[x+offset] )/2 + variation*(midrnd()*i)/iterations;
			m_nodes[x] = ( m_nodes[x-offset] + m_nodes[x+offset] )/2 + variation*(midrnd()*i);
		}
	}
}

void NoiseLine::render(BITMAP* where, int x, int y, int x1, int y1, int colour)
{
	Vec inc = Vec( x1 - x, y1-y ) / m_nodes.size();
	Vec perpToLine = inc.perp().normal();
	Vec pos = Vec(x,y);
	Vec oldRenderPos = pos;
	for ( int x = 0; x < m_nodes.size(); ++x )
	{
		pos += inc;
		Vec renderPos = pos + perpToLine*m_nodes[x];
		line( where, oldRenderPos.x, oldRenderPos.y, renderPos.x, renderPos.y, colour);
		oldRenderPos = renderPos;
	}
}

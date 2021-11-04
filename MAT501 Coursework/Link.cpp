#include "Link.h"
#include "Neuron.h"


double Link::Value()
{
	return m_weight * m_pFrom->Output();
}



#include "genomechart.h"

GenomeChart::GenomeChart()
{
    init();
}

void GenomeChart::init()
{
    m_cView.setScene(&m_cScene);
}

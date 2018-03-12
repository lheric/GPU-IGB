#include "tracktable.h"

TrackTable::TrackTable()
{
}

void TrackTable::resize(long lSize)
{
    m_acTrackTable.resize(lSize);
    reset();
}

void TrackTable::reset()
{
    size_t iSize =  m_acTrackTable.size();
    for(size_t i = 0; i <iSize; ++i)
    {
        m_acTrackTable[static_cast<int>(i)].reset();
    }
}

int TrackTable::size()
{
    return m_acTrackTable.size();
}

void TrackTable::clear()
{
    m_acTrackTable.clear();
}

bool TrackTable::isQualifed(long lIdx)
{
    long lSize = m_acTrackTable.size();
    if(lIdx >= lSize)
        return false;
    return m_acTrackTable[lIdx].isQualified();
}

TrackTableElement& TrackTable::getElement(long lIdx)
{
    return m_acTrackTable[lIdx];
}

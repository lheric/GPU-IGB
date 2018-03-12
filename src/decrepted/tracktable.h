#ifndef TRACKTABLE_H
#define TRACKTABLE_H
#include <QVector>
#include "gitldef.h"


/**
 * @brief The ElementStatus enum
 * Three status of each property of each track for display
 */
enum ElementStatus
{
    QUALIFIED = 0,
    UNQUALIFIED,
    UNTESTED,
};

/**
 * @brief The TrackTableElement class
 * Property for each track
 */
class TrackTableElement
{
public:
    TrackTableElement()
    {
        m_eSampled = UNTESTED;
        m_eIsShort = UNTESTED;
        m_eIsClose = UNTESTED;
    }

    void reset()
    {
        m_eSampled = UNTESTED;
        m_eIsShort = UNTESTED;
        m_eIsClose = UNTESTED;
    }

    bool isQualified() const
    {
        return m_eSampled==QUALIFIED && m_eIsShort==QUALIFIED && m_eIsClose==QUALIFIED;
    }

    ADD_CLASS_FIELD(ElementStatus, eSampled, getSampled, setSampled)    ///< chosen by sampling or not
    ADD_CLASS_FIELD(ElementStatus, eIsShort, getIsShort, setIsShort)    ///< too short or not
    ADD_CLASS_FIELD(ElementStatus, eIsClose, getIsClose, setIsClose)    ///< start-end distance too close or not
};

/**
 * @brief The TrackTable class
 * Table for all tracks in file
 */
class TrackTable
{
public:
    TrackTable();
    void resize(long lSize);
    void reset();
    int size();
    void clear();
    bool isQualifed(long lIdx);
    TrackTableElement& getElement(long lIdx);

    ADD_CLASS_FIELD_NOSETTER(QVector<TrackTableElement>, acTrackTable, getTrackTable)   ///< table
};

#endif // TRACKTABLE_H

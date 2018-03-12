#ifndef RAY_H
#define RAY_H

#include <QVector>
#include <QVector4D>
#include <QVector3D>
#include <QColor>
#include <QSet>
#include "gitldef.h"
#include "model/rom.h"

class InterpComponent
{
public:
    InterpComponent()
    {
        m_iLabel = 0;
        m_dWeight = 0.0;
        m_dIntensity = 0.0;
    }

    bool operator == (const InterpComponent& rcOther) const
    {
        //TODO
//        return m_iLabel == rcOther.m_iLabel;
        Q_UNUSED(rcOther)
        return false;
    }

    void addWeight(double dWeight)
    {
        m_dWeight += dWeight;
    }

    void addIntensity(double dIntensity)
    {
        m_dIntensity += dIntensity;
    }

    bool mergeInterpComponent(InterpComponent& other)
    {
        if(*this == other)
        {
            m_dIntensity = (m_dWeight*m_dIntensity+other.m_dWeight*other.m_dIntensity) /
                    (m_dWeight+other.m_dWeight);
            m_dWeight += other.m_dWeight;
            return true;
        }
        else
        {
            return false;
        }
    }




    ADD_CLASS_FIELD(int, iLabel, getLabel, setLabel)
    ADD_CLASS_FIELD(float, dIntensity, getIntensity, setIntensity)
    ADD_CLASS_FIELD(float, dWeight, getWeight, setWeight)
    ADD_CLASS_FIELD(QVector3D, dGradient, getGradient, setGradient)

};


class RaySamplePoint
{
public:
    RaySamplePoint() {
        m_cColor = QVector4D(0,0,0,0);
        m_cShade = QVector4D(0,0,0,0);
        m_cComponents.reserve(8);
        m_iLabel = -1;
        m_iNumLabel = -1;
    }


    QSet<int> getLabelSet()
    {
        QSet<int> cLabelSet;
        for(QSet<InterpComponent>::iterator iter = m_cComponents.begin(); iter != m_cComponents.end(); iter++)
            cLabelSet.insert(iter->getLabel());

        return cLabelSet;
    }


    double getInterpIntensity()
    {
        double dIntensity = 0.0;
        for(QSet<InterpComponent>::iterator iter = m_cComponents.begin(); iter != m_cComponents.end(); iter++)
        {
            dIntensity += (iter->getIntensity())*(iter->getWeight());
        }
        return dIntensity;
    }

    void getInterpColor(QVector4D& rcColor)
    {
        rcColor = QVector4D();
        for(QSet<InterpComponent>::iterator iter = m_cComponents.begin(); iter != m_cComponents.end(); iter++)
        {
            rcColor += g_acColorTableForRaycastHighlight[iter->getLabel()]*(iter->getWeight());
        }

    }

    void getInterpGradient(QVector3D& rcGradient)
    {
        rcGradient = QVector3D();
        for(QSet<InterpComponent>::iterator iter = m_cComponents.begin(); iter != m_cComponents.end(); iter++)
        {
            rcGradient += (iter->getGradient())*(iter->getWeight());
        }
    }

    void getInterpLightColor(QVector4D& rcColor)
    {
        rcColor = QVector4D();
        for(QSet<InterpComponent>::iterator iter = m_cComponents.begin(); iter != m_cComponents.end(); iter++)
        {
            rcColor += g_acColorTableForLightMap[iter->getLabel()]*(iter->getWeight());
        }
    }

    void merge(RaySamplePoint& rcOther)
    {
        m_cColor += (1-m_cColor.w()) * rcOther.m_cColor;
        m_cShade += (1-m_cShade.w()) * rcOther.m_cShade;
    }

    ADD_CLASS_FIELD(int, iLabel, getLabel, setLabel)
    ADD_CLASS_FIELD(int, iNumLabel, getNumLabel, setNumLabel)
    ADD_CLASS_FIELD(QVector4D, cColor, getColor, setColor)
    ADD_CLASS_FIELD(QVector4D, cShade, getShade, setShade)

    ADD_CLASS_FIELD_NOSETTER(QSet<InterpComponent>, cComponents, getComponents)

};

uint qHash(const InterpComponent &rcComp);



class Ray
{
public:
    Ray()
    {
        m_dTStart = m_dTEnd = 0;
        m_bHit = false;
        m_cColor = QColor(0,0,0,0);
        m_cViewportPos = QPoint(0,0);
    }

    int numOfPassedLabels()
    {
        QSet<int> iLabelsAlongRay;
        for(int i = 0; i < m_cSamplePoints.size(); i++)
        {
            QSet<InterpComponent>& cComponents = m_cSamplePoints[i].getComponents();

            foreach(const InterpComponent& cComp, cComponents)
            {
                iLabelsAlongRay.insert(cComp.getLabel());
            }
        }
        return iLabelsAlongRay.size();
    }

    ADD_CLASS_FIELD(QPoint, cViewportPos, getViewportPos, setViewportPos)
    ADD_CLASS_FIELD(QVector3D, cPoint, getPoint, setPoint)
    ADD_CLASS_FIELD(QVector3D, cDir, getDir, setDir)
    ADD_CLASS_FIELD(bool, bHit, getHit, setHit)
    ADD_CLASS_FIELD(double, dTStart, getTStart, setTStart)
    ADD_CLASS_FIELD(double, dTEnd, getTEnd, setTEnd)
    ADD_CLASS_FIELD(QColor, cColor, getColor, setColor)
    ADD_CLASS_FIELD(QColor, cShade, getShade, setShade)
    ADD_CLASS_FIELD_NOSETTER(QVector<RaySamplePoint>, cSamplePoints, getSamplePoints)
};
#endif // RAY_H

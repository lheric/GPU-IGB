#ifndef CSVMATRIXREADER_H
#define CSVMATRIXREADER_H
#include <QVector>
#include <QString>

class CSVMatrixReader
{
public:
    CSVMatrixReader();
    ~CSVMatrixReader();
    static QVector<QVector<double> > readMatrix(QString strFilename, double *pdMin, double *pdMax);
};

#endif // CSVMATRIXREADER_H

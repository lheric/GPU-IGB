#ifndef ROM_H
#define ROM_H

#include <QSettings>
#include <QVector4D>

#define ORGANIZATION_NAME   "IUPUI"
#define APPLICATION_NAME    "BECA"

#define COLOR_TABLE_SIZE 456

extern QSettings g_cAppSetting;
const double *g_getColorFromTable(int iIdx);
extern const double g_aiColorTable[COLOR_TABLE_SIZE][3];
extern const QVector4D g_acColorTable[COLOR_TABLE_SIZE];
extern const char* g_astrROINameLUT33[];
extern const char* g_astrROINameLUT60[];
extern const char* g_astrROINameLUT125[];
extern const char* g_astrROINameLUT250[];
extern const char* g_astrROINameLUT500[];
#define VERSION "1.0.0"    ///< version number

/// for li's proposal only TODO
extern const int g_indegrees_100631_scale33[];
extern const int g_indgrees_100631_max;
extern const int g_indgrees_100631_min;

///
extern const QVector4D g_acColorTableForRaycast[COLOR_TABLE_SIZE];
extern QVector4D g_acColorTableForRaycastHighlight[COLOR_TABLE_SIZE];
extern QVector4D g_acColorTableForLightMap[COLOR_TABLE_SIZE];
#endif // ROM_H

/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtCore/QLoggingCategory>
#include <QtGui/QVector3D>
#include <QtPositioning/QGeoCoordinate>

Q_DECLARE_LOGGING_CATEGORY(QGCGeoLog)

namespace QGCGeo
{

/**
 * @brief Project a geodetic coordinate on to local tangential plane (LTP) as coordinate with East,
 * North, and Down components in meters.
 * @param[in] coord Geodetic coordinate to project onto LTP.
 * @param[in] origin Geoedetic origin for LTP projection.
 * @param[out] x North component of coordinate in local plane.
 * @param[out] y East component of coordinate in local plane.
 * @param[out] z Down component of coordinate in local plane.
 */
void convertGeoToNed(const QGeoCoordinate &coord, const QGeoCoordinate &origin, double &x, double &y, double &z);

/**
 * @brief Transform a local (East, North, and Down) coordinate into a geodetic coordinate.
 * @param[in] x North component of local coordinate in meters.
 * @param[in] x East component of local coordinate in meters.
 * @param[in] x Down component of local coordinate in meters.
 * @param[in] origin Geoedetic origin for LTP.
 * @param[out] coord Geodetic coordinate to hold result.
 */
void convertNedToGeo(double x, double y, double z, const QGeoCoordinate &origin, QGeoCoordinate &coord);

// LatLonToUTMXY
// Converts a latitude/longitude pair to x and y coordinates in the
// Universal Transverse Mercator projection.
//
// Inputs:
//   lat - Latitude of the point, in radians.
//   lon - Longitude of the point, in radians.
//   zone - UTM zone to be used for calculating values for x and y.
//          If zone is less than 1 or greater than 60, the routine
//          will determine the appropriate zone from the value of lon.
//
// Outputs:
//   x - The x coordinate (easting) of the computed point. (in meters)
//   y - The y coordinate (northing) of the computed point. (in meters)
//
// Returns:
//   The UTM zone used for calculating the values of x and y.
//   If conversion failed the function returns 0
int convertGeoToUTM(const QGeoCoordinate& coord, double &easting, double &northing);

// UTMXYToLatLon
//
// Converts x and y coordinates in the Universal Transverse Mercator//   The UTM zone parameter should be in the range [1,60].

// projection to a latitude/longitude pair.
//
// Inputs:
// x - The easting of the point, in meters.
// y - The northing of the point, in meters.
// zone - The UTM zone in which the point lies.
// southhemi - True if the point is in the southern hemisphere;
//               false otherwise.
//
// Outputs:
// lat - The latitude of the point, in radians.
// lon - The longitude of the point, in radians.
//
// Returns:
// The function returns true if conversion succeeded.
bool convertUTMToGeo(double easting, double northing, int zone, bool southhemi, QGeoCoordinate &coord);

// Converts a latitude/longitude pair to MGRS string
//
// Inputs:
//   coord - Latitude, Longiture coordinate
//
// Returns:
//   The MGRS coordinate string
//   If conversion fails the function returns empty string
QString convertGeoToMGRS(const QGeoCoordinate &coord);

// Converts MGRS string to a latitude/longitude pair.
//
// Inputs:
// mgrs - MGRS coordinate string
//
// Outputs:
// lat - The latitude of the point, in radians.
// lon - The longitude of the point, in radians.
//
// Returns:
// The function returns true if conversion succeeded.
bool convertMGRSToGeo(const QString &mgrs, QGeoCoordinate &coord);

QVector3D convertGeodeticToEcef(const QGeoCoordinate &llh);

QGeoCoordinate convertEcefToGeodetic(const QVector3D &xyz);

QVector3D convertEcefToEnu(const QVector3D &ecef, const QGeoCoordinate &ref);

QVector3D convertEnuToEcef(const QVector3D &enu, const QGeoCoordinate &ref);

QVector3D convertGpsToEnu(const QGeoCoordinate &llh, const QGeoCoordinate &ref);

QGeoCoordinate convertEnuToGps(const QVector3D &enu, const QGeoCoordinate &ref);

} // namespace QGCGeo

#include "nmea_parser.h"
#include <stdio.h>
#include <string.h>

/*
 * GPRMC:
 *  GPRMC - Recommended minimum specific GPS/Transit data
 * 
 * Example GPRMC string:
 *  $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
 * 
 * Functions:
 *  is_gprmc() returns 1 if the string is a GPRMC string, 0 otherwise
 *  parse_gprmc() parses the GPRMC string and stores the data in the data struct
 * 
 * Data struct:
 *  struct gprmc_data
 *      - time: time in UTC (hhmmss.ss) [char[10]]
 *      - status: status (A=active or V=void) [char]
 *      - latitude: latitude (ddmm.mmmm) [char[10]]
 *      - latitude_direction: latitude direction (N=north or S=south) [char]
 *      - longitude: longitude (dddmm.mmmm) [char[10]]
 *      - longitude_direction: longitude direction (E=east or W=west) [char]
 *      - speed: speed over ground in knots (000.0-999.9) [char[10]]
 *      - course: course over ground (degrees) (000.0-359.9) [char[10]]
 *      - date: date (ddmmyy) [char[10]]
 *      - magnetic_variation: magnetic variation (degrees) [char[10]]
 *      - magnetic_variation_direction: magnetic variation direction (E=east or W=west) [char]
 */ 

int is_gprmc(char *nmea) {
    if (strncmp(nmea, "$GPRMC", (unsigned long)6) == 0) {
        return 1;
    }
    return 0;
}

int parse_gprmc(char *nmea, struct gprmc_data *data) {
    // Parse the NMEA data and store it in the data struct
    if (!is_gprmc(nmea)) {
        return -1;
    }
    // Example parsing code for GPRMC data
    sscanf(nmea, "$GPRMC,%[^,],%c,%[^,],%c,%[^,],%c,%[^,],%[^,],%[^,],%[^,],%c",
           data->time, &data->status, data->latitude, &data->latitude_direction,
           data->longitude, &data->longitude_direction, data->speed, data->course,
           data->date, data->magnetic_variation, &data->magnetic_variation_direction);
    return 0;
}

/*
 * GPVTG:
 *  GPVTG - Track Made Good and Ground Speed
 * 
 * Example GPVTG string:
 *  $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
 * 
 * Functions:
 *  is_gpvtg() returns 1 if the string is a GPVTG string, 0 otherwise
 *  parse_gpvtg() parses the GPVTG string and stores the data in the data struct
 * 
 * Data struct:
 *  struct gpvtg_data
 *      - true_track: true track made good (degrees) (000.0-359.9) [char[10]]
 *      - true_track_indicator: true track made good indicator (T) [char]
 *      - magnetic_track: magnetic track made good (degrees) (000.0-359.9) [char[10]]
 *      - magnetic_track_indicator: magnetic track made good indicator (M) [char]
 *      - ground_speed_knots: ground speed in knots (000.0-999.9) [char[10]]
 *      - ground_speed_knots_unit: ground speed in knots unit (N) [char]
 *      - ground_speed_kph: ground speed in kilometers per hour (000.0-999.9) [char[10]]
 *      - ground_speed_kph_unit: ground speed in kilometers per hour unit (K) [char]
 */ 

int is_gpvtg(char *nmea) {
    if (strncmp(nmea, "$GPVTG", (unsigned long)6) == 0) {
        return 1;
    }
    return 0;
}

int parse_gpvtg(char *nmea, struct gpvtg_data *data) {
    // Parse the NMEA data and store it in the data struct
    if (!is_gpvtg(nmea)) {
        return -1;
    }
    // Example parsing code for GPVTG data
    sscanf(nmea, "$GPVTG,%[^,],%c,%[^,],%c,%[^,],%c,%[^,],%c",
           data->true_track, &data->true_track_indicator, data->magnetic_track,
           &data->magnetic_track_indicator, data->ground_speed_knots,
           &data->ground_speed_knots_unit, data->ground_speed_kph,
           &data->ground_speed_kph_unit);
    return 0;
}

/*
 * GPGGA:
 *  GPGGA - Global Positioning System Fix Data
 * 
 * Example GPGGA string:
 *  $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
 * 
 * Functions:
 *  is_gpgga() returns 1 if the string is a GPGGA string, 0 otherwise
 *  parse_gpgga() parses the GPGGA string and stores the data in the data struct
 * 
 * Data struct:
 *  struct gpgga_data
 *      - time: time in UTC (hhmmss.ss) [char[10]]
 *      - latitude: latitude (ddmm.mmmm) [char[10]]
 *      - latitude_direction: latitude direction (N=north or S=south) [char]
 *      - longitude: longitude (dddmm.mmmm) [char[10]]
 *      - longitude_direction: longitude direction (E=east or W=west) [char]
 *      - fix_quality: GPS fix quality (0=invalid, 1=GPS fix, 2=DGPS fix) [int]
 *      - num_satellites: number of satellites in use (00-12) [int]
 *      - hdop: horizontal dilution of precision (0.5-99.9) [float]
 *      - altitude: altitude above mean sea level (meters) [float]
 *      - altitude_unit: altitude unit (M) [char]
 *      - geoid_height: height of geoid above WGS84 ellipsoid (meters) [float]
 *      - geoid_height_unit: geoid height unit (M) [char]
 */ 

int is_gpgga(char *nmea) {
    if (strncmp(nmea, "$GPGGA", (unsigned long)6) == 0) {
        return 1;
    }
    return 0;
}

int parse_gpgga(char *nmea, struct gpgga_data *data) {
    // Parse the NMEA data and store it in the data struct
    if (!is_gpgga(nmea)) {
        return -1;
    }
    // Example parsing code for GPGGA data
    sscanf(nmea, "$GPGGA,%[^,],%[^,],%c,%[^,],%c,%d,%d,%f,%f,%c,%f,%c",
           data->time, data->latitude, &data->latitude_direction, data->longitude,
           &data->longitude_direction, &data->fix_quality, &data->num_satellites,
           &data->hdop, &data->altitude, &data->altitude_unit, &data->geoid_height,
           &data->geoid_height_unit);
    return 0;
}

/*
 * Support Functions
 */

int check_nmea_type(char *input) {
    if (is_gpgga(input)) {
        return GPGGA;
    } else if (is_gprmc(input)) {
        return GPRMC;
    } else if (is_gpvtg(input)) {
        return GPVTG;
    } else {
        return UNKNOWN;
    }
}

int parse_nmea(char *input, struct nmea_data *data) {
    int type = check_nmea_type(input);
    if (type == GPGGA) {
        return (int)parse_gpgga(input, &data->gpgga);
    } else if (type == GPRMC) {
        return (int)parse_gprmc(input, &data->gprmc);
    } else if (type == GPVTG) {
        return (int)parse_gpvtg(input, &data->gpvtg);
    } else {
        return -1;
    }
}
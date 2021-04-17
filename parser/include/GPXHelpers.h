#ifndef GPX_HELPERS_H
#define GPX_HELPERS_H
#include "GPXParser.h"


/* Helpers API */

/** Function to put data in GPX object.
 *@pre GPX object and xmlNode exist, not null, valid
 *@param doc - a pointer to an GPX struct
 *@param node - xmlNode to read from
**/
void readGPX(xmlNode* node, GPXdoc* doc);

/** Function to count the number of GPXData in tracks.
 *@pre tracks List exists, is not null, and is valid
 *@return number of GPXData found.
 *@param tracks - a pointer to a tracks List
**/
int countGPXInTracks(List * tracks);

/** Function to count the number of GPXData in routess.
 *@pre routes List exists, is not null, and is valid
 *@return number of GPXData found.
 *@param tracks - a pointer to a routes List
**/
int countGPXInRoutes(List * routes);

/** Function to count the number of GPXData in waypoints.
 *@pre waypoints List exists, is not null, and is valid
 *@return number of GPXData found.
 *@param tracks - a pointer to a waypoints List
**/
int countGPXInWaypoints(List * waypoints);

/* ******************************* A2 Helpers *************************** */

//Module 1

/** Function to convert GPX to xml.
 *@pre GPXdoc exists, is not null, and is valid
 *@return xml ptr or NULL if the gpx is invalid.
 *@param gpx - a GPXDoc
**/
xmlDocPtr convertGPXToXml(GPXdoc * gpx);

/** Function to validate an xml tree.
 *@pre xmlDoc and schema file exists, is not null, and is valid
 *@return true if xml is valid, otherwise false.
 *@param doc - an xmlDoc 
 *@param gpxSchemaFile - schema file name
**/
bool validateXmlTree(xmlDoc * doc, char * gpxSchemaFile);

//Module 2

/** Function to find the total lenght of a list of waypoints.
 *@pre waypoints list exists, is not null, and is valid
 *@return the total distance.
 *@param wpts - a list of waypoints 
**/
float getListDistance(List * wpts);

/** Function to find the distance between 2 waypoints
 *@pre both waypoints exist, are not null, and are valid
 *@return the distance between the two waypoints
 *@param tmp1 - first waypoint
 *@param tmp2 - second waypoint
**/
float getDistance(Waypoint * tmp1, Waypoint * tmp2);

/** A dummy delete function for lists
**/
void dummyDelete(void * data);

//Bonus

/** Function to split JSON strings
 *@pre json string exist, are not null, and are valid
 *@return an array of strings
 *@param json - JSON string
 *@param item - total items in the json
**/
char **splitJSON(char * json, int items);
char **splitJSONList(char * json, int items);

/** Function that initializes a GPXDoc struct
 *@pre doc exist, are not null, and are valid
 *@param doc - GPXDoc to initialize
**/
void initializeGPX(GPXdoc * doc);

/** Function that initializes a Waypoint struct
 *@pre waypoint exist, are not null, and are valid
 *@param waypoint- Waypoint to initialize
**/
void initializeWaypoint(Waypoint * waypoint);

/** Function that initializes a Route struct
 *@pre route exist, are not null, and are valid
 *@param route - Route to initialize
**/
void initializeRoute(Route * route);


#endif
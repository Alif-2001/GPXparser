#include "GPXHelpers.h"

#define BUFSIZE 512

/** Function to put data in List of tracks.
 *@pre List of tracks and xmlNode exist, not null, valid
 *@param tracks - a pointer to a tracks List
 *@param node - xmlNode to read from
**/
void readTrk(xmlNode * node, List * tracks);

/** Function to put data in List of routes.
 *@pre List of routes and xmlNode exist, not null, valid
 *@param tracks - a pointer to a routes List
 *@param node - xmlNode to read from
**/
void readRte(xmlNode * node, List * routes);

/** Function to put data in List of segments.
 *@pre List of segements and xmlNode exist, not null, valid
 *@param tracks - a pointer to a segments List
 *@param node - xmlNode to read from
**/
void readTrkSeg(xmlNode * node, List * segments);

/** Function to put data in List of otherData.
 *@pre List of otherData and xmlNode exist, not null, valid
 *@param tracks - a pointer to a otherData List
 *@param node - xmlNode to read from
**/
void readOtherData(xmlNode * node, List * otherData);

/** Function to read a waypoint.
 *@pre List of otherData and xmlNode exist, not null, valid
 *@return 1 if waypoint was valid, 0 if not
 *@param wpt - a pointer to a Waypoint struct
 *@param node - xmlNode to read from
**/
int readWpt(xmlNode * node, Waypoint * wpt);

/** Function to iterate through nodes and read in waypoints.
 *@pre Waypoint object and node exists, not null, valid
 *@return the xmlNode we are at after the iteration
 *@param node - xmlNode to read from
 *@param waypoints - a pointer to an Waypoint struct
 *@param wptName - name of the waypoint (wpt or rtept or trkpt)
**/
xmlNode * readWpts(xmlNode * node, List * waypoints, char * wptName);

void initializeGPX(GPXdoc * doc){
    doc->creator = malloc(sizeof(char)*BUFSIZE);
    strcpy(doc->creator, "");
    strcpy(doc->namespace, "");
    doc->tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);
    doc->routes =  initializeList(&routeToString,&deleteRoute,&compareRoutes);
    doc->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
}

void initializeTrack(Track * track){
    track->name = malloc(sizeof(char)*BUFSIZE);
    strcpy(track->name,"");
    track->segments = initializeList(&trackSegmentToString,&deleteTrackSegment,&compareTrackSegments);
    track->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
}

void initializeRoute(Route * route){
    route->name = malloc(sizeof(char)*BUFSIZE);
    strcpy(route->name,"");
    route->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    route->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
}

void initializeTrackSeg(TrackSegment * trackSegment){
    trackSegment->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
}

void initializeWaypoint(Waypoint * waypoint){
    waypoint->name =  malloc(sizeof(char)*BUFSIZE);
    strcpy(waypoint->name,"");
    waypoint->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
}

void initializeGPXData(GPXData * data){
    strcpy(data->name, "");
    strcpy(data->value, "");
}

void readGPX(xmlNode * node, GPXdoc * doc) {
    if(node == NULL || doc == NULL){
        return;
    }

    initializeGPX(doc);
    xmlNode *cur_node = NULL;
    for(cur_node = node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if(strcmp((char *)cur_node->name,"gpx") == 0){
                xmlAttr *attr;
                for (attr = cur_node->properties; attr !=NULL; attr = attr->next) {
                    xmlNode *value = attr->children;
                    strcpy(doc->namespace,(char *)cur_node->ns->href);
                    if(strcmp((char *)attr->name,"version")==0){
                        char * ptr = NULL;
                        doc->version = strtod((char *)value->content,&ptr);
                    }else if(strcmp((char *)attr->name,"creator")==0){
                        strcpy(doc->creator,(char *)value->content);
                    }
                }
            }
        }
        xmlNode *children = NULL;
        for(children = cur_node->children; children!=NULL; children = children->next) {
            if(children->type == XML_ELEMENT_NODE) {
                if(strcmp((char *)children->name,"trk") == 0){
                    readTrk(children, doc->tracks);
                }else if(strcmp((char *)children->name,"rte") == 0){
                    readRte(children, doc->routes);
                }else if(strcmp((char *)children->name,"wpt") == 0){
                    children = readWpts(children, doc->waypoints, "wpt");
                }
            }
        }
    }
}


void readTrk(xmlNode * node, List * tracks){
    if(node == NULL || tracks == NULL){
        return;
    }
    
    Track *track = malloc(sizeof(Track));
    initializeTrack(track);
    xmlNode * children = NULL;
    for(children = node->children; children!=NULL; children = children->next) {
        if(children->type == XML_ELEMENT_NODE) {
            if(strcmp((char *)children->name,"name") == 0){
                if(children->children != NULL){
                    strcpy(track->name,(char *)children->children->content);
                }
            }else if(strcmp((char *)children->name,"trkseg") == 0){
                readTrkSeg(children,track->segments);
            }else{
                readOtherData(children,track->otherData);
            }
        }
    }
    insertBack(tracks, track);
}

void readRte(xmlNode * node, List * routes){
    if(node == NULL || routes == NULL){
        return;
    }

    Route *route = malloc(sizeof(Route));
    initializeRoute(route);
    xmlNode *children = NULL;
    for(children = node->children; children!=NULL; children = children->next) {
        if(children->type == XML_ELEMENT_NODE) {
            if(strcmp((char *)children->name,"name") == 0){
                if(children->children != NULL){
                    strcpy(route->name,(char *)children->children->content);
                }
            }else if(strcmp((char *)children->name,"rtept") == 0){
                children = readWpts(children, route->waypoints,"rtept");
            }else{
                readOtherData(children,route->otherData);
            }
        }
    }
    insertBack(routes, route);
}

void readTrkSeg(xmlNode * node, List * segments){
    if(node == NULL || segments == NULL){
        return;
    }

    TrackSegment *trackSegment = malloc(sizeof(TrackSegment));
    initializeTrackSeg(trackSegment);
    xmlNode * children = NULL;
    for(children = node->children; children!=NULL; children = children->next) {
        if(children->type == XML_ELEMENT_NODE) {
            if(strcmp((char *)children->name,"trkpt") == 0){
                children = readWpts(children, trackSegment->waypoints,"trkpt");
            }
        }
    }
    insertBack(segments,trackSegment);
}

void readOtherData(xmlNode * node, List * otherData){
    if(node == NULL || otherData == NULL){
        return;
    }

    GPXData * data = malloc(sizeof(GPXData)+BUFSIZE);
    initializeGPXData(data);
    if(node->children == NULL || node->name == NULL){
        free(data);
        return;
    }else{
        
        if(node != NULL && node->children != NULL){
            strcpy(data->name,(char *)node->name);
            strcpy(data->value,(char *)node->children->content);
        }
    }
    insertBack(otherData, data);
}

int readWpt(xmlNode * node, Waypoint * wpt){
    if(node == NULL || wpt == NULL){
        return 0;
    }

    xmlNode * children = NULL;
    for(children = node->children; children!=NULL; children = children->next) {
        if(children->type == XML_ELEMENT_NODE) {
            if(strcmp((char *)children->name,"name") == 0){
                if(children->children != NULL){
                    strcpy(wpt->name,(char *)children->children->content);
                }
            }else{
                readOtherData(children,wpt->otherData);
            }
        }
    }

    int check = 0;
    xmlAttr *attr;
    for(attr = node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        if(strcmp((char *)attr->name,"lon") == 0){
            check++;
            char * ptr = NULL;
            wpt->longitude = strtod((char *)value->content,&ptr);
        }else if(strcmp((char *)attr->name,"lat") == 0){
            check++;
            char * ptr = NULL;
            wpt->latitude = strtod((char *)value->content,&ptr);
        }

    }

    if(check == 2){
        return 1;
    }
    return 0;
}

xmlNode * readWpts(xmlNode * node, List * waypoints, char * wptName){
    if(node == NULL || waypoints == NULL){
        return NULL;
    }

    while(strcmp((char *)node->name,wptName) == 0){
        Waypoint *waypoint = malloc(sizeof(Waypoint));
        initializeWaypoint(waypoint);

        if(readWpt(node, waypoint)){
            insertBack(waypoints,waypoint);
        }else{
            free(waypoint->name);
            freeList(waypoint->otherData);
            free(waypoint);
        }

        node = node->next;
    }
    return node;
}

int countGPXInTracks(List * tracks){
    if(tracks == NULL){
        return 0;
    }
    int count = 0;
    void *elem;
    ListIterator iter = createIterator(tracks);
    while((elem = nextElement(&iter)) != NULL){
        Track * tmp = (Track *) elem;
        count += getLength(tmp->otherData);
        if(strcmp(tmp->name,"") != 0){
            count+=1;
        }
        void * elem2;
        ListIterator iter = createIterator(tmp->segments);
        while((elem2 = nextElement(&iter)) != NULL){
            TrackSegment * tmp = (TrackSegment *) elem2;
            count += countGPXInWaypoints(tmp->waypoints);
        }
    }
    return count;
}

int countGPXInRoutes(List * routes){
    if(routes == NULL){
        return 0;
    }
    int count = 0;
    void *elem;
    ListIterator iter = createIterator(routes);
    while((elem = nextElement(&iter)) != NULL){
        Route * tmp = (Route *) elem;
        count += getLength(tmp->otherData);
        if(strcmp(tmp->name,"") != 0){
            count+=1;
        }
        count += countGPXInWaypoints(tmp->waypoints);
    }
    return count;
}

int countGPXInWaypoints(List * waypoints){
    if(waypoints == NULL){
        return 0;
    }
    int count = 0;
    void *elem;
    ListIterator iter = createIterator(waypoints);
    while((elem = nextElement(&iter)) != NULL){
        Waypoint * tmp = (Waypoint *) elem;
        count += getLength(tmp->otherData);
        if(strcmp(tmp->name,"") != 0){
            count+=1;
        }
    }
    return count;
}


/* ******************************* A2 Helpers *************************** */

// Module 1
bool convertTrk(List * tracks, xmlNodePtr node);
bool convertRte(List * routes, xmlNodePtr node);
bool convertWpts(List * waypoints, xmlNodePtr node, char * name);
bool convertOtherData(List * otherData, xmlNodePtr node);
bool convertTrkSeg(List * segments, xmlNodePtr node);


bool convertGPX(GPXdoc * gpx, xmlNodePtr node){
    if(gpx == NULL || node == NULL){
        return false;
    }
    
    if(strcmp(gpx->namespace, "") == 0 || gpx->version == 0 
        || gpx->creator == NULL || strcmp(gpx->creator, "") == 0){
            return false;
    }

    xmlNsPtr namespace = NULL;
    namespace = xmlNewNs(node, (xmlChar *) gpx->namespace, NULL);
    xmlSetNs(node, namespace);
    char str[50];
    snprintf(str, 50, "%.1f", gpx->version);
    xmlNewProp(node, (xmlChar *)"version", (xmlChar *) str);
    xmlNewProp(node, (xmlChar *)"creator", (xmlChar *) gpx->creator);

    //xmlNewProp(node, (xmlChar *)"schemaLocation", (xmlChar *) gpx->namespace);

    if(convertWpts(gpx->waypoints, node, "wpt") == false){
        return false;
    }

    if(convertRte(gpx->routes, node) == false){
        return false;
    }

    if(convertTrk(gpx->tracks, node) == false){
        return false;
    }
    return true;
}

bool convertTrk(List * tracks, xmlNodePtr node){
    if(tracks == NULL){
        return false;
    }
    void * elem;
    ListIterator iter = createIterator(tracks);
    while((elem = nextElement(&iter)) != NULL){
        Track * tmp = (Track *) elem;
        xmlNodePtr track = NULL;
        track = xmlNewNode(node->ns, BAD_CAST "trk");
        if(tmp->name == NULL){
            xmlFreeNode(track);
            return false;
        }
        xmlNewChild(track, track->ns, (xmlChar *) "name", (xmlChar *) tmp->name);

        if(convertOtherData(tmp->otherData, track) == false){
            xmlFreeNode(track);
            return false;
        }

        if(convertTrkSeg(tmp->segments, track) == false){
            xmlFreeNode(track);
            return false;
        }
        xmlAddChild(node, track);
    }

    return true;
}
bool convertRte(List * routes, xmlNodePtr node){
    if(routes == NULL){
        return false;
    }
    void * elem;
    ListIterator iter = createIterator(routes);
    while((elem = nextElement(&iter)) != NULL){
        Route * tmp = (Route *) elem;
        xmlNodePtr route = NULL;
        route = xmlNewNode(node->ns, BAD_CAST "rte");
        if(tmp->name == NULL){
            xmlFreeNode(route);
            return false;
        }
        xmlNewChild(route, route->ns, (xmlChar *) "name", (xmlChar *) tmp->name);

        if(convertOtherData(tmp->otherData, route) == false){
            xmlFreeNode(route);
            return false;
        }

        if(convertWpts(tmp->waypoints, route, "rtept") == false){
            xmlFreeNode(route);
            return false;
        }
        xmlAddChild(node, route);
    }
    return true;
}

bool convertTrkSeg(List * segments, xmlNodePtr node){
    if(segments == NULL){
        return false;
    }
    void * elem;
    ListIterator iter = createIterator(segments);
    while((elem = nextElement(&iter)) != NULL){
        TrackSegment * tmp = (TrackSegment *) elem;
        xmlNodePtr trackSeg = NULL;
        trackSeg = xmlNewNode(node->ns, BAD_CAST "trkseg");
        if(convertWpts(tmp->waypoints, trackSeg, "trkpt") == false){
            xmlFreeNode(trackSeg);
            return false;
        }
        xmlAddChild(node, trackSeg);
    }
    return true;
}

bool convertOtherData(List * otherData, xmlNodePtr node){
    if(otherData == NULL){
        return false;
    }

    void * elem;
    ListIterator iter = createIterator(otherData);
    while((elem = nextElement(&iter)) != NULL){
        GPXData * tmp = (GPXData *) elem;
        if(strcmp(tmp->name, "") == 0 || strcmp(tmp->value, "") == 0){
            return false;
        }
        xmlNewChild(node, node->ns, (xmlChar *) tmp->name, (xmlChar *) tmp->value);
    }
    return true;
}

bool convertWpts(List * waypoints, xmlNodePtr node, char * name){
    if(waypoints == NULL){
        return false;
    }
    void * elem;
    ListIterator iter = createIterator(waypoints);
    while((elem = nextElement(&iter)) != NULL){
        Waypoint * tmp = (Waypoint *) elem;
        xmlNodePtr waypoint = NULL;
        waypoint = xmlNewNode(node->ns, BAD_CAST name);
        char str[50];
        snprintf(str, 50, "%f", tmp->latitude);
        xmlNewProp(waypoint, (xmlChar *) "lat", (xmlChar *) str);
        char str2[50];
        snprintf(str2, 50, "%f", tmp->longitude);
        xmlNewProp(waypoint, (xmlChar *) "lon", (xmlChar *) str2);
        if(tmp->name == NULL){
            xmlFreeNode(waypoint);
            return false;
        }
        if(strcmp(tmp->name, "") != 0){
            xmlNewChild(waypoint, waypoint->ns, (xmlChar *) "name", (xmlChar *) tmp->name);
        }

        if(convertOtherData(tmp->otherData, waypoint) == false){
            xmlFreeNode(waypoint);
            return false;
        }

        xmlAddChild(node, waypoint);
    }
    return true;
}

xmlDocPtr convertGPXToXml(GPXdoc * gpx){
    if(gpx == NULL){
        return NULL;
    }

    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
    doc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST "gpx");

    xmlDocSetRootElement(doc, root);
    if(convertGPX(gpx, root) == false){
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    return doc;
}

bool validateXmlTree(xmlDoc * doc, char * gpxSchemaFile){
    if(doc == NULL || gpxSchemaFile == NULL){
        return false;
    }
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    xmlLineNumbersDefault(1);       //Should I?
    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);


    xmlSchemaValidCtxtPtr check;
    int ret;
    bool validation;

    check = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(check, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(check, doc);
    if (ret == 0){
        validation = true;
    }else{
        validation = false;
    }

    xmlSchemaFreeValidCtxt(check);
    xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();

    return validation;
}

float getDistance(Waypoint * tmp1, Waypoint * tmp2){
    if(tmp1 == NULL || tmp2 == NULL){
        return 0;
    }
    float R = 6371e3;
    float r1 = tmp1->latitude * M_PI / 180;
    float r2 = tmp2->latitude * M_PI / 180;
    float d1 = (tmp2->latitude - tmp1->latitude) * M_PI / 180;
    float d2 = (tmp2->longitude - tmp1->longitude) * M_PI / 180;

    float a = sin(d1/2) * sin(d1/2) + cos(r1) * cos(r2) * sin(d2/2) * sin(d2/2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}

float getListDistance(List * wpts){
    if(wpts == NULL){
        return 0;
    }

    float totalDistance = 0;
    void * elem1;
    ListIterator iter = createIterator(wpts);
    if((elem1 = nextElement(&iter)) != NULL){
        void * elem2;
        while((elem2 = nextElement(&iter)) != NULL){
            Waypoint * tmp1 = (Waypoint *) elem1;
            Waypoint * tmp2 = (Waypoint *) elem2;
            
            totalDistance += getDistance(tmp1, tmp2);
            elem1 = elem2;
        }
    }
    return totalDistance;
}

void dummyDelete(void * data){}


char **splitJSON(char * json, int items){
    char **tokens = malloc(items * sizeof(char*));
    char * token;
    token = strtok(json, "\":,{}");
    int i = 0;
    while(token != NULL){
        tokens[i] = token;
        i++;
        token = strtok(NULL,"\":,{}");
    }
    tokens[i] = NULL;
    return tokens;
}

char **splitJSONList(char * json, int items){
    char **tokens = malloc(items * sizeof(char*));
    char * token;
    token = strtok(json, ",\"{}:[]");
    int i = 0;
    while(token != NULL){
        tokens[i] = token;
        i++;
        token = strtok(NULL,",\"{}:[]");
    }
    tokens[i] = NULL;
    return tokens;
}

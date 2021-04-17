#include "GPXParser.h"
#include "GPXHelpers.h"


GPXdoc* createGPXdoc(char* fileName) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    GPXdoc *gpx = malloc(sizeof(GPXdoc));

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        fprintf(stderr,"error: Could not parse file %s\n", fileName);
        free(gpx);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc);
    readGPX(root_element, gpx);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return gpx;
}

char* GPXdocToString(GPXdoc* doc) {
    char *tmpStr;
    GPXdoc * tmp;
    int len;

    if(doc == NULL){
        return NULL;
    }

    tmp = doc;
    len = strlen(tmp->namespace) + strlen(tmp->creator) + 20 + 25; //23
    tmpStr = (char*)malloc(sizeof(char)*len);
    sprintf(tmpStr,"%s\nversion: %f\ncreator: %s",tmp->namespace,tmp->version,tmp->creator);
    
    char * text = toString(tmp->waypoints);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);

    free(text);

    text = toString(tmp->routes);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);

    free(text);

    text = toString(tmp->tracks);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);

    free(text);

    return tmpStr;
}

void deleteGPXdoc(GPXdoc* doc) {
    if (doc == NULL){
        return;
    }
    
    free(doc->creator);

    freeList(doc->routes);    
    freeList(doc->waypoints);
    freeList(doc->tracks);
    free(doc);
}

int getNumWaypoints(const GPXdoc* doc) {
    if(doc == NULL){
        return 0;
    }

    return getLength(doc->waypoints);
}

int getNumRoutes(const GPXdoc* doc) {
    if(doc == NULL){
        return 0;
    }
    return getLength(doc->routes);
}

int getNumTracks(const GPXdoc* doc){
    if(doc == NULL){
        return 0;
    }
    return getLength(doc->tracks);
}

int getNumSegments(const GPXdoc* doc) {
    if(doc == NULL){
        return 0;
    }
    int count = 0;
    void *elem;
    ListIterator iter = createIterator(doc->tracks);
    while((elem = nextElement(&iter)) != NULL){
        Track * tmp = (Track *) elem;
        count += getLength(tmp->segments);
    }
    return count;
}

int getNumGPXData(const GPXdoc* doc) {
    if(doc == NULL){
        return 0;
    }
    int count = 0;
    count += countGPXInTracks(doc->tracks);
    count += countGPXInRoutes(doc->routes);
    count += countGPXInWaypoints(doc->waypoints);

    return count;
}

Waypoint* getWaypoint(const GPXdoc* doc, char* name) {
    if(doc == NULL){
        return NULL;
    }
    Waypoint * store = NULL;
    void * data;
    ListIterator iter = createIterator(doc->waypoints);
    while((data = nextElement(&iter)) != NULL){
        Waypoint * tmp = (Waypoint *) data;
        if(strcmp(tmp->name,name)==0){
            store = tmp;
        }
    }
    return store;
}

Track* getTrack(const GPXdoc* doc, char* name) {
    if(doc == NULL){
        return NULL;
    }
    Track * store = NULL;
    void * data;
    ListIterator iter = createIterator(doc->tracks);
    while((data = nextElement(&iter)) != NULL){
        Track * tmp = (Track *) data;
        if(strcmp(tmp->name,name)==0){
            store = tmp;
        }
    }
    return store;
}

Route* getRoute(const GPXdoc* doc, char* name) {
    if(doc == NULL){
        return NULL;
    }
    Route * store = NULL;
    void * data;
    ListIterator iter = createIterator(doc->routes);
    while((data = nextElement(&iter)) != NULL){
        Route * tmp = (Route *) data;
        if(strcmp(tmp->name,name)==0){
            store = tmp;
        }
    }
    return store;
}

void deleteGpxData( void* data){
    GPXData * tmpGPXData;

    if(data == NULL) {
        return;
    }
    tmpGPXData = (GPXData*) data;

    free(tmpGPXData);
}

char* gpxDataToString( void* data){
    char * tmpStr;
    GPXData * tmpGPXData;
    int len;

    if(data == NULL) {
        return NULL;
    }

    tmpGPXData = (GPXData *) data;
    len = strlen(tmpGPXData->name) + strlen(tmpGPXData->value) + 4;
    tmpStr = (char*)malloc(sizeof(char)*len);
    sprintf(tmpStr,"\t%s: %s",tmpGPXData->name, tmpGPXData->value);

    return tmpStr;
}

int compareGpxData(const void *first, const void *second){
    
    /* if(first == NULL || second == NULL){
        return 1;
    }

    int i = 0;
    GPXData * tmp1 = (GPXData*) first;
    GPXData * tmp2 = (GPXData*) second;

    i += strcmp(tmp1->name,tmp2->name);
    i += strcmp(tmp1->value,tmp1->value); 
    return i;*/

    return 0;
}

void deleteWaypoint(void* data){
    Waypoint * tmpWpt;

    if(data == NULL){
        return;
    }
    tmpWpt = (Waypoint*) data;

    free(tmpWpt->name);
    freeList(tmpWpt->otherData);
    free(tmpWpt);
}

char* waypointToString( void* data){
    char * tmpStr;
    Waypoint * tmp;
    int len;

    if(data == NULL){
        return NULL;
    }

    tmp = (Waypoint *) data;
    len = strlen(tmp->name) + 64 + 64 + 16;
    tmpStr = (char*)malloc(sizeof(char)*len);
    sprintf(tmpStr,"\t\tlat: %f, lon: %f %s",tmp->latitude,tmp->longitude,tmp->name);
    
    char * text = toString(tmp->otherData);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);
    free(text);

    return tmpStr;
}

int compareWaypoints(const void *first, const void *second){
    return 0;
}

void deleteRoute(void* data){
    Route * tmpRoute;

    if(data == NULL) {
        return;
    }

    tmpRoute = (Route*) data;
    free(tmpRoute->name);
    freeList(tmpRoute->waypoints);
    freeList(tmpRoute->otherData);
    free(tmpRoute);
}

char* routeToString(void* data){
    char * tmpStr;
    Route * tmp;
    int len;
    if(data == NULL){
        return NULL;
    }
    tmp = (Route *) data;
    len = strlen(tmp->name) + 8;
    tmpStr = (char*)malloc(sizeof(char)*len);
    sprintf(tmpStr,"Route: %s",tmp->name);

    char * text = toString(tmp->otherData);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);

    free(text);

    text = toString(tmp->waypoints);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);
    
    free(text);

    return tmpStr;
}

int compareRoutes(const void *first, const void *second){
    return 0;
}

void deleteTrackSegment(void* data){
    TrackSegment * tmpSeg;

    if(data == NULL){
        return;
    }

    tmpSeg = (TrackSegment*) data;
    freeList(tmpSeg->waypoints);
    free(tmpSeg);
}

char* trackSegmentToString(void* data){
    char * tmpStr;
    TrackSegment * tmp;
    int len;

    if(data == NULL){
        return NULL;
    }

    tmp = (TrackSegment *) data;
    len = 16; 
    tmpStr = (char*)malloc(sizeof(char)*len);
    sprintf(tmpStr,"Track Segment: ");

    char * text = toString(tmp->waypoints);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);

    free(text);
    return tmpStr;
}

int compareTrackSegments(const void *first, const void *second){
    return 0;
}

void deleteTrack(void* data){
    Track * tmpTrk;

    if(data == NULL) {
        return;
    }

    tmpTrk = (Track*) data;
    free(tmpTrk->name);
    freeList(tmpTrk->segments);
    freeList(tmpTrk->otherData);
    free(tmpTrk);
}

char* trackToString(void* data){
    char * tmpStr;
    Track * tmp;
    int len;

    if(data == NULL){
        return NULL;
    }

    tmp = (Track *) data;
    len = strlen(tmp->name) + 8;
    tmpStr = (char*)malloc(sizeof(char)*len);
    sprintf(tmpStr,"Track: %s",tmp->name);

    char * text = toString(tmp->otherData);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);

    free(text);

    text = toString(tmp->segments);
    len += strlen(text);
    len++;
    tmpStr = (char*) realloc(tmpStr, len);
    strcat(tmpStr, text);
    
    free(text);

    return tmpStr;
}

int compareTracks(const void *first, const void *second){
    return 0;
}

/* ******************************* A2 functions  *************************** */

//Module 1

GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        fprintf(stderr,"error: Could not parse file %s\n", fileName);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    bool validation = validateXmlTree(doc, gpxSchemaFile);

    if(validation == true){
        GPXdoc *gpx = malloc(sizeof(GPXdoc));
        root_element = xmlDocGetRootElement(doc);
        readGPX(root_element, gpx);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return gpx;
    }

    fprintf(stderr,"Validation failed for %s\n", fileName);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return NULL;
}

bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){
    if(doc == NULL || gpxSchemaFile == NULL){
        return false;
    }

    xmlDocPtr xml = convertGPXToXml(doc);  //converts and checks for constraints
    if(xml == NULL){
        return false;
    }
    
    bool validation = validateXmlTree(xml, gpxSchemaFile);
    if(validation == false){
        xmlFreeDoc(xml);
        xmlCleanupParser();
        return false;
    }

    xmlFreeDoc(xml);
    xmlCleanupParser();
    return true;
}

bool writeGPXdoc(GPXdoc* doc, char* fileName){
    if(doc == NULL || fileName == NULL){
        return false;
    }

    xmlDocPtr xml = convertGPXToXml(doc);
    if(xml == NULL){
        return false;
    }
    xmlSaveFormatFileEnc(fileName, xml, "UTF-8", 1);
    xmlFreeDoc(xml);
    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}

//Module 2

float round10(float len){
    float store = fmod(len,10);
    return ((store >= 5) ? len - store + 10 : len - store);
}

float getRouteLen(const Route *rt){
    if(rt == NULL){
        return 0;
    }
    float totalDistance = getListDistance(rt->waypoints);
    return totalDistance;
}

float getTrackLen(const Track *tr){
    if(tr == NULL){
        return 0;
    }
    float totalDistance = 0;
    void * elem1;
    ListIterator iter = createIterator(tr->segments);
    if((elem1 = nextElement(&iter)) != NULL){
        TrackSegment * tmp = (TrackSegment *) elem1;
        totalDistance += getListDistance(tmp->waypoints);
        void * elem2;
        while((elem2 = nextElement(&iter)) != NULL){
            TrackSegment * tmp1 = (TrackSegment *) elem1;
            TrackSegment * tmp2 = (TrackSegment *) elem2;
            totalDistance += getListDistance(tmp2->waypoints);
            totalDistance += getDistance(getFromBack(tmp1->waypoints), getFromFront(tmp2->waypoints));
            elem1 = elem2;
        }
    }
    return totalDistance;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta){
    if(doc == NULL || len < 0 || delta < 0){
        return 0;
    }
    int count = 0;
    void * elem;
    ListIterator iter = createIterator(doc->routes);
    while((elem = nextElement(&iter)) != NULL){
        Route * tmp = (Route *) elem;
        if( fabs(len - getRouteLen(tmp)) <= delta){
            count++;
        }
    }
    return count;
}

int numTracksWithLength(const GPXdoc* doc, float len, float delta){
    if(doc == NULL || len < 0 || delta < 0){
        return 0;
    }
    int count = 0;
    void * elem;
    ListIterator iter = createIterator(doc->tracks);
    while((elem = nextElement(&iter)) != NULL){
        Track * tmp = (Track *) elem;
        if( fabs(len - getTrackLen(tmp)) <= delta){
            count++;
        }
    }
    return count;
}

bool isLoopRoute(const Route* route, float delta){
    if(route == NULL || delta < 0){
        return false;
    }

    if(getLength(route->waypoints) >= 4){
        if( fabs(getDistance((Waypoint*) getFromFront(route->waypoints), (Waypoint*) getFromBack(route->waypoints))) <= delta){
            return true;
        }
    }
    return false;
}

bool isLoopTrack(const Track *tr, float delta){
    if(tr == NULL || delta < 0){
        return false;
    }
    int count = 0;
    void * elem;
    ListIterator iter = createIterator(tr->segments);
    while((elem = nextElement(&iter)) != NULL){
        TrackSegment * tmp = (TrackSegment *) elem;
        count += getLength(tmp->waypoints);
    }

    if(count >= 4){
        if((Waypoint *)getFromFront(((TrackSegment *)getFromFront(tr->segments))->waypoints) != NULL && 
            (Waypoint *)getFromBack(((TrackSegment *)getFromBack(tr->segments))->waypoints) != NULL){
            if(fabs(getDistance((Waypoint *)getFromFront(((TrackSegment *)getFromFront(tr->segments))->waypoints), 
                                (Waypoint *)getFromBack(((TrackSegment *)getFromBack(tr->segments))->waypoints)))
                                <= delta){
                                    return true;
            }
        }
    }

    return false;
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
    if(doc == NULL){
        return NULL;
    }
    List * bRoutes = initializeList(&routeToString, &dummyDelete, &compareRoutes);

    Waypoint * source = malloc(sizeof(Waypoint));
    source->latitude = sourceLat;
    source->longitude = sourceLong;
    Waypoint * dest = malloc(sizeof(Waypoint));
    dest->latitude = destLat;
    dest->longitude = destLong;

    void * elem;
    ListIterator iter = createIterator(doc->routes);
    while((elem = nextElement(&iter)) != NULL){
        Route * tmp = (Route *) elem;
        if( (fabs(getDistance(source, getFromFront(tmp->waypoints))) <= delta &&
            fabs(getDistance(dest, getFromBack(tmp->waypoints))) <= delta ) || 
            (fabs(getDistance(dest, getFromFront(tmp->waypoints))) <= delta &&
            fabs(getDistance(source, getFromBack(tmp->waypoints))) <= delta)){
            insertBack(bRoutes, tmp);
        }
    }
    free(source); free(dest);
    if(getLength(bRoutes) == 0){
        freeList(bRoutes);
        return NULL;
    }else{
        return bRoutes;
    }
}

List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
    if(doc == NULL){
        return NULL;
    }
    List * bTracks = initializeList(&trackToString, &dummyDelete, &compareTracks);
    Waypoint * source = malloc(sizeof(Waypoint));
    source->latitude = sourceLat;
    source->longitude = sourceLong;
    Waypoint * dest = malloc(sizeof(Waypoint));
    dest->latitude = destLat;
    dest->longitude = destLong;

    void * elem;
    ListIterator iter = createIterator(doc->tracks);
    while((elem = nextElement(&iter)) != NULL){
        Track * tmp = (Track *) elem;
        if( (fabs(getDistance(source, getFromFront(((TrackSegment *)getFromFront(tmp->segments))->waypoints))) <= delta &&
            fabs(getDistance(dest, getFromBack(((TrackSegment *)getFromBack(tmp->segments))->waypoints))) <= delta) ||
            (fabs(getDistance(dest, getFromFront(((TrackSegment *)getFromFront(tmp->segments))->waypoints))) <= delta &&
            fabs(getDistance(source, getFromBack(((TrackSegment *)getFromBack(tmp->segments))->waypoints))) <= delta) ){
            insertBack(bTracks, tmp);
        }
    }

    free(source); free(dest);
    if(getLength(bTracks) == 0){
        freeList(bTracks);
        return NULL;
    }else{
        return bTracks;
    }

}

// Module 3
char* waypointToJSON(const Waypoint * waypoint){
    char * json;
    int len = 0;
    if(waypoint == NULL){
        json = malloc(sizeof(char)*5);
        sprintf(json,"{}");
        return json;
    }
    len += strlen(waypoint->name) + 256;
    json = malloc(sizeof(char) * len);
    sprintf(json,"{\"name\": \"%s\", \"lon\": %f, \"lat\": %f}", waypoint->name, waypoint->longitude, waypoint->latitude);
    return json;
}

char* waypointListToJSON(const List * list){
    char * json;
    int len = 0, count = 0;

    if(list == NULL){
        json = (char*) malloc(sizeof(char)*5);
        sprintf(json,"[]");
        return json;
    }
    int length = getLength((List*) list);
    len += 2;
    json = (char*) malloc(sizeof(char)*len);
    sprintf(json, "[");
    char * text = NULL;
    void * elem;
    ListIterator iter = createIterator((List*) list);
    while((elem = nextElement(&iter)) != NULL){
        Waypoint * tmp = (Waypoint *) elem;
        text = waypointToJSON(tmp);
        len += strlen(text);
        len++;
        json = (char *) realloc(json, len);
        strcat(json, text);
        count++;
        if(count != length){
            len += 2;
            json = (char *) realloc(json, len);
            strcat(json,",");
        }
        free(text);
    }
    len += 2;
    json = (char *) realloc(json, len);
    strcat(json,"]");
    return json;
}

char* otherDataToJSON(const GPXData * other){
    char * json;
    int len = 0;
    if(other == NULL){
        json = malloc(sizeof(char)*5);
        sprintf(json,"{}");
        return json;
    }
    len += strlen(other->name) + 250 + 10;
    json = malloc(sizeof(char) * len);
    sprintf(json,"{\"%s\": \"%s\"}", other->name, other->value);
    return json;
}

char* otherListToJSON(const List * list){
    char * json;
    int len = 0, count = 0;

    if(list == NULL){
        json = (char*) malloc(sizeof(char)*5);
        sprintf(json,"[]");
        return json;
    }
    int length = getLength((List*) list);
    len += 2;
    json = (char*) malloc(sizeof(char)*len);
    sprintf(json, "[");
    char * text = NULL;
    void * elem;
    ListIterator iter = createIterator((List*) list);
    while((elem = nextElement(&iter)) != NULL){
        GPXData * tmp = (GPXData *) elem;
        text = otherDataToJSON(tmp);
        len += strlen(text);
        len++;
        json = (char *) realloc(json, len);
        strcat(json, text);
        count++;
        if(count != length){
            len += 2;
            json = (char *) realloc(json, len);
            strcat(json,",");
        }
        free(text);
    }
    len += 2;
    json = (char *) realloc(json, len);
    strcat(json,"]");
    return json;
}

char* trackToJSON(const Track *tr){
    char * json;
    int len = 0;
    if(tr == NULL){
        json = (char*) malloc(sizeof(char)*3);
        sprintf(json,"{}");
        return json;
    }

    len += strlen(tr->name) + 64 + 5 + 58; 
    json = (char*) malloc(sizeof(char) * len);

    int count = 0;
    void * elem;
    ListIterator iter = createIterator(tr->segments);
    while((elem = nextElement(&iter)) != NULL){
        TrackSegment * tmp = (TrackSegment *) elem;
        count += getLength(tmp->waypoints);
    }

    sprintf(json,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s,\"other\":", tr->name, count,
            round10(getTrackLen(tr)), isLoopTrack(tr, 10) ? "true" : "false");
    

    char * other = otherListToJSON(tr->otherData);
    len += strlen(other) + 5;
    json = realloc(json, len);
    strcat(json, other);
    strcat(json, "}");
    free(other);
    return json;
}

char* routeToJSON(const Route *rt){
    char * json;
    int len = 0;

    if(rt == NULL){
        json = malloc(sizeof(char)*5);
        sprintf(json,"{}");
        return json;
    }

    len += strlen(rt->name) + 64 + 64 + 5 + 50;
    json = malloc(sizeof(char) * len);
    sprintf(json,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s,\"other\":", rt->name, getLength(rt->waypoints),
            round10(getRouteLen(rt)), isLoopRoute(rt, 10) ? "true" : "false");
    
    char * other = otherListToJSON(rt->otherData);
    len += strlen(other) + 5;
    json = realloc(json, len);
    strcat(json, other);

    len += 15;
    json = realloc(json, len);
    strcat(json, ",\"points\":");

    free(other);
    other = waypointListToJSON(rt->waypoints);
    len += strlen(other) + 5;
    json = realloc(json, len);
    strcat(json, other);

    strcat(json, "}");
    free(other);

    return json;
}

char* routeListToJSON(const List *list){
    char * json;
    int len = 0, count = 0;

    if(list == NULL){
        json = malloc(sizeof(char)*5);
        sprintf(json,"[]");
        return json;
    }
    int length = getLength((List*) list);
    len += 2;
    json = malloc(sizeof(char)*len);
    sprintf(json, "[");
    char * text = NULL;
    void * elem;
    ListIterator iter = createIterator((List*) list);
    while((elem = nextElement(&iter)) != NULL){
        Route * tmp = (Route *) elem;
        text = routeToJSON(tmp);
        len += strlen(text);
        len++;
        json = (char *) realloc(json, len);
        strcat(json, text);
        count++;
        if(count != length){
            len += 2;
            json = (char *) realloc(json, len);
            strcat(json,",");
        }
        free(text);
    }
    len += 2;
    json = (char *) realloc(json, len);
    strcat(json,"]");
    return json;
}

char* trackListToJSON(const List *list){
    char * json;
    int len = 0, count = 0;

    if(list == NULL){
        json = (char*) malloc(sizeof(char)*3);
        sprintf(json,"[]");
        return json;
    }

    int length = getLength((List*) list);

    len += 2;
    json = (char*) malloc(sizeof(char)*len);
    sprintf(json, "[");
    char * text = NULL;
    void * elem;
    ListIterator iter = createIterator((List*) list);
    while((elem = nextElement(&iter)) != NULL){
        Track * tmp = (Track *) elem;
        text = trackToJSON(tmp);
        len += strlen(text);
        len++;
        json = (char *) realloc(json, len);
        strcat(json, text);
        count++;
        if(count != length){
            len += 2;
            json = (char *) realloc(json, len);
            strcat(json,",");
        }
        free(text);
    }
    len += 2;
    json = (char *) realloc(json, len);
    strcat(json,"]");
    return json;
}

char* GPXtoJSON(const GPXdoc* gpx){
    char * json;
    int len = 0;

    if(gpx == NULL){
        json = (char*) malloc(sizeof(char)*3);
        sprintf(json,"{}");
        return json;
    }

    len += strlen(gpx->creator) + 64 + 64 + 64 + 64 + 45;
    json = (char*) malloc(sizeof(char) * len);
    sprintf(json,"{\"version\":%.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", gpx->version, gpx->creator,
            getNumWaypoints(gpx), getNumRoutes(gpx), getNumTracks(gpx));
    
    return json;
}


//Bonus
void addWaypoint(Route *rt, Waypoint *pt){
    if(rt == NULL || pt == NULL){
        return;
    }
    if(pt->name != NULL && pt->otherData != NULL && rt->waypoints != NULL){
        insertBack(rt->waypoints, pt);
    }
}

void addRoute(GPXdoc* doc, Route* rt){
    if(rt == NULL || doc == NULL){
        return;
    }
    if(rt->name != NULL && rt->waypoints != NULL && rt->otherData != NULL && doc->routes != NULL){
        insertBack(doc->routes, rt);
    }
}

GPXdoc* JSONtoGPX(const char* gpxString){
    if(gpxString == NULL){
        return NULL;
    }
    GPXdoc * gpx = malloc(sizeof(GPXdoc));
    initializeGPX(gpx);
    if(strstr(gpxString, "version") != NULL && strstr(gpxString,"creator") != NULL){
        char ** split = splitJSON((char *) gpxString, 250);
        for(int i = 0; i < 4; i++){
            if(strcmp(split[i], "version") == 0){
                char * ptr;
                gpx->version = strtod(split[i+1],&ptr);
            }
            if(strcmp(split[i], "creator") == 0){
                strcpy(gpx->creator, split[i+1]);
            }
        }
        free(split);
    }
    strcpy(gpx->namespace, "http://www.topografix.com/GPX/1/1");
    return gpx;
}

Waypoint* JSONtoWaypoint(const char* gpxString){
    if(gpxString == NULL){
        return NULL;
    }
    Waypoint * wpt = malloc(sizeof(Waypoint));
    initializeWaypoint(wpt);
    if(strstr(gpxString, "lat") != NULL && strstr(gpxString,"lon") != NULL){
        char ** split = splitJSON((char *) gpxString, 250);
        for(int i = 0; i < 4; i++){
            if(strcmp(split[i], "lat") == 0){
                char * ptr;
                wpt->latitude = strtod(split[i+1],&ptr);
            }
            if(strcmp(split[i], "lon") == 0){
                char * ptr;
                wpt->longitude = strtod(split[i+1],&ptr);
            }
        }
        free(split);
    }
    return wpt;
}

Route* JSONtoRoute(const char* gpxString){
    if(gpxString == NULL){
        return NULL;
    }
    Route * rt = malloc(sizeof(Route));
    initializeRoute(rt);
    if(strstr(gpxString, "name") != NULL ){
        char ** split = splitJSON((char *)gpxString, 250);
        for(int i = 0; i < 2; i++){
            if(strcmp(split[i], "name") == 0){
                strcpy(rt->name, split[i+1]);
            }
        }
        free(split);
    }
    return rt;
}


//A3

char * GPXFileToJson(char * filename, char* gpxSchemaFile ){
    GPXdoc * doc = createValidGPXdoc(filename, gpxSchemaFile);
    char * string = GPXtoJSON(doc);
    deleteGPXdoc(doc);
    return string;
}

char * GPXFileToData(char * filename, char* gpxSchemaFile){
    GPXdoc * doc = createValidGPXdoc(filename, gpxSchemaFile);
    char * string = (char*) malloc(sizeof(char)*13);
    sprintf(string, "{\"routes\":");

    char * string1 = routeListToJSON(doc->routes);
    char * string2 = trackListToJSON(doc->tracks);
    string = (char *) realloc(string, strlen(string)+strlen(string1)+5);
    strcat(string, string1);

    string = (char *) realloc(string, strlen(string)+2+18);
    strcat(string, ",");
    strcat(string, "\"tracks\":");

    string = (char *) realloc(string, strlen(string)+strlen(string2)+5);
    strcat(string, string2);
    strcat(string, "}");

    free(string1);
    free(string2);
    deleteGPXdoc(doc);
    return string;
}

void createGPX(char * filename, char* gpxString){
    GPXdoc * doc = JSONtoGPX(gpxString);
    writeGPXdoc(doc, filename);
    deleteGPXdoc(doc);
}

void JSONtoWptList(char * wptList, int numWaypoints, Route * rt){
    char ** split = splitJSONList((char *)wptList, numWaypoints*4);
    for(int i = 0; i < numWaypoints*4; i++){
        if(strcmp(split[i], "lat") == 0){
            char * ptr;
            Waypoint * wpt = malloc(sizeof(Waypoint));
            initializeWaypoint(wpt);
            wpt->latitude = strtod(split[i+1],&ptr);
            wpt->longitude = strtod(split[i+3],&ptr);
            addWaypoint(rt, wpt);
        }
    }
    free(split);
}

int addRouteToGPX(char * filename, char * schema, char * routeString, char * wptsJSON, int numWaypoints){
    Route * rt = JSONtoRoute(routeString);
    GPXdoc * doc = createValidGPXdoc(filename, schema);
    JSONtoWptList(wptsJSON, numWaypoints, rt);
    addRoute(doc, rt);
    if(validateGPXDoc(doc, schema)){
        writeGPXdoc(doc, filename);
        deleteGPXdoc(doc);
        return 1;
    }else{
        deleteGPXdoc(doc);
        return 0;
    }
    return 0;
}

char * GPXFileFind(char * filename, char* gpxSchemaFile, double lat1, double lon1, double lat2, double lon2, double acc){
    GPXdoc * doc = createValidGPXdoc(filename, gpxSchemaFile);
    List * routes = getRoutesBetween(doc, lat1, lon1, lat2, lon2, acc);
    List * tracks = getTracksBetween(doc, lat1, lon1, lat2, lon2, acc);

    char * string = (char *) malloc(sizeof(char)*13);
    sprintf(string, "{\"routes\":");

    char * string1 = routeListToJSON(routes);
    char * string2 = trackListToJSON(tracks);

    string = (char *) realloc(string, strlen(string)+strlen(string1)+10);
    strcat(string, string1);

    string = realloc(string, strlen(string)+2+18);
    strcat(string, ",");

    strcat(string, "\"tracks\":");
    string = (char *) realloc(string, strlen(string)+strlen(string2)+10);
    strcat(string, string2);
    strcat(string, "}");

    free(string1);
    free(string2);
    freeList(routes);
    freeList(tracks);
    deleteGPXdoc(doc);
    return string;
}

int renameRoute(char * filename, char * gpxSchemaFile, char * routeName, char * newName){
    GPXdoc * doc = createValidGPXdoc(filename, gpxSchemaFile);
    Route * rt = getRoute(doc, routeName);
    
    if(rt != NULL){
        free(rt->name);
        rt->name = malloc(sizeof(char) * strlen(newName) + 5);
        strcpy(rt->name, newName);
        if(validateGPXDoc(doc, gpxSchemaFile)){
            writeGPXdoc(doc,filename);
            deleteGPXdoc(doc);
            return 1;
        }else{
            deleteGPXdoc(doc);
            return 0;
        }
    }
    Track * tr = getTrack(doc, routeName);
    if(tr != NULL){
        free(tr->name);
        tr->name = malloc(sizeof(char) * strlen(newName) + 5);
        strcpy(tr->name, newName);
        if(validateGPXDoc(doc, gpxSchemaFile)){
            writeGPXdoc(doc,filename);
            deleteGPXdoc(doc);
            return 1;
        }else{
            deleteGPXdoc(doc);
            return 0;
        }
    }


    deleteGPXdoc(doc);
    return 0;
}

int validateGPXFile(char * filename, char * gpxSchemaFile){
    GPXdoc * doc = createValidGPXdoc(filename, gpxSchemaFile);
    if(doc == NULL){
        free(doc);
        return 0;
    }
    free(doc);
    return 1;
}

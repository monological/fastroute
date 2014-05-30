/*
 * create_db.h - creates the graph.db
 *
 */

#ifndef CREATE_DB_H
#define CREATE_DB_H

enum CsvNodeFields {
    nodeId,
    xCoord,
    yCoord,
    nodeCost,
    arcNodeList,
};

enum CsvArcFields {
    arcId,
    isDirectional,
    isExcluded,
    toNodeId,
};

#endif

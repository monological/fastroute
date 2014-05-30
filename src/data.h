/*
 * data.h - Arc and node data structs.
 *
 */

#ifndef DATA_H
#define DATA_H


/*
 * NetData will not be serialized to disk,
 * because it was never meant to.
 *
 */
typedef struct {
    const char *name;
    uint32_t src_node_id;
    uint32_t *dst_node_ids;
    uint32_t num_dst_node_ids;
} NetData;

/*
 * NodeData, ArcData are meant to be serialized to disk.
 *
 */
typedef struct {
    uint32_t id;
    uint8_t x_coord;
    uint8_t y_coord;
    uint8_t cost;
    uint32_t num_arcnodes;
} NodeData;

typedef struct {
    uint32_t id;
    bool is_directional;
    bool is_excluded;
    uint32_t src_node_id;
    uint32_t dst_node_id;
} ArcData;


/*
 * Functions
 */

NetData *new_net_data(const char *name,
        uint32_t src_node_id,
        uint32_t *dst_node_ids,
        uint32_t num_dst_node_ids);

void free_net_data(NetData *data);

NodeData * new_node_data(uint32_t id,
        uint8_t x_coord,
        uint8_t y_coord,
        uint8_t cost,
        uint32_t num_arcnodes);

void free_node_data(NodeData *data);

ArcData * new_arc_data(uint32_t id,
        bool is_directional,
        bool is_excluded,
        uint32_t src_node_id,
        uint32_t dst_node_id);

void free_arc_data(ArcData *data);

#endif

#!/usr/bin/env python

import random
#import matplotlib.pyplot as plt
import networkx as nx

g = nx.random_geometric_graph(300, 0.2)
g = g.to_directed()

pos = nx.spring_layout(g)

src_dst_to_arc_id = {}
cur_arc_id = 0

with open('rand_graph.csv', 'w') as f:
    print >>f, '#<src_node_id>,<x_coord>,<y_coord>,<cost>,<arc_id>:<is_directional>:<is_excluded>:<dst_node_id>,...,EOL'
    for src_node in g.nodes():
        x = str(pos[src_node][0])
        y = str(pos[src_node][1])
        cost = str(random.randint(1, 10))
        line = [str(src_node), x, y, cost]
        for dst_node in g.edges(src_node):
            h = hash((src_node, dst_node))
            if not h in src_dst_to_arc_id:
                cur_arc_id += 1
                src_dst_to_arc_id[h] = cur_arc_id
            arc_id = src_dst_to_arc_id[h]
            arcnode = ':'.join([str(arc_id), 'f', 'f', str(dst_node[-1])])
            line.append(arcnode)
        line.append('EOL')
        line = ','.join(line)
        print >>f, line



with open('unrouted.csv', 'w') as f:
    print >>f, '#<name>,<src_node_id>,<num_dst_nodes>,<dst_node_id>,...,EOL'
    for i in range(1):
        name = '%s_net' % i
        src_node_id = str(random.randint(0, 10))
        line = [name, src_node_id]
        dst_nodes = []
        dst_nodes.append(str(random.randint(g.number_of_nodes()-10, g.number_of_nodes()-1)))

        line.append(str(len(dst_nodes)))
        line.extend(dst_nodes)
        line.append('EOL')
        line = ','.join(line)
        print >>f, line








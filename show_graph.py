#!/usr/bin/env python

""" Loads the graph.csv and the routed.csv and plots the graph.
"""

import matplotlib.pyplot as plt
import networkx as nx

g = nx.DiGraph()

pos = {}

arc_id_to_nodes = {}

unrouted_src_dst_to_cost = {}
nodelist = []
with open('rand_graph.csv') as f:
    for l in f:
        l = l.strip()
        if l.find('#') >= 0:
            continue
        sp = l.split(',')
        src_node_id = sp[0]
        x = sp[1]
        y = sp[2]
        cost = sp[3]
        nodelist.append(src_node_id)
        pos[src_node_id] = [float(x), float(y)]
        g.add_node(src_node_id)

        for arcnode in sp[4:]:
            if arcnode == 'EOL':
                continue
            spa = arcnode.split(':')
            arc_id = spa[0]
            is_dir = spa[1]
            is_ex = spa[2]
            dst_node_id = spa[3]
            unrouted_src_dst_to_cost[(src_node_id, dst_node_id)] = cost
            arc_id_to_nodes[arc_id] = (src_node_id, dst_node_id)
            g.add_edge(src_node_id, dst_node_id)

routed_src_dst_to_cost = {}
routed_nodelist = []
routed_edgelist = []
with open('routed.csv') as f:
    for l in f:
        l = l.strip()
        if l.find('#') >= 0:
            continue
        sp = l.split(',')
        for arc_id in sp[2:]:
            if arc_id == 'EOL':
                continue
            try:
                src_node_id, dst_node_id = arc_id_to_nodes[arc_id]
                routed_src_dst_to_cost[(src_node_id, dst_node_id)] = cost
                routed_nodelist.append(src_node_id)
                routed_nodelist.append(dst_node_id)
                routed_edgelist.append((src_node_id, dst_node_id))
            except KeyError:
                pass

g = g.to_undirected()

""" Draw all the nodes and edges.
"""
nx.draw_networkx_nodes(
    g,
    pos=pos,
    nodelist=nodelist,
    node_size=15,
    node_color='green',
)
nx.draw_networkx_edges(
    g,
    pos=pos,
    edge_color='black',
)
if False:
    nx.draw_networkx_edge_labels(
        g,
        pos=pos,
        edge_labels=unrouted_src_dst_to_cost,
        font_color='black',
        font_size=4,
        alpha=0.1,
    )

""" Draw the routed nodes and edges.
"""
nx.draw_networkx_nodes(
    g,
    pos=pos,
    nodelist=routed_nodelist,
    node_size=30,
    node_color='red',
)
nx.draw_networkx_edges(
    g,
    pos=pos,
    edgelist=routed_edgelist,
    edge_color='b',
    width=5.0,
)
nx.draw_networkx_edge_labels(
    g,
    pos=pos,
    edge_labels=routed_src_dst_to_cost,
    font_color='black',
    font_size=5,
)

plt.show()



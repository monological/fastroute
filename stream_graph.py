#!/usr/bin/env python

""" Loads the graph.csv and the routed.csv and plots the graph.
"""

import zmq
import sys
#from  multiprocessing import Process

import matplotlib
matplotlib.use('Qt4Agg')
import matplotlib.pyplot as plt
import networkx as nx








if __name__ == "__main__":
    port = "5556"
    if len(sys.argv) > 1:
        port =  sys.argv[1]
        port = int(port)
    #Process(target=client, args=(port,)).start()

    context = zmq.Context()
    print "Connecting to server..."
    socket = context.socket(zmq.PULL)
    socket.connect ("tcp://localhost:%s" % port)
    # Initialize poll set
    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)


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
                #is_dir = spa[1]
                #is_ex = spa[2]
                dst_node_id = spa[3]
                unrouted_src_dst_to_cost[(src_node_id, dst_node_id)] = cost
                arc_id_to_nodes[arc_id] = (src_node_id, dst_node_id)
                g.add_edge(src_node_id, dst_node_id)
    g = g.to_undirected()


    fig = plt.figure(1)
    axes = fig.add_subplot(111)
    plt.ion()
    plt.show(False)

    """ Draw all the nodes and edges.
    """
    nx.draw_networkx_nodes(
        g,
        pos=pos,
        ax=axes,
        nodelist=nodelist,
        node_size=15,
        node_color='green',
    )
    nx.draw_networkx_edges(
        g,
        pos=pos,
        ax=axes,
        edge_color='black',
    )

    background = fig.canvas.copy_from_bbox(axes.bbox)
    axes.hold(True)
    fig.show()
    fig.canvas.draw()


    # Work on requests from both server and publisher
    should_continue = True
    while should_continue:
        socks = dict(poller.poll())
        if socket in socks and socks[socket] == zmq.POLLIN:
            message = socket.recv()
            print "Recieved control command: %s" % message

            arc_ids = message.replace('S_','').replace('_E','')
            arc_ids = arc_ids.split(':')

            routed_nodelist = []
            routed_edgelist = []

            for arc_id in arc_ids:
                src_node_id, dst_node_id = arc_id_to_nodes[arc_id]
                routed_nodelist.append(src_node_id)
                routed_nodelist.append(dst_node_id)
                routed_edgelist.append((src_node_id, dst_node_id))

            #fig.canvas.restore_region(background)
            rows = nx.draw_networkx_nodes(
                g,
                pos=pos,
                ax=axes,
                nodelist=routed_nodelist,
                node_size=30,
                node_color='red',
            )
            #axes.draw_artist(rows)
            rows = nx.draw_networkx_edges(
                g,
                pos=pos,
                ax=axes,
                edgelist=routed_edgelist,
                edge_color='b',
                width=5.0,
            )
            #axes.draw_artist(rows)
            #axes.figure.canvas.blit(axes.bbox)
            fig.canvas.draw()
            if message == "Exit":
                print "Recieved exit command, client will stop recieving messages"
                should_continue = False





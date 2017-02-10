
## What is this?

Fastroute is a general purpose graph router written in C, which utilizes the A\* search algorithm.

## How to use it?

1. Build the code by running make.
2. A graph.csv needs to be generated, which defines the structure of your graph.
3. Create a database.db file, which takes the graph.csv and serializes the data to disk.
4. Create an unrouted.csv file listing what you wanted routed.
5. Run fastroute with the db, unrouted.csv and a path to a routed.csv file.
6. When fastroute is finished routing, it will write the routed paths it found in the provided routed.csv path.

## How do I demo it?

There are three helper python scripts that automatically generates a random graph,
routes a path and displays the graph. However, the code snippet below will run everything
for you.

Required Python modules:

* networkx
* matplotlib

To run everything, simply execute:

```bash
make all
./run.sh
```

## Terminology

The terminology used by fastroute is a little different than normal
graph theory parlance.

- Node refers to a vertex
- Arc refers to an edge

## CSV Specs

#### graph.csv

```csv
<src_node_id>,<x_coord>,<y_coord>,<cost>,<arc_id>:<is_directional>:<is_excluded>:<dst_node_id>,...,EOL
```

**Note:** arc_id is a unique id that indexes a (src_node, dst_node) pair.

#### unrouted.csv

```csv
<name>,<src_node_id>,<num_dst_nodes>,<dst_node_id>,...,EOL
```

## License

Copyright 2014 Monological.

fastroute is free software made available under the MIT License. For details see the LICENSE file.




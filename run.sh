#!/bin/sh

echo "Creating the graph..."
./create_graph.py
echo "Creating the db..."
./bin/create_db -c rand_graph.csv
echo "Routing nets..."
./bin/fastroute -d rand.db -u unrouted.csv -r routed.csv
echo "Showing the graph..."
./show_graph.py



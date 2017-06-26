// problem_c.cpp : Defines the entry point for the console application.
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

/*
This function reads a line of int values into a vector function and returns that
vector.
*/
vector<int> readlineofints(int count) {
    // Input values
    vector<int> linevalues(count);
    for (int j = 0; j < count; j++) {
        int val;
        cin >> val;
        linevalues[j] = val;
    }
    return linevalues; // Return line values as a vector
}

// Edge struct
struct edge {
    int target;
    long weight;

    //Default constructor
    edge() {}

    //Constructor
    edge(int target, long weight) :target(target), weight(weight) {}

    //Override comparator operator
    bool operator<(const edge& rhs) const {
        return weight < rhs.weight;
    }
};

class Reversed_Edge_Comparator {
public:
    bool operator()(const edge a, const edge b) {
        return (a.weight > b.weight);
    }
};

// Graph code, at least in original part, from
// http://www.geeksforgeeks.org/graph-implementation-using-stl-for-competitive-programming-set-2-weighted-graph/
/*
Adds an undirected edge of set weight to the graph
*/
void addUndirectedEdge(vector<vector<edge>> &adj_list, int u, int v, long weight) {
    edge edge1(v, weight);
    edge edge2(u, weight);
    adj_list[u].push_back(edge1); // Add the v edge and the weight to u's list
    adj_list[v].push_back(edge2); // Add the u edge and the weight to v's list
}

/*
Adds a directed edge of set weight to the graph
*/
void addDirectedEdge(vector<vector<edge>> &adj_list, int u, int v, long weight) {
    edge edge1(v, weight);
    adj_list[u].push_back(edge1); // Add the v edge and the weight to u's list
}

/*
Updates the weight of set edge
*/
void updateEdgeWeight(vector<vector<edge>> &adj_list, int u, int v, long new_weight) {
    for (edge &e : adj_list[u]) {
        if (e.target == v) {
            e.weight = new_weight;
        }
    }
}

/*
Gets the weight of indicated edge and returns it
*/
long const get_edge_weight(const vector<vector<edge>> &adj_list, int u, int v) {
    for (const edge &e : adj_list[u]) {
        if (e.target == v) { return e.weight; }
    }
    return -1;
}

bool dijkstra_nearest_stations(const vector<vector<edge>> &graph, int origin, int supermarket_list[10000], int(&supermarket_distances)[10000]) {
    vector<long> distance(graph.size(), 2147483647); //Initialize vertices list with lengths set to max
    vector<int> prev_node(graph.size(), -1); //Previous node initialized to -1

    priority_queue<edge, vector<edge>, Reversed_Edge_Comparator> to_visit;
    to_visit.push(edge(origin, 0)); //Push the origin with a 0 weight as the first item in the queue
    distance[origin] = 0;

    while (to_visit.size() > 0) { //While there are unvisited nodes left
        edge this_node = to_visit.top(); //Get next node
        to_visit.pop(); //Pop the node we just got

        //Iterate through edges of next node
        for (const edge &next_node : graph[this_node.target]) {
            if (distance[next_node.target] > (distance[this_node.target] + next_node.weight)) { //If the path to this edge's target is longer than the path to this node plus this edge's weight
                distance[next_node.target] = distance[this_node.target] + next_node.weight; //update the distance to the next node
                to_visit.push(edge(next_node.target, next_node.weight)); //Insert this edge into to_visit list with negative weight to reverse sort order
                prev_node[next_node.target] = this_node.target; //Update that node with the current one as the previous node
            }
        }
    }

    bool distance_found = false;
    //Iterate through supermarkets list
    for (int i = 0; i < graph.size(); i++) {
        if (supermarket_list[i] != -1) { //If current node has a supermarket
            if (distance[i] != 2147483647) { //If this distance is less than infinity
                supermarket_distances[i] = distance[i]; //Record the distance from the origin to this node
                distance_found = true; //Note that at least once distance was found
            }
        }
    }

    return distance_found;
}

void initialize_int_array(int(&array)[10000], int value) {
    for (int i = 0; i < 10000; i++) {
        array[i] = value;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);

    // get test case count
    int t;
    std::cin >> t;

    // loop over all the test cases
    for (int i = 1; i <= t; i++) {
        // Read in params
        vector<int> params = readlineofints(5);

        // Initialize adjacency list graph
        vector<vector<edge>> graph(params[0]);

        // Iterate over walking trails
        for (int j = 0; j < params[1]; j++) {
            // Input edge
            vector<int> this_edge = readlineofints(3);
            this_edge[0]--; //Decrement edge numberings for 0 indexing
            this_edge[1]--;
            addUndirectedEdge(graph, this_edge[0], this_edge[1], this_edge[2]); // Add edge, with weight, to adjacency list
        }

        int supermarkets[10000]; //Track the supermarkets and time in them required
        initialize_int_array(supermarkets, -1); //Initialize the array to -1

        for (int j = 0; j < params[2]; j++) {
            // Input station
            vector<int> read_line = readlineofints(2);
            supermarkets[read_line[0] - 1] = read_line[1]; //Add this time to the stations list (decrementing the index)
        }

        int supermarket_distances_from_origin[10000];
        initialize_int_array(supermarket_distances_from_origin, -1); //Initialize the array to -1
        bool from_origin = dijkstra_nearest_stations(graph, params[3] - 1, supermarkets, supermarket_distances_from_origin);

        int supermarket_distances_from_dest[10000];
        initialize_int_array(supermarket_distances_from_dest, -1); //Initialize the array to -1
        bool from_dest = dijkstra_nearest_stations(graph, params[4] - 1, supermarkets, supermarket_distances_from_dest);

        bool path_found = false;
        long shortest_time = 2147483647; //Initialize shortest distance
        //Iterate through supermarkets list
        for (int i = 0; i < params[0]; i++) {
            if ((supermarkets[i] != -1) && (supermarket_distances_from_origin[i] != -1) && (supermarket_distances_from_dest[i] != -1)) { //If current node has a supermarket and a path to it from origin and destination
                long this_time = supermarkets[i] + supermarket_distances_from_origin[i] + supermarket_distances_from_dest[i]; //Calculate the total time from origin, destination, and in supermarket
                if (this_time < shortest_time) { //If this supermarket's total time is shorter than any logged before
                    shortest_time = this_time; //Replace the shortest time with this one
                    path_found = true;
                }
            }
        }

        if (path_found) { //If a path was found to supermarkets from both the origin and the destination
            //Print result showing time in hours and minutes
            if (shortest_time % 60 < 10) {
                std::cout << "Case #" << i << ": " << shortest_time / 60 << ":0" << shortest_time % 60 << std::endl;
            }
            else {
                std::cout << "Case #" << i << ": " << shortest_time / 60 << ":" << shortest_time % 60 << std::endl;
            }
        }
        else { //If no path was found
            std::cout << "Case #" << i << ": impossible" << std::endl;  //Print result showing impossible
        }
    }
    return 0;
}
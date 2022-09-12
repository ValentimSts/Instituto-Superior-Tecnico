/* 
    PROJETO 2 -> ASA
Filipa Magalhaes - 99216
Tiago Quinteiro - 99336
*/

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <limits>

using namespace std;

typedef enum {
    WHITE, GRAY, BLACK, COMMON
} color;

struct Graph {
    int V; /* Number of nodes on the tree */
    int E; /* Number of arcs on the tree */
    // vector<vector>>
    vector<int> *adjNodesList; /* List of adjacent nodes to each one */
    int *visited;
    
    /* Constructor */
    Graph (int V, int E) {
        this->V = V;
        this->E = E;

        this->visited = new int[V];
        this->adjNodesList = new vector<int>[V];
        for (int i = 0; i < V; i++) {
            adjNodesList[i] = new vector<int>[4];

            adjNodesList[i][0] = -1;
            adjNodesList[i][1] = -1;
            adjNodesList[i][2] = 0;
            adjNodesList[i][3] = WHITE;

            visited[i] = 0;
        }
        
        
    }
};

    
void DFS_checkCycle(Graph &tree, int s, int v, bool *check) {
// Mark the current node as visited
    tree.visited[v] = true;
    *check = true;
    //cout << v << " ";

    // Recursive for all the vertices adjacent to this vertex
    //vector<int>::iterator i;
    int node;
    int size = tree.adjNodesList[v].size();
    for (int i = 0; i < size; i++) {
        node = tree.adjNodesList[v][i];
        if (node == s) {
            *check = false;
            break;
        }
        if (!tree.visited[node])
            DFS_checkCycle(tree, s, node, check);
    }
}

/* Adds the son's node to the father's list of adjacent vertices.
    -- verifies if, after adding the edge, the tree is still valid, no cycles --
*/
bool addEdge(Graph &tree, int v_father, int v_son) {
    int parent_number = tree.adjNodesList[v_son][2];

    if (parent_number == 2) {
        return false;
    }

    tree.adjNodesList[v_son][parent_number] = v_father;

    tree.adjNodesList[v_son][2]++;
    /*
    for (auto x : adjNodesList[v_father])
        cout << "-> " << x << endl;
    */
    return true;
}


void new_DFS(Graph graph, int node, color curr_color) {
    // visit node
    if (graph.adjNodesList[node][3] == WHITE) {
        graph.adjNodesList[node][3] = curr_color;
    }
    if (graph.adjNodesList[node][3] == GRAY && curr_color == BLACK) {
        graph.adjNodesList[node][3] = COMMON;
    }

    if (graph.adjNodesList[node][2] == 0) {
        return;
    }

    for (int i = 0; i < graph.adjNodesList[node][2]; i++) {
        new_DFS(graph, graph.adjNodesList[node][i], curr_color);
    }

}


void delete_memory(Graph &tree) {
    delete[] tree.adjNodesList;
    delete[] tree.visited;
}


void calculate_lca2(Graph graph) {
    vector<int> common_ancestors;

    for (int i = 0; i < graph.V; i++) {
        if (graph.adjNodesList[i][3] == COMMON) {
            common_ancestors.push_back(i);
        }
    }

    int common_size = common_ancestors.size();

    if (common_size == 0) {
        cout << "-" << endl;
        // TODO: hmmmmm?
        return;
    }

    sort(common_ancestors.begin(), common_ancestors.end());

    for (int node: common_ancestors) {
        for (int i = 0; i < graph.adjNodesList[node][2]; i++) {
            graph.adjNodesList[node][3] = GRAY;
        }
    }

    for (int node: common_ancestors) {
        if (graph.adjNodesList[node][3] == COMMON) {
            cout << node + 1 << " ";
        }
    }

    cout << endl;
}


bool DFS_checkCycle(Graph graph, int node) {
    graph.visited[node] = 1;

    for (int i = 0; i < graph.adjNodesList[node][2]; i++) {

        int father = graph.adjNodesList[node][i];

        if (graph.visited[father] == 0) {
            if (DFS_checkCycle(graph, father)) {
                return true;
            }
        }
        else if(graph.visited[father] == 1) {
            return true;
        }
    }

    graph.visited[node] = 2;
    return false;
}   


int main() {
    int v1, v2, n, m, vfather, vson;
    vector<int> *lca_v1v2 = new vector<int>;

    /** READ INPUT **/

    /* 1-   Identify nodes (v1, v2), number of nodes (n)
            and number of arcs (m) in the graph before creating it */
    cin >> v1 >> v2 >> n >> m;
    Graph familytree(n, m);
    /* 2-   Read m lines that correspond to the arcs */
    while (m > 0) {
        cin >> vfather >> vson;
        if (!addEdge(familytree, vfather - 1, vson - 1)) {
            cout << '0' << endl;
            return;
        }
        m--;
    }

    for (int i = 0; i < familytree.V; i++) {
        if (familytree.visited[i] == 0) {
            if(DFS_checkCycle(familytree, i)) {
                std::cout << 0 << std::endl;
                return;
            }
        }
    }

    /* PRINT OUTPUT */

    /* Valid family tree */

    new_DFS(familytree, v1, GRAY);
    new_DFS(familytree, v1, BLACK);

    calculate_lca2(familytree);

    delete_memory(familytree);
    delete lca_v1v2;

    return 0;
}

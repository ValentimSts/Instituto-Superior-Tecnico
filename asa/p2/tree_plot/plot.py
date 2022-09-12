import os
import sys
import graphviz

# *****************************************************************
# Vizualizer of DAGS
# ****************************************************************/

# You may need to download the package:
# - graphviz
# https://graphviz.readthedocs.io/en/stable/manual.html#installation

# To create the DAG, just run at the terminal:
#               python plot.py < file
# Where file is the input with the format used in the Project
# If you detect a bug let us know

dot = graphviz.Digraph(comment='Tree')

# Reads the vertices
a = (sys.stdin.readline().split())
vertice1 = int(a[0])
vertice2 = int(a[1])

# Reads the number of Vertices and Edges
a = (sys.stdin.readline().split())
num_vertice = int(a[0])
num_edges = int(a[1])

# Creates the vertices, the vertices used to calculate are highlighted
for i in range(1, num_vertice + 1):
    if (i == vertice1):
        with dot.subgraph(name="1") as c:
            c.node_attr['fillcolor'] = 'red'
            c.node_attr['style'] = 'filled'
            c.node(str("v" + str(i)))
    else:
        if(i == vertice2):
            with dot.subgraph(name="2") as c:
                c.node_attr['color'] = 'green'
                c.node_attr['style'] = 'filled'
                c.node(str("v" + str(i)))
        else:
            dot.node(str("v" + str(i)))

# Creates the edges
for i in range(0, num_edges):
    a = (sys.stdin.readline().split())
    dot.edge(str("v" + str(a[0])), str("v" + str(a[1])))

# Creates the PDF with the image
dot.render('Tree', view=True)

# Eliminates the diagram automatically created by the program
os.unlink("./Tree")
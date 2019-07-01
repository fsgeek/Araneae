'''using another graph visualization library 
input data from graphdata1, graphdata2 and graphdata3'''
from pyvis.network import Network
import pandas as pd

graph_net = Network(height="100%", width="6500px", bgcolor="#222222", font_color="white")


# set the physics layout of the network
graph_net.barnes_hut()
'''File1'''
graph_data = pd.read_csv("graphdata1")

sources = graph_data['Source']
targets = graph_data['Target']
weights = graph_data['Weight']

edge_data = zip(sources, targets, weights)

for e in edge_data:
    src = e[0]
    dst = e[1]
    w = e[2]

    graph_net.add_node(src, src, title=src, color='white', font='100px arial white')
    graph_net.add_node(dst, dst, title=dst, color='white', font='100px arial white')
    graph_net.add_edge(src, dst, value=w, physics=True, color='orange', title ='name')

'''File2'''
graph_data = pd.read_csv("graphdata2")

sources = graph_data['Source']
targets = graph_data['Target']
weights = graph_data['Weight']

edge_data = zip(sources, targets, weights)

for e in edge_data:
    src = e[0]
    dst = e[1]
    w = e[2]

    graph_net.add_node(src, src, title=src, color='white', font='100px arial white')
    graph_net.add_node(dst, dst, title=dst, color='white', font='100px arial white')
    graph_net.add_edge(src, dst, value=w, physics=True, color='green', title ='time')

'''File3'''
graph_data = pd.read_csv("graphdata3")

sources = graph_data['Source']
targets = graph_data['Target']
weights = graph_data['Weight']

edge_data = zip(sources, targets, weights)

for e in edge_data:
    src = e[0]
    dst = e[1]
    w = e[2]

    graph_net.add_node(src, src, title=src, color='white', font='100px arial white')
    graph_net.add_node(dst, dst, title=dst, color='white', font='100px arial white')
    graph_net.add_edge(src, dst, value=w, physics=True, color='blue', title = 'content')




neighbor_map = graph_net.get_adj_list()

# add neighbor data to node hover data
for node in graph_net.nodes:
    node["title"] += " Neighbors:<br>" + "<br>".join(neighbor_map[node["id"]])
    node["value"] = len(neighbor_map[node["id"]])


graph_net.show("graph.html")

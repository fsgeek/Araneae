'''Visualizing File Relationships using python librarires from data in 'graphdata5'
For more see 'Demo.mp4' video'''

import json
import time
import subprocess, os, platform
import dash
from dash.dependencies import Input, Output, State
import dash_core_components as dcc
import dash_html_components as html

import dash_cytoscape as cyto
from demos import dash_reusable_components as drc


# Load extra layouts
cyto.load_extra_layouts()

app = dash.Dash(__name__)
server = app.server


# ###################### DATA PREPROCESSING ######################
# Load data
with open('graphdata5', 'r') as f:
    network_data = f.read().split('\n')


edges = network_data

nodes = set()

following_node_di = {}  # user id -> list of users they are following
following_edges_di = {}  # user id -> list of cy edges starting from user id

followers_node_di = {}  # user id -> list of followers (cy_node format)
followers_edges_di = {}  # user id -> list of cy edges ending at user id

cy_edges = []
cy_nodes = []

for edge in edges:
   
    if len(edge.split("^"))==1:
        break
   
    linktype, source, sourcename, sourcemtime, target, targetname, targetmtime= edge.split("^")

    if linktype == "location":
        cy_edge = {'data': {'id': '@#$'+source+target, 'source': source, 'target': target}, 'style': {'line-color': 'red'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}
    elif linktype == "name":
        cy_edge = {'data': {'id': '@#$'+source+target, 'source': source, 'target': target}, 'style': {'line-color': 'blue'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}
    elif linktype == "time":
        cy_edge = {'data': {'id': '@#$'+source+target, 'source': source, 'target': target}, 'style': {'line-color': 'yellow'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}
    elif linktype == "content":
        cy_edge = {'data': {'id': '@#$'+source+target, 'source': source, 'target': target}, 'style': {'line-color': 'green'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}

    if linktype == "location":
        if target[0:3] == 'dir':
            cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'red_rect', 'mtime': targetmtime}
        else: 
            cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'red_circle', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'red_rect', 'mtime': sourcemtime}
    elif linktype == "name":
        cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'blue', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'blue', 'mtime': sourcemtime}
    elif linktype == "time":
        cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'yellow', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'yellow', 'mtime': sourcemtime}
    elif linktype == "content":
        cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'green', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'green', 'mtime': sourcemtime}

    if source not in nodes:
        nodes.add(source)
        cy_nodes.append(cy_source)
    if target not in nodes:
        nodes.add(target)
        cy_nodes.append(cy_target)

    # Process dictionary of following
    if not following_node_di.get(source):
        following_node_di[source] = []
    if not following_edges_di.get(source):
        following_edges_di[source] = []

    following_node_di[source].append(cy_target)
    following_edges_di[source].append(cy_edge)

    # Process dictionary of followers
    if not followers_node_di.get(target):
        followers_node_di[target] = []
    if not followers_edges_di.get(target):
        followers_edges_di[target] = []

    followers_node_di[target].append(cy_source)
    followers_edges_di[target].append(cy_edge)



default_elements = [cy_nodes[0]]

default_stylesheet = [
    {
        "selector": 'node',
        'style': {
            "label": "data(label)",
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": 'edge',
        'style': {
            "opacity": 0.45,
            'z-index': 5000
        }
    },
    {
        "selector": '.red_circle',
        'style': {
            "label": "data(label)",
            'background-color': 'red',
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.red_rect',
        'style': {
            "label": "data(label)",
            'background-color': 'red',
            "shape": 'rectangle',
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.green',
        'style': {
            "label": "data(label)",
            'background-color': 'green',
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.blue',
        'style': {
            "label": "data(label)",
            'background-color': 'blue',
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.yellow',
        'style': {
            "label": "data(label)",
            'background-color': 'yellow',
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.gray_rect',
        'style': {
            "label": "data(label)",
            'background-color': 'gray',
            "shape": 'rectangle',
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.gray_circle',
        'style': {
            "label": "data(label)",
            'background-color': 'gray',
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        'selector': ':selected',
        "style": {
            "border-width": 2,
            "border-color": "black",
            "border-opacity": 1,
            "opacity": 1,
            "label": "data(label)",
            "color": "black",
            "font-size": 12,
            'z-index': 9999
        }
    }
]

# ################################# APP LAYOUT ################################
tyles = {
    'json-output': {
        'overflow-y': 'scroll',
        'height': 'calc(50% - 25px)',
        'border': 'thin lightgrey solid'
    },
    'tab': {'height': 'calc(98vh - 80px)'}
}


app.layout = html.Div([

    html.Div(className='four columns', children=[

                dcc.Slider(
                    id='timeslider',
                    min=0,
                    max=8,
                    marks={1: 'Today', 2: 'One Week', 3: 'One Month', 4: 'One Quarter', 5: 'One Semester', 6: 'One Year', 7: 'All Time'},
                    step=1,
                    value=7,
                ),
                
            ]),

    html.Div(className='eight columns', children=[
        cyto.Cytoscape(
            id='cytoscape',
            elements=default_elements,
            stylesheet=default_stylesheet,
            style={
                'height': '85vh',
                'width': '100%'
            }
        )
    ]),

    html.Div(className='four columns', children=[
                dcc.Slider(
                    id='layoutslider',
                    min=0,
                    max=8,
                    marks={1: 'grid', 2: 'circle', 3: 'concentric', 4: 'breadthfirst', 5: 'dagre', 6: 'klay', 7: 'cola'},
                    step=1,
                    value=7,
                ),
                
            ]),

        
])


# ############################## CALLBACKS ####################################

@app.callback(Output('cytoscape', 'layout'),
              [Input('layoutslider', 'value')])
def update_cytoscape_layout(layoutnumber):
    layout = 'cola'
    if layoutnumber == 1:
        layout = 'grid'
    elif layoutnumber == 2:
        layout = 'circle'
    elif layoutnumber == 3:
        layout = 'concentric'
    elif layoutnumber == 4:
        layout = 'breadthfirst'
    elif layoutnumber == 5:
        layout = 'dagre'
    elif layoutnumber == 6:
        layout = 'klay'
    elif layoutnumber == 7:
        layout = 'cola'
    return {'name': layout}


@app.callback(Output('cytoscape', 'elements'),
              [Input('cytoscape', 'tapNodeData'),Input('timeslider', 'value')],
              [State('cytoscape', 'elements')])
def generate_elements(nodeData, timefilternumber, elements):
    if not nodeData:
        return default_elements
    

    last_timefilter = ''
    dummy_node = {}
    timefilter ='All Time'
    if timefilternumber == 1:
        timefilter = 'Today'
    elif timefilternumber == 2:
        timefilter = 'One Week'
    elif timefilternumber == 3:
        timefilter = 'One Month'
    elif timefilternumber == 4:
        timefilter = 'One Quarter'
    elif timefilternumber == 5:
        timefilter = 'One Semester'
    elif timefilternumber == 6:
        timefilter = 'One Year'
    elif timefilternumber == 7:
        timefilter = 'All Time'
    

    if nodeData['id'][0:3] == 'dir':
        nodeclass = 'gray_rect'
    else:
        nodeclass = 'gray_circle'

    new_elements = []

    if len(elements)>1:
        
        i = 0

        for element in elements: #adding all path nodes
            if element.get('data').get('id')[0:8] == '00000000': #detecting dummy marker
                last_timefilter = element.get('data').get('id')[8:]
                break
            elif element.get('data').get('id') == nodeData['id']:
                if elements[i+1].get('data').get('id')[0:8] == '00000000' and element.get('data').get('id')[0:3] != 'dir' and elements[i+1].get('data').get('id')[8:]==timefilter: #detect intent to open file
                    subprocess.call(('xdg-open', nodeData['id']))
                for element in elements: #adding all path nodes
                    if element.get('data').get('id')[0:8] == '00000000': #detecting dummy marker
                        last_timefilter = element.get('data').get('id')[8:]
                        break      
                break
            else:
                new_elements.append(element)  
            i = i+1

        new_click = {'data': nodeData, 'classes': nodeclass}
        dummy_node = {'data': {'id': '00000000' + timefilter, 'label': '...'}} #dummy node to mark path and to keep track of previous filter
        if new_click['data']['id'][0:8]!= '00000000':
            new_elements.extend([new_click,dummy_node])
        else: 
            new_elements.extend([dummy_node])
        elements = new_elements
        
    else:
        new_click = {'data': nodeData, 'classes': nodeclass}
        dummy_node = {'data': {'id': '00000000' + timefilter, 'label': '...'}} #dummy node to mark path and to keep track of previous filter 
        new_elements = [new_click,dummy_node]

    for element in elements:
        if element['data']['id'][0:3]=='@#$':  #detecting edges
            new_elements.append(element)   

    elements = new_elements

    followers_nodes = followers_node_di.get(nodeData['id'])
    followers_edges = followers_edges_di.get(nodeData['id'])

    if followers_nodes:
        elements.extend(followers_nodes)

    if followers_edges:
        elements.extend(followers_edges)

    

    following_nodes = following_node_di.get(nodeData['id'])
    following_edges = following_edges_di.get(nodeData['id'])

    if following_nodes:
        elements.extend(following_nodes)

    if following_edges:
        elements.extend(following_edges)

    

    return_elements = []

    # Time Filter

    currentTimesinceEpoc = time.time()
    modificationTimesinceEpoc = 0
    timeSinceModification = 0
     
    flag = 0 # elements before the marker '00000000' are tag elements and are not filtered                   
    for element in elements:
        if flag == 0:
            if element.get('data').get('id')[0:8] == '00000000':#detecting dummy node
                dummy_node = {'data': {'id': '00000000' + timefilter, 'label': '...'}}
                return_elements.append(dummy_node)
                flag = 1
            else:
                return_elements.append(element)
        else:
            path = element.get('data').get('id')
            if timefilter == 'All Time':  
                return_elements.append(element)
            else:
                if path[0:3]=='dir':
                    path = path[3:]
                if path[0:3]=='@#$':  #detecting edges
                    modificationTimesinceEpoc = float(element['targetmtime'])
                else:
                    modificationTimesinceEpoc = float(element['mtime'])

                timeSinceModification = currentTimesinceEpoc -modificationTimesinceEpoc

                if timefilter == 'One Year':
                    if timeSinceModification<=366*24*60*60:
                        return_elements.append(element)
                if timefilter == 'One Semester':
                    if timeSinceModification<=183*24*60*60:
                        return_elements.append(element)
                if timefilter == 'One Quarter':
                    if timeSinceModification<=92*24*60*60:
                        return_elements.append(element)
                elif timefilter == 'One Month':
                    if timeSinceModification<=31*24*60*60:
                        return_elements.append(element)
                elif timefilter == 'One Week':
                    if timeSinceModification<=7*24*60*60:
                        return_elements.append(element)
                elif timefilter == 'Today':
                    if timeSinceModification<=24*60*60:
                        return_elements.append(element) 

     
    # Adding edges between path nodes 
    source_id = '000000000'
    target_id = '000000000'
    for element in return_elements:
        if element.get('data').get('id')[0:8] == '00000000': #detecting dummy node
            break
        else:
            target_id = source_id
            source_id = element.get('data').get('id')
            if source_id!= '000000000' and target_id!= '000000000':
                link = {'data': {'id': '@#$'+source_id+target_id, 'source': source_id, 'target': target_id}, 'style': {'line-color': 'gray', 'mid-source-arrow-color': 'gray', 'mid-source-arrow-shape': 'triangle'}}
                return_elements.append(link)

    return return_elements


if __name__ == '__main__':
    app.run_server(debug=True)

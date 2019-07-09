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

directory_identifier = 'dir' #marks directory node ids
edge_identifier = '@#$' #marks edge ids
special_identifier = '!@#' #marks time/content/name nodes and edges

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
   
    linktype, target, targetname, targetmtime, source, sourcename, sourcemtime= edge.split("^")

    if linktype == "location":
        cy_edge = {'data': {'id': edge_identifier+source+target, 'source': source, 'target': target}, 'style': {'line-color': 'red', 'mid-source-arrow-color': 'red', 'mid-source-arrow-shape': 'triangle'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}
    elif linktype == "name":
        cy_edge_11 = {'data': {'id': edge_identifier+special_identifier+source+'name'+'1', 'source': source, 'target': special_identifier+source+'name'}, 'style': {'line-color': 'blue'}, 'sourcemtime': sourcemtime, 'targetmtime': sourcemtime}
        cy_edge_12 = {'data': {'id': edge_identifier+source+target+'name'+'12', 'source': special_identifier+source+'name', 'target': target}, 'style': {'line-color': 'blue'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}
        cy_edge_21 = {'data': {'id': edge_identifier+special_identifier+target+'name'+'1', 'source': target, 'target': special_identifier+target+'name'}, 'style': {'line-color': 'blue'}, 'sourcemtime': targetmtime, 'targetmtime': targetmtime}
        cy_edge_22 = {'data': {'id': edge_identifier+source+target+'name'+'22', 'source': special_identifier+target+'name', 'target': source}, 'style': {'line-color': 'blue'}, 'sourcemtime': targetmtime, 'targetmtime': sourcemtime}
    elif linktype == "time":
        cy_edge_11 = {'data': {'id': edge_identifier+special_identifier+source+'time'+'1', 'source': source, 'target': special_identifier+source+'time'}, 'style': {'line-color': 'yellow'}, 'sourcemtime': sourcemtime, 'targetmtime': sourcemtime}
        cy_edge_12 = {'data': {'id': edge_identifier+source+target+'time'+'12', 'source': special_identifier+source+'time', 'target': target}, 'style': {'line-color': 'yellow'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}
        cy_edge_21 = {'data': {'id': edge_identifier+special_identifier+target+'time'+'1', 'source': target, 'target': special_identifier+target+'time'}, 'style': {'line-color': 'yellow'}, 'sourcemtime': targetmtime, 'targetmtime': targetmtime}
        cy_edge_22 = {'data': {'id': edge_identifier+source+target+'time'+'22', 'source': special_identifier+target+'time', 'target': source}, 'style': {'line-color': 'yellow'}, 'sourcemtime': targetmtime, 'targetmtime': sourcemtime}
    elif linktype == "content":
        cy_edge_11 = {'data': {'id': edge_identifier+special_identifier+source+'content'+'1', 'source': source, 'target': special_identifier+source+'content'}, 'style': {'line-color': 'green'}, 'sourcemtime': sourcemtime, 'targetmtime': sourcemtime}
        cy_edge_12 = {'data': {'id': edge_identifier+source+target+'content'+'12', 'source': special_identifier+source+'content', 'target': target}, 'style': {'line-color': 'green'}, 'sourcemtime': sourcemtime, 'targetmtime': targetmtime}
        cy_edge_21 = {'data': {'id': edge_identifier+special_identifier+target+'content'+'1', 'source': target, 'target': special_identifier+target+'content'}, 'style': {'line-color': 'green'}, 'sourcemtime': targetmtime, 'targetmtime': targetmtime}
        cy_edge_22 = {'data': {'id': edge_identifier+source+target+'content'+'22', 'source': special_identifier+target+'content', 'target': source}, 'style': {'line-color': 'green'}, 'sourcemtime': targetmtime, 'targetmtime': sourcemtime}

    if linktype == "location":
        if target[0:3] == directory_identifier:
            cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'red_rect', 'mtime': targetmtime}
        else: 
            cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'red_circle', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'red_rect', 'mtime': sourcemtime}
    elif linktype == "name":
        cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'blue', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'blue', 'mtime': sourcemtime}
        cy_bubble_1 = {"data": {"id": special_identifier+source + 'name', "label": 'name'}, 'classes': 'blue', 'mtime': sourcemtime}
        cy_bubble_2 = {"data": {"id": special_identifier+target + 'name', "label": 'name'}, 'classes': 'blue', 'mtime': targetmtime}
    elif linktype == "time":
        cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'yellow', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'yellow', 'mtime': sourcemtime}
        cy_bubble_1 = {"data": {"id": special_identifier+source + 'time', "label": 'time'}, 'classes': 'yellow', 'mtime': sourcemtime}
        cy_bubble_2 = {"data": {"id": special_identifier+target + 'time', "label": 'time'}, 'classes': 'yellow', 'mtime': targetmtime}
    elif linktype == "content":
        cy_target = {"data": {"id": target, "label": targetname}, 'classes': 'green', 'mtime': targetmtime}
        cy_source = {"data": {"id": source, "label": sourcename}, 'classes': 'green', 'mtime': sourcemtime}
        cy_bubble_1 = {"data": {"id": special_identifier+source + 'content', "label": 'content'}, 'classes': 'green', 'mtime': sourcemtime}
        cy_bubble_2 = {"data": {"id": special_identifier+target + 'content', "label": 'content'}, 'classes': 'green', 'mtime': targetmtime}

    if linktype != "location":

        if target not in nodes:
            nodes.add(target)
            cy_nodes.append(cy_target)
        if source not in nodes:
            nodes.add(source)
            cy_nodes.append(cy_source)
        
        if cy_bubble_1['data']['id'] not in nodes:
            nodes.add(cy_bubble_1['data']['id'])
            cy_nodes.append(cy_bubble_1)
        if cy_bubble_2['data']['id'] not in nodes:
            nodes.add(cy_bubble_2['data']['id'])
            cy_nodes.append(cy_bubble_2)

        # Process dictionary of following
        if not following_node_di.get(source):
            following_node_di[source] = []
        if not following_edges_di.get(source):
            following_edges_di[source] = []

        if not following_node_di.get(target):
            following_node_di[target] = []
        if not following_edges_di.get(target):
            following_edges_di[target] = []

        if not following_node_di.get(cy_bubble_1['data']['id']):
            following_node_di[cy_bubble_1['data']['id']] = []
        if not following_edges_di.get(cy_bubble_1['data']['id']):
            following_edges_di[cy_bubble_1['data']['id']] = []

        if not following_node_di.get(cy_bubble_2['data']['id']):
            following_node_di[cy_bubble_2['data']['id']] = []
        if not following_edges_di.get(cy_bubble_2['data']['id']):
            following_edges_di[cy_bubble_2['data']['id']] = []


        if cy_bubble_1 not in following_node_di[source]:
            following_node_di[source].append(cy_bubble_1)
        if cy_bubble_2 not in following_node_di[target]:
            following_node_di[target].append(cy_bubble_2)

        if cy_target not in following_node_di[cy_bubble_1['data']['id']]:
            following_node_di[cy_bubble_1['data']['id']].append(cy_target)
        if cy_target not in following_node_di[cy_bubble_2['data']['id']]:
            following_node_di[cy_bubble_2['data']['id']].append(cy_target)
        if cy_source not in following_node_di[cy_bubble_1['data']['id']]:
            following_node_di[cy_bubble_1['data']['id']].append(cy_source)
        if cy_source not in following_node_di[cy_bubble_2['data']['id']]:
            following_node_di[cy_bubble_2['data']['id']].append(cy_source)


        if cy_edge_11 not in following_edges_di[source]:
            following_edges_di[source].append(cy_edge_11)
        if cy_edge_21 not in following_edges_di[target]:
            following_edges_di[target].append(cy_edge_21)

        if cy_edge_11 not in following_edges_di[cy_bubble_1['data']['id']]:
            following_edges_di[cy_bubble_1['data']['id']].append(cy_edge_11)
        if cy_edge_12 not in following_edges_di[cy_bubble_1['data']['id']]:
            following_edges_di[cy_bubble_1['data']['id']].append(cy_edge_12)
        if cy_edge_21 not in following_edges_di[cy_bubble_2['data']['id']]:
            following_edges_di[cy_bubble_2['data']['id']].append(cy_edge_21)
        if cy_edge_22 not in following_edges_di[cy_bubble_2['data']['id']]:
            following_edges_di[cy_bubble_2['data']['id']].append(cy_edge_22)




        # Process dictionary of followers   
        ######## followers no in use anymore - all nodes and edges added to following
        if not followers_node_di.get(target):
            followers_node_di[target] = []
        if not followers_edges_di.get(target):
            followers_edges_di[target] = []

        

    else:
        
        if target not in nodes:
            nodes.add(target)
            cy_nodes.append(cy_target)
        if source not in nodes:
            nodes.add(source)
            cy_nodes.append(cy_source)

        # Process dictionary of following
        if not following_node_di.get(source):
            following_node_di[source] = []
        if not following_edges_di.get(source):
            following_edges_di[source] = []
        if not following_node_di.get(target):
            following_node_di[target] = []
        if not following_edges_di.get(target):
            following_edges_di[target] = []

       
        following_node_di[source].append(cy_target)
        following_edges_di[source].append(cy_edge)
        following_node_di[target].append(cy_source)
        following_edges_di[target].append(cy_edge)

        # Process dictionary of followers
        ######## followers no in use anymore - all nodes and edges added to following
        if not followers_node_di.get(target):
            followers_node_di[target] = []
        if not followers_edges_di.get(target):
            followers_edges_di[target] = []



default_elements = [cy_nodes[0]]

default_stylesheet = [
    {
        "selector": 'node',
        'style': {
            "label": "data(label)",
           "font-size": 10,
            "opacity": 0.65,
            "text-wrap": "wrap",
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
           "font-size": 10,
            "text-wrap": "wrap",
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.red_rect',
        'style': {
            "label": "data(label)",
            'background-color': 'red',
           "font-size": 10,
            "text-wrap": "wrap",
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
           "font-size": 10,
            "text-wrap": "wrap",
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.blue',
        'style': {
            "label": "data(label)",
            'background-color': 'blue',
           "font-size": 10,
            "text-wrap": "wrap",
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.yellow',
        'style': {
            "label": "data(label)",
            'background-color': 'yellow',
           "font-size": 10,
            "text-wrap": "wrap",
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        "selector": '.gray_rect',
        'style': {
            "label": "data(label)",
            'background-color': 'gray',
           "font-size": 10,
            "text-wrap": "wrap",
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
            "text-wrap": "wrap",
           "font-size": 10,
            "opacity": 0.65,
            'z-index': 9999
        }
    },
    {
        'selector': ':selected',
        "style": {
            "border-width": 2,
            "border-color": "black",
            "text-wrap": "wrap",
            "border-opacity": 1,
            "opacity": 1,
            "label": "data(label)",
            "color": "black",
           "font-size": 10,
            'z-index': 9999
        }
    }
]

# ################################# APP LAYOUT ################################

styles = {
    'json-output': {
        'overflow-y': 'scroll',
        'height': 'calc(50% - 25px)',
        'border': 'thin lightgrey solid'
    },
    'tab': {'height': 'calc(98vh - 80px)'}
}


app.layout = html.Div([

    html.Div(className='four columns', children=[

                dcc.RangeSlider(
                    id='timeslider',
                    min=0,
                    max=10,
                    marks={9: 'Present', 8: 'One day', 7: 'One Week', 6: 'One Month', 5: 'One Quarter', 4: 'One Semester', 3: 'One Year', 2: 'Two Years', 1: 'Long Time'},
                    step=1,
                    value=[1,9],
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
            },
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
def generate_elements(nodeData, timefilterpair, elements):
    if not nodeData:
        return default_elements
    
    timefilter = str(timefilterpair[0]) + str(timefilterpair[1]) #To keep track of the kind of folter
    last_timefilter = ''
    dummy_node = {}
   
    if nodeData['id'][0:3] == directory_identifier:
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
                if elements[i+1].get('data').get('id')[0:8] == '00000000' and element.get('data').get('id')[0:3] != directory_identifier and elements[i+1].get('data').get('id')[8:]==timefilter: #detect intent to open file
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
        if element['data']['id'][0:3]==edge_identifier:  #detecting edges
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
    '''{9: 'Present', 8: 'One day', 7: 'One Week', 6: 'One Month', 5: 'One Quarter', 4: 'One Semester', 3: 'One Year', 2: 'Two Years', 1: 'Long Time'}'''

    timefilterrange = [currentTimesinceEpoc,0]

    if timefilterpair[0] == 9: #time range in seconds
        timefilterrange[0] = 0*24*60*60
    elif timefilterpair[0] == 8:
        timefilterrange[0] = 1*24*60*60
    elif timefilterpair[0] == 7:
        timefilterrange[0] = 7*24*60*60
    elif timefilterpair[0] == 6:
        timefilterrange[0] = 31*24*60*60
    elif timefilterpair[0] == 5:
        timefilterrange[0] = 183*24*60*60
    elif timefilterpair[0] == 4:
        timefilterrange[0] = 92*24*60*60
    elif timefilterpair[0] == 3:
        timefilterrange[0] = 366*24*60*60
    elif timefilterpair[0] == 8:
        timefilterrange[0] = (366+365)*24*60*60
    elif timefilterpair[0] == 1:
        timefilterrange[0] = currentTimesinceEpoc

    if timefilterpair[1] == 9:
        timefilterrange[1] = 0*24*60*60
    elif timefilterpair[1] == 8:
        timefilterrange[1] = 1*24*60*60
    elif timefilterpair[1] == 7:
        timefilterrange[1] = 7*24*60*60
    elif timefilterpair[1] == 6:
        timefilterrange[1] = 28*24*60*60
    elif timefilterpair[1] == 5:
        timefilterrange[1] = 180*24*60*60
    elif timefilterpair[1] == 4:
        timefilterrange[1] = 90*24*60*60
    elif timefilterpair[1] == 3:
        timefilterrange[1] = 365*24*60*60
    elif timefilterpair[1] == 8:
        timefilterrange[1] = (365+365)*24*60*60
    elif timefilterpair[1] == 1:
        timefilterrange[1] = currentTimesinceEpoc
     
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
           
            if path[0:3]==directory_identifier: #detecting directories
                path = path[3:]
            if path[0:3]==edge_identifier:  #detecting edges
                modificationTimesinceEpoc = float(element['targetmtime'])
            else:
                print(element['mtime'])
                modificationTimesinceEpoc = float(element['mtime'])

            timeSinceModification = currentTimesinceEpoc -modificationTimesinceEpoc

                
            if (timeSinceModification<=timefilterrange[0] and timeSinceModification>=timefilterrange[1]) or (timeSinceModification>=currentTimesinceEpoc): # the second condition detects special nodes
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
                link = {'data': {'id': edge_identifier+source_id+target_id, 'source': source_id, 'target': target_id}, 'style': {'line-color': 'gray', 'mid-source-arrow-color': 'gray', 'mid-source-arrow-shape': 'triangle'}}
                return_elements.append(link)

    copy_return_elements = []
    copy_return_elements.extend(return_elements)

    # To show directory to which a file belongs
    for element in copy_return_elements:
        if element['data']['id'][0:3]!=edge_identifier and element['data']['id'][0:3]!=directory_identifier and element.get('data').get('id')[0:8] != '00000000' and element['data']['id'][0:3]!=special_identifier:  #detecting files
            following_nodes = following_node_di.get(element['data']['id'])
            following_edges = following_edges_di.get(element['data']['id'])
            print(element['data']['id'])
    
            if following_nodes:
                for following_node in following_nodes :
                    if (following_node not in return_elements) and (following_node['data']['id'][0:3]!=special_identifier):
                        return_elements.append(following_node)

            if following_edges:
                for following_edge in following_edges:
                    if (following_edge not in return_elements) and (following_edge['data']['id'][3:6]!=special_identifier):
                        return_elements.append(following_edge)


    return return_elements



if __name__ == '__main__':
    app.run_server(debug=True)
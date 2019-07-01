'''Finds files with similar modification time, names, and content.
Scans local system directory specified by the variable 'path' and Google Drive files.
Output needs to be printed to 'graphdata5', so that 'GraphVisualization2.py' can use it.
'''
from __future__ import print_function
import pickle
import time
import textdistance
import os
import io
import operator
from py_essentials import hashing as hs
import numpy as np
import magic
from sklearn.cluster import KMeans
import datetime as dt
import googleapiclient
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from datasketch import MinHash, MinHashLSH
import shutil

# If modifying these scopes, delete the file token.pickle.
SCOPES = ['https://www.googleapis.com/auth/drive']

def main():

    
    path = '../../Desktop' # Directory in local computer
    """Shows basic usage of the Drive v3 API.
    Prints the names and ids of the first 10 files the user has access to.
    """
    creds = None
    # The file token.pickle stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists('token.pickle'):
        with open('token.pickle', 'rb') as token:
            creds = pickle.load(token)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'credentials.json', SCOPES)
            creds = flow.run_local_server()
        # Save the credentials for the next run
        with open('token.pickle', 'wb') as token:
            pickle.dump(creds, token)


    service = build('drive', 'v3', credentials=creds)

    

    # Call the Drive v3 API
    results = service.files().list(
        pageSize=1000, fields="nextPageToken, files(parents, name, id, modifiedTime, mimeType, md5Checksum)").execute()
    items = results.get('files', [])

    

    currentTimesinceEpoc = time.time()
    currentday = time.strftime('%d', time.localtime(currentTimesinceEpoc))
    currentmonth = time.strftime('%m', time.localtime(currentTimesinceEpoc))
    currentyear = time.strftime('%Y', time.localtime(currentTimesinceEpoc))

    folderModifTimes = {}

    if not items:
        print('No files found.')
    else:
        '''location heirarchy for Drive files'''
        
        for item in items:

            
            modifyear, modifmonth, modifday = item['modifiedTime'].split('-')
            modifday = modifday.split('T')[0]
                
            modifdate = dt.datetime(int(modifyear),int(modifmonth), int(modifday),0,0,0)
            currentdate = dt.datetime(int(currentyear),int(currentmonth), int(currentday),0,0,0)
            modificationTimesinceEpoc = str(currentTimesinceEpoc - (currentdate-modifdate).total_seconds())

            item ['modificationTimesinceEpoc'] = modificationTimesinceEpoc

            if item.get('parents') != None and len(item.get('parents')) >1: #more tham 1 parent not handled
                print("many parents")
            if item.get('parents') == None:
                if item['mimeType'] == 'application/vnd.google-apps.folder':
                    parentname = 'Drive'
                    parentid = 'drive'
                else:
                    parentname = 'Parent-less Files'
                    parentid = 'parentlessfiles'
            else:
                parentname = (service.files().get(fileId=item.get('parents')[0]).execute())['name']
                parentid = item.get('parents')[0]
                
            if folderModifTimes.get(parentid) == None or float(folderModifTimes[parentid]) < float(modificationTimesinceEpoc):
                folderModifTimes[parentid] = modificationTimesinceEpoc
 
            item['parentid'] = parentid
            item['parentname'] = parentname

        new_items = []

        print('location' + '^' + 'dir' + 'root' + '^' + 'Root'  + '^' + str(currentTimesinceEpoc) + '^' + 'dir'+os.path.abspath(path)+ '^' + 'Desktop'  + '^' + str(currentTimesinceEpoc))
        print('location' + '^' + 'dir' + 'root' + '^' + 'Root'  + '^' + str(currentTimesinceEpoc) + '^' + 'dir'+'drive'+ '^' + 'Drive'  + '^' + str(currentTimesinceEpoc))
        print('location' + '^' + 'dir' + 'drive' + '^' + 'Drive'  + '^' + str(currentTimesinceEpoc) + '^' + 'dir'+'parentlessfiles'+ '^' + 'Parent-less Files'  + '^' + folderModifTimes['parentlessfiles'])

        for item in items:
            
            if item['mimeType'] == 'application/vnd.google-apps.folder': #if a folder
                if folderModifTimes.get(item['id']) == None:
                    print('location' + '^' + 'dir' + item['parentid'] + '^' + item['parentname']  + '^' + folderModifTimes[item['parentid']] + '^' + 'dir'+item['id']+ '^' + item['name']  + '^' + item ['modificationTimesinceEpoc'])
                else:
                    print('location' + '^' + 'dir' + item['parentid'] + '^' + item['parentname']  + '^' + folderModifTimes[item['parentid']] + '^' + 'dir'+item['id']+ '^' + item['name']  + '^' + folderModifTimes[item['id']])
            else: #if not a folder
                new_items.append(item) #further links would only be between files
                print('location' + '^' + 'dir' + item['parentid'] + '^' + item['parentname']  + '^' + folderModifTimes[item['parentid']] + '^' +item['id']+ '^' + item['name']  + '^' + item ['modificationTimesinceEpoc'])
        
        items = new_items

        for r, d, f in os.walk(path):
            f = [file for file in f if not (file[0] == '.' or file[0] == '_')]
            d[:] = [dr for dr in d if not (dr[0] == '.' or dr[0] == '_')]
            for file in f:
                    filepath = os.path.join(r, file)
                    filepath2 = filepath.split('/')
                    filename = filepath2[-1]
                    filemtime = str(os.path.getmtime(os.path.abspath(filepath)))
                    mime = magic.Magic(mime=True)
                    mimeType = mime.from_file(os.path.abspath(filepath))
                    md5Checksum = hs.fileChecksum(os.path.abspath(filepath), "md5")
                    item = {'id': os.path.abspath(filepath),'name' :filename, 'modificationTimesinceEpoc': filemtime, 'mimeType': mimeType, 'md5Checksum': md5Checksum }
                    items.append(item)

        ''' same hash'''

        for f1 in items:
            for f2 in items:
                if  f1['id']!=f2['id'] and f1.get('md5Checksum') != None and f2.get('md5Checksum') != None and f1.get('md5Checksum') == f2.get('md5Checksum'):
                    f1mtime = f1['modificationTimesinceEpoc']
                    f2mtime = f2['modificationTimesinceEpoc']
                    print('content' + '^'+ f1['id'] + '^' + f1['name'] + '^' + f1mtime + '^'+ f2['id'] + '^' + f2['name'] + '^' + f2mtime)

        '''content similarity'''


        text_files = []

        '''scanning the directory'''
        for f in items:
            filepath = f['id'] 
            file_type = f['mimeType']
            major_minor = file_type.split('/')
                    
            if major_minor[0] == 'text' or file_type == 'application/vnd.google-apps.document':
                text_files.append(f)

        documents =[]

        os.mkdir('DriveTextFiles') #To temporarily store drive text files

        for f in text_files:
            try:
                documents.append((open(f['id']).read(),f))
            except FileNotFoundError: # downloading drive text files
                file_id = f['id']
                if f['mimeType'] == 'application/vnd.google-apps.document':
                    request = service.files().export_media(fileId=file_id, mimeType='text/plain')
                else:
                    request = service.files().get_media(fileId=file_id)
                fh = io.FileIO(os.path.join('DriveTextFiles',f['id']), 'wb')
                downloader = googleapiclient.http.MediaIoBaseDownload(fh, request)
                done = False
                while done is False:
                    status, done = downloader.next_chunk()
                documents.append((open(os.path.join('DriveTextFiles',f['id'])).read(),f))

        shutil.rmtree('DriveTextFiles')

        lsh = MinHashLSH(threshold=0.3, num_perm=128)

        for f in documents:
            setdoc = set(f[0].split())  
            m = MinHash(num_perm=128)
            for d in setdoc:
                m.update(d.encode('utf8'))
            lsh.insert(f[1]['id'] + '^' +f[1]['name']+ '^' +f[1]['modificationTimesinceEpoc'], m)


        results = []
        for doc in documents:
            setdoc = set(doc[0].split())    
            m = MinHash(num_perm=128)
            for d in setdoc:
                m.update(d.encode('utf8'))
            result = lsh.query(m)
            results.append((doc[1]['id'] + '^' +doc[1]['name']+ '^' +doc[1]['modificationTimesinceEpoc'],result)) 


        '''forming links between files with similar content'''

        for result in results:
            f2mtime = result[0].split('^')[2]
            for r in result[1]:
                if r!=result[0]: 
                    f1mtime = r.split('^')[2]
                    print('content' + '^'+ r.split('^')[0] + '^' + r.split('^')[1] + '^' + f1mtime +'^'+ result[0].split('^')[0] + '^' + result[0].split('^')[1] + '^' + f2mtime)

        '''name similarity'''

        for f1 in items:
            for f2 in items:
                distance1 = textdistance.jaro.distance(f1['name'],f2['name'])
                distance2 = textdistance.levenshtein.distance(f1['name'],f2['name']) 
                if ((distance1<=0.30 and f1['id']!=f2['id'] and distance2<0.75*min(len(f1['name']),len(f2['name']),8)) or distance1<=0.15 or distance2<=0.25*min(len(f1['name']),len(f2['name']),8)) and f1['id']!=f2['id']:
                    f1mtime = f1['modificationTimesinceEpoc']
                    f2mtime = f2['modificationTimesinceEpoc']
                    print('name' + '^'+ f1['id'] + '^' + f1['name'] + '^' + f1mtime +'^' + f2['id'] + '^' + f2['name'] + '^' + f2mtime)


        '''time similarity'''

        file_threshhold = 10
        filetimes = []
        i = 0

        for item in items:
            filetimes.append([float(item['modificationTimesinceEpoc'])])
            i = i+1

        kmeans = KMeans(n_clusters=int(i/file_threshhold) +1, random_state=0).fit(filetimes)
        labels = kmeans.labels_

        for j in range(int(i/file_threshhold)+1) : #iterating through all clusters
            idx = [] 
            for i in range(0, len(labels)) : 
                if labels[i] == j : 
                    idx.append(i) 


            filesj = [items[i] for i in idx] #all the files in a cluster
    
            #forming similar time links
            for f1 in filesj:
                for f2 in filesj:
                    if  f1['id']!=f2['id'] :
                        f1mtime = f1['modificationTimesinceEpoc']
                        f2mtime = f2['modificationTimesinceEpoc']
                        print('time' + '^'+ f1['id'] + '^' + f1['name'] + '^' + f1mtime + '^'+ f2['id'] + '^' + f2['name'] + '^' + f2mtime)

        

    

if __name__ == '__main__':
    main()




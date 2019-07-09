import email, getpass, imaplib, os
from py_essentials import hashing as hs
import magic
import datetime as dt
import time

detach_dir = '.' # directory where to save attachments (default: current)
user = getpass.getpass("Enter your GMail username:")
pwd = getpass.getpass("Enter your password: ")

# connecting to the gmail imap server
m = imaplib.IMAP4_SSL("imap.gmail.com")
m.login(user,pwd)
m.select('"[Gmail]/All Mail"')

# use m.list() to get all the mailboxes

resp, items = m.search(None, "ALL") # you could filter using the IMAP rules here (check http://www.example-code.com/csharp/imap-search-critera.asp)
items = items[0].split() # getting the mails id

currentTimesinceEpoc = time.time()
currentday = time.strftime('%d', time.localtime(currentTimesinceEpoc))
currentmonth = time.strftime('%m', time.localtime(currentTimesinceEpoc))
currentyear = time.strftime('%Y', time.localtime(currentTimesinceEpoc))
currentdate = dt.datetime(int(currentyear),int(currentmonth), int(currentday),0,0,0)

for emailid in items:

    resp, data = m.fetch(emailid, '(BODY[HEADER.FIELDS (MESSAGE-ID)])')
    msg_str = email.message_from_string(data[0][1].decode('utf-8'))
    message_id = msg_str.get('Message-ID') #unique message id of an email

    resp, data = m.fetch(emailid, "(RFC822)") # fetching the mail, "`(RFC822)`" means "get the whole stuff", but you can ask for headers only, etc
    email_body = data[0][1] # getting the mail content
    mail = email.message_from_string(email_body.decode('utf-8')) # parsing the mail content to get a mail object

    #Check if any attachments at all
    if mail.get_content_maintype() != 'multipart':
        continue
    
    # we use walk to create a generator so we can iterate on the parts and forget about the recursive headach
    for part in mail.walk():
        # multipart are just containers, so we skip them
        if part.get_content_maintype() == 'multipart':
            continue

        # is this part an attachment ?
        if part.get('Content-Disposition') is None:
            continue

        att_name = part.get_filename()
        counter = 1

        # if there is no att_name, we create one with a counter to avoid duplicates
        if not att_name:
            att_name = 'part-%03d%s' % (counter, 'bin')
            counter += 1

        att_path = os.path.join(detach_dir, att_name)

        #Check if its already there
        if not os.path.isfile(att_path) :
            # finally write the stuff
            fp = open(att_path, 'wb')
            fp.write(part.get_payload(decode=True))
            fp.close()
            mime = magic.Magic(mime=True)
            att_mimeType = mime.from_file(os.path.abspath(att_path))
            att_hash = hs.fileChecksum(att_path, "md5")
            att_id = message_id
            os.remove(att_name) 
            att_name = att_name.replace('\n', ' ').replace('\r', ' ')
            att_id = att_id.replace('>', '').replace('<', '')


            att_mtime = email.utils.parsedate(mail['date'])
            att_modifdate = dt.datetime(int(att_mtime[0]),int(att_mtime[1]), int(att_mtime[2]),0,0,0)
            att_modificationTimesinceEpoc = str(currentTimesinceEpoc - (currentdate-att_modifdate).total_seconds())
            

            print('https://mail.google.com/mail/u/1/#search/rfc822msgid%3A' + att_id + '^' + 'email' + '-' + att_name + '^' +att_mimeType + '^' +att_hash + '^' + att_modificationTimesinceEpoc)
            


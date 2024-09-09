#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

fileitem = form['filename']

message = 'Uploading Failed'

upload_dir = os.path.join(os.getcwd(), 'cgi-bin', 'tmp')
os.makedirs(upload_dir, exist_ok=True)

if fileitem.filename:
    filename = os.path.basename(fileitem.filename)
    filepath = os.path.join(upload_dir, filename)
    
    try:
        with open(filepath, 'wb') as file:
            file.write(fileitem.file.read())
        message = f'The file "{filename}" was uploaded to {upload_dir}'
    except Exception as e:
        message = f'An error occurred: {str(e)}'

print("Content-Type: text/html;charset=utf-8\r\n")
print(f"<H1>{message}</H1>")

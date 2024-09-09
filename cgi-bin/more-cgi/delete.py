#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

fileitem = form.getvalue('filename')

message = 'Deletion Failed'

upload_dir = os.path.join(os.getcwd(), 'cgi-bin', 'tmp')

if fileitem:
    filename = os.path.basename(fileitem)
    filepath = os.path.join(upload_dir, filename)
    
    if os.path.exists(filepath):
        try:
            os.remove(filepath)
            message = f'The file "{filename}" was successfully deleted from {upload_dir}'
        except Exception as e:
            message = f'An error occurred while deleting the file: {str(e)}'
    else:
        message = 'File does not exist'

print("Content-Type: text/html;charset=utf-8\r\n")
print(f"<H1>{message}</H1>")

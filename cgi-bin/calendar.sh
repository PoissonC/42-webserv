#!/bin/bash

echo "Status: 200 OK\r\nContent-Type: text/html\r\n\r\n"

cat <<EOF

<h3>Date: </h3>
<pre>$(date)
$(date -u)</pre>

<h4>Days have passed since the beginning of the year: </h4>
<pre>$(date +%j)</pre>
EOF

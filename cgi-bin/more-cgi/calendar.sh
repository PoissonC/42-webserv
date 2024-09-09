#!/bin/bash

printf "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"

cat <<EOF

<h3>Date: </h3>
<pre>$(date)
$(date -u)</pre>

<h4>Days have passed since the beginning of the year: </h4>
<pre>$(date +%j)</pre>
EOF

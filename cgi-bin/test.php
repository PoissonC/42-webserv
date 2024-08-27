#!/usr/bin/php-cgi
<?php
// Tell the browser this is a plain text response
header("Content-Type: text/plain");



// Output the request method
echo "Request Method: " . $_SERVER['REQUEST_METHOD'] . "\n";

// Output the query string (if any)
if (isset($_SERVER['QUERY_STRING'])) {
    echo "Query String: " . $_SERVER['QUERY_STRING'] . "\n";
}

// Output POST data (if any)
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = file_get_contents('php://input');
    echo "POST Data: " . $input;
    echo "\n";
    foreach ($_POST as $key => $value) {
        echo "$key: $value\n";
    }
}

// Output any additional environment variables (e.g., from the web server)
echo "Additional Environment Variables:\n";
foreach ($_SERVER as $key => $value) {
    if (strpos($key, 'HTTP_') === 0 || in_array($key, ['SERVER_SOFTWARE', 'SERVER_NAME', 'SERVER_PROTOCOL', 'REQUEST_URI'])) {
        echo "$key: $value\n";
    }
}
?>

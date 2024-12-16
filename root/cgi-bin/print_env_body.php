#!/usr/bin/env php
<?php
// Get the request body
// $requestBody = file_get_contents("php://input");

// Prevent PHP from sending the default Content-Type header
header("Content-Type: text/html");  // Set custom Content-Type header
header_remove("Content-Type"); 

// $htmlContent = "<html><body>Hello World!</body></html>";

// Get the request body
$requestBody = file_get_contents("php://input");

// Generate HTML content and store it in a variable
$htmlContent = "<html>";
$htmlContent .= "<head><title>Environment Variables</title></head>";
$htmlContent .= "<body>";
$htmlContent .= "<h1>Environment Variables</h1>";
$htmlContent .= "<ul>";

// Iterate through environment variables and display them
foreach ($_SERVER as $key => $value) {
    $htmlContent .= "<li>" . htmlspecialchars($key) . "=" . htmlspecialchars($value) . "</li>";
}

$htmlContent .= "</ul>";
$htmlContent .= "<h2>Body</h2>";
$htmlContent .= "<div>" . htmlspecialchars($requestBody) . "</div>";
$htmlContent .= "</body>";
$htmlContent .= "</html>";

// Calculate the Content-Length
$contentLength = strlen($htmlContent);

// Output HTTP headers with Content-Length
echo("HTTP/1.1 200 OK\n");
echo("Content-Type: text/html\n");
echo("Access-Control-Allow-Origin: *\n");
echo("Access-Control-Allow-Methods: GET, POST, DELETE\n");
echo("Access-Control-Allow-Headers: *\n");
echo("Content-Length: " . $contentLength . "\r\n\r\n");
// Output the HTML content
echo $htmlContent;
?>


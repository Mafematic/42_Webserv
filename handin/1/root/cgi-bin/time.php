<?php
$current_time = date("l, F d, Y h:i:s A");

header("Content-Type: text/html");
header_remove("Content-Type");

$html_content = "<html>";
$html_content .= "<head>";
$html_content .= "<title>Current Time</title>";
$html_content .= "<style>";
$html_content .= "body {";
$html_content .= "font-family: Arial, sans-serif;";
$html_content .= "background-color: #f0f8ff;";
$html_content .= "text-align: center;";
$html_content .= "padding: 50px;";
$html_content .= "}";
$html_content .= "h1 {";
$html_content .= "color: #333;";
$html_content .= "font-size: 48px;";
$html_content .= "margin-bottom: 20px;";
$html_content .= "}";
$html_content .= ".time {";
$html_content .= "font-size: 36px;";
$html_content .= "font-weight: bold;";
$html_content .= "color: #4CAF50;";
$html_content .= "}";
$html_content .= ".footer {";
$html_content .= "margin-top: 50px;";
$html_content .= "font-size: 18px;";
$html_content .= "color: #666;";
$html_content .= "}";
$html_content .= "</style>";
$html_content .= "</head>";
$html_content .= "<body>";
$html_content .= "<h1>Current Time</h1>";
$html_content .= '<div class="time">';
$html_content .= $current_time;
$html_content .= "</div>";
$html_content .= '<div class="footer">';
$html_content .= "<p>Powered by PHP</p>";
$html_content .= "</div>";
$html_content .= "</body>";
$html_content .= "</html>";

echo ("HTTP/1.1 200 OK\n");
echo ("Content-Type: text/html\n");
echo ("Access-Control-Allow-Origin: *\n");
echo ("Access-Control-Allow-Methods: GET, POST, DELETE\n");
echo ("Access-Control-Allow-Headers: *\n");
echo ("Content-Length: " . strlen($html_content) . "\r\n\r\n");

echo $html_content;
?>
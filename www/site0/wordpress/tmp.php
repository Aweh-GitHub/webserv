<?php

header('Content-Type: text/plain');

echo "HTTP_HOST=" . ($_SERVER['HTTP_HOST'] ?? '') . "\n";
echo "SERVER_NAME=" . ($_SERVER['SERVER_NAME'] ?? '') . "\n";
echo "SERVER_PORT=" . ($_SERVER['SERVER_PORT'] ?? '') . "\n";
echo "REQUEST_URI=" . ($_SERVER['REQUEST_URI'] ?? '') . "\n";
echo "HTTPS=" . ($_SERVER['HTTPS'] ?? '') . "\n";

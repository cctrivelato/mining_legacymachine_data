<?php
// webserver_publishing.php

// Get the POST data
$vibration = isset($_POST['vibration']) ? $_POST['vibration'] : '';
$laps = isset($_POST['laps']) ? $_POST['laps'] : '';
$button = isset($_POST['button']) ? $_POST['button'] : '';
$machine_id = isset($_POST['machine_id']) ? $_POST['machine_id'] : '';
$node_id = isset($_POST['node_id']) ? $_POST['node_id'] : '';
$temperature = isset($_POST['temperature']) ? $_POST['temperature'] : '';
$humidity = isset($_POST['humidity']) ? $_POST['humidity'] : '';

// Connect to the database
$servername = "local";
$username = "admin";
$password = "******";
$dbname = "test";

$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

// Insert data into the table
$sql = "INSERT INTO temp_sensor_data (machine_id, node_id, vibration, laps, button_status, temperature, humidity) 
        VALUES ('$machine_id', '$node_id', '$vibration', '$laps', '$button', '$temperature', '$humidity')";

if ($conn->query($sql) === TRUE) {
  echo "New record created successfully";
} else {
  echo "Error: " . $sql . "<br>" . $conn->error;
}

// Close the connection
$conn->close();
?>


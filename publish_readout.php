<?php
// Database Credentials
$host = "local";
$dbname = "test";
$username = "admin";
$password = "*****";

// Establish database connection
try {
	$pdo = new PDO("mysql:host=$host;dbname=$dbname", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Could not connect to the database: " . $e->getMessage());
}

// Check if data was sent via POST
if ($_SERVER["REQUEST_METHOD"] === "POST" && isset($_POST['data_value'])) {
    $data_value = $_POST['data_value'];

    // Prepare and execute the SQL statement
    $stmt = $pdo->prepare("INSERT INTO dp900_data (data_value) VALUES (:data_value)");
    $stmt->bindParam(':data_value', $data_value);

    if ($stmt->execute()) {
        echo "Data inserted successfully";
    } else {
        echo "Error inserting data";
    }
} else {
    echo "No data received";
}
?>
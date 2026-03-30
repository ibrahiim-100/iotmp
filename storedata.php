<?php
$conn = new mysqli("localhost", "username", "password", "iot_safety");

$temp = $_GET['s1'];
$smoke = $_GET['s2'];
$fire = $_GET['s3'];
$light = $_GET['s4'];

$sql = "INSERT INTO safety_data (temperature, smoke, fire, light_status)
VALUES ('$temp', '$smoke', '$fire', '$light')";

if ($conn->query($sql) === TRUE) {
    echo "Data Stored";
} else {
    echo "Error";
}
$conn->close();
?>
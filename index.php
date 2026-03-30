<?php
$conn = new mysqli("localhost", "username", "password", "iot_safety");
$result = $conn->query("SELECT * FROM safety_data ORDER BY id DESC");
?>

<!DOCTYPE html>
<html>
<head>
    <title>IoT Safety Dashboard</title>
</head>
<body>

<h2>Industrial Safety System</h2>

<table border="1">
<tr>
<th>Temp</th>
<th>Smoke</th>
<th>Fire</th>
<th>Light</th>
<th>Time</th>
</tr>

<?php
while($row = $result->fetch_assoc()) {
echo "<tr>
<td>{$row['temperature']}</td>
<td>{$row['smoke']}</td>
<td>{$row['fire']}</td>
<td>{$row['light_status']}</td>
<td>{$row['created_at']}</td>
</tr>";
}
?>

</table>

</body>
</html>
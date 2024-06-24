<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

$servername = "localhost"; // Change if needed
$username = "root"; // Change if needed
$password = ""; // Change if needed
$dbname = "testsystemdbcrest";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$imei = isset($_GET['imei']) ? $_GET['imei'] : '';
$hours = isset($_GET['hours']) ? intval($_GET['hours']) : 0;

// Fetch the data
$sql = "SELECT t.TestID, t.TestDate, t.TestTime, t.DeviceType, r.MeasurementType, r.MeasurementValue 
        FROM teststable t
        JOIN testresults r ON t.TestID = r.TestID";

$whereClauses = [];
if (!empty($imei)) {
    $whereClauses[] = "r.MeasurementType = 'IMEI' AND r.MeasurementValue = '$imei'";
}
if ($hours > 0) {
    $whereClauses[] = "t.TestDate >= DATE_SUB(NOW(), INTERVAL $hours HOUR)";
}
if (!empty($whereClauses)) {
    $sql .= " WHERE " . implode(' AND ', $whereClauses);
}

$sql .= " ORDER BY t.TestDate DESC, t.TestTime DESC, t.TestID, r.MeasurementType";

$result = $conn->query($sql);

$tests = array();

if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $testID = $row['TestID'];
        if (!isset($tests[$testID])) {
            $tests[$testID] = array(
                "TestID" => $testID,
                "TestDate" => $row['TestDate'],
                "TestTime" => $row['TestTime'],
                "DeviceType" => $row['DeviceType'],
                "Results" => array()
            );
        }
        $tests[$testID]["Results"][] = array(
            "MeasurementType" => $row['MeasurementType'],
            "MeasurementValue" => $row['MeasurementValue']
        );
    }
}

$conn->close();

echo json_encode(array_values($tests));
?>
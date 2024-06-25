<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "testsystemdbcrest";

// Create a connection to the database
$conn = new mysqli($servername, $username, $password, $dbname);

// Check the connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

// Get the data from the POST request
if (isset($_POST['imei'])) {
  $imei = filter_input(INPUT_POST, 'imei', FILTER_SANITIZE_STRING);
  $iccid = filter_input(INPUT_POST, 'iccid', FILTER_SANITIZE_STRING);
  $firmware = filter_input(INPUT_POST, 'firmware', FILTER_SANITIZE_STRING);
  $vbatt = filter_input(INPUT_POST, 'vbatt', FILTER_SANITIZE_STRING);
  $rtc_con = filter_input(INPUT_POST, 'rtc_con', FILTER_SANITIZE_STRING);
  $flash_con = filter_input(INPUT_POST, 'flash_con', FILTER_SANITIZE_STRING);
  $network = filter_input(INPUT_POST, 'network', FILTER_SANITIZE_STRING);
  $revision = filter_input(INPUT_POST, 'revision', FILTER_SANITIZE_STRING);
  $imsi = filter_input(INPUT_POST, 'imsi', FILTER_SANITIZE_STRING);
  $rtc_sync = filter_input(INPUT_POST, 'rtc_sync', FILTER_SANITIZE_STRING);
  $sht20 = filter_input(INPUT_POST, 'sht20', FILTER_SANITIZE_STRING);
  $vb86 = filter_input(INPUT_POST, 'vb86', FILTER_SANITIZE_STRING);
  $klay = filter_input(INPUT_POST, 'klay', FILTER_SANITIZE_STRING);
  $deviceType = filter_input(INPUT_POST, 'deviceType', FILTER_SANITIZE_STRING);

  // Insert the test into the database using prepared statements
  $stmt = $conn->prepare("INSERT INTO teststable (TestDate, TestTime, DeviceType) VALUES (NOW(), NOW(), ?)");
  $stmt->bind_param("s", $deviceType);
  $stmt->execute();

  if ($stmt->affected_rows > 0) {
    $testID = $stmt->insert_id; // Get the TestID of the inserted test

    // Insert the test results into the database using prepared statements
    $stmt = $conn->prepare("INSERT INTO testresults (TestID, MeasurementType, MeasurementValue) VALUES (?, ?, ?)");
    foreach (['IMEI' => $imei, 'ICCID' => $iccid, 'Firmware' => $firmware, 'Vbatt' => $vbatt, 'RTC_con' => $rtc_con,
              'Flash_con' => $flash_con, 'Network' => $network, 'Revision' => $revision, 'IMSI' => $imsi, 
              'RTC_sync' => $rtc_sync, 'SHT20' => $sht20, 'VB86' => $vb86, 'Klay' => $klay] as $type => $value) {
      $stmt->bind_param("iss", $testID, $type, $value);
      $stmt->execute();
    }

    echo "New record created successfully";
  } else {
    echo "Error: " . $conn->error;
  }
} else {
  echo "No data received";
}

$conn->close();
?>

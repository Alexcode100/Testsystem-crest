<?php
$servername = "localhost"; // Change if needed
$username = "root"; // Change if needed
$password = ""; // Change if needed
$dbname = "testsystemdbcrest";

// Create a connection to the database
$conn = new mysqli($servername, $username, $password, $dbname);

// Check the connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

// Get the data from the POST request
if (isset($_POST['imei'])) {
  $imei = $_POST['imei'];
  $iccid = $_POST['iccid'];
  $firmware = $_POST['firmware'];
  $vbatt = $_POST['vbatt'];
  $rtc_con = $_POST['rtc_con'];
  $flash_con = $_POST['flash_con'];
  $network = $_POST['network'];
  $revision = $_POST['revision'];
  $imsi = $_POST['imsi'];
  $rtc_sync = $_POST['rtc_sync'];
  $sht20 = $_POST['sht20'];
  $vb86 = $_POST['vb86'];
  $klay = $_POST['klay'];
  $deviceType = $_POST['deviceType'];

  // Insert the test into the database
  $sql = "INSERT INTO teststable (TestDate, TestTime, DeviceType) VALUES (NOW(), NOW(), '$deviceType')";

  if ($conn->query($sql) === TRUE) {
    $testID = $conn->insert_id; // Get the TestID of the inserted test

    // Insert the test results into the database
    $results = [
      'IMEI' => $imei,
      'ICCID' => $iccid,
      'Firmware' => $firmware,
      'Vbatt' => $vbatt,
      'RTC_con' => $rtc_con,
      'Flash_con' => $flash_con,
      'Network' => $network,
      'Revision' => $revision,
      'IMSI' => $imsi,
      'RTC_sync' => $rtc_sync,
      'SHT20' => $sht20,
      'VB86' => $vb86,
      'Klay' => $klay
    ];

    foreach ($results as $type => $value) {
      $sql = "INSERT INTO testresults (TestID, MeasurementType, MeasurementValue) VALUES ('$testID', '$type', '$value')";
      if ($conn->query($sql) !== TRUE) {
        echo "Error: " . $sql . "<br>" . $conn->error;
      }
    }

    echo "New record created successfully";
  } else {
    echo "Error: " . $sql . "<br>" . $conn->error;
  }
} else {
  echo "No data received";
}

$conn->close();
?>

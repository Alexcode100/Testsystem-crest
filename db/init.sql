CREATE DATABASE IF NOT EXISTS TestSystemDBCrest;

-- Use the newly created database
USE TestSystemDBCrest;

-- Drop Tables if they exist
DROP TABLE IF EXISTS TestResults;
DROP TABLE IF EXISTS TestsTable;

-- Create TestsTable
CREATE TABLE TestsTable (
    TestID INT AUTO_INCREMENT PRIMARY KEY,
    TestDate DATE NOT NULL,
    TestTime TIME NOT NULL,
    DeviceType VARCHAR(255) NOT NULL,
);

-- Create TestResults
CREATE TABLE TestResults (
    TestSpecificID INT AUTO_INCREMENT PRIMARY KEY,
    TestID INT NOT NULL,
    MeasurementType VARCHAR(255) NOT NULL,
    MeasurementValue VARCHAR(255) NOT NULL,
    FOREIGN KEY (TestID) REFERENCES TestsTable(TestID) ON DELETE CASCADE
);
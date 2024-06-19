# Testsystem-crest
Database added to testsystem


## Database Initialisatie
De database kan worden geïnitialiseerd met het script in de `/db` directory.

### Stappen:
1. Start XAMPP en zorg ervoor dat MySQL draait.
2. Open phpMyAdmin en voer het script `init.sql` uit, dat zich in de `/db` directory bevindt.

#### Het SQL script `init.sql`:
```sql
-- Create Database
CREATE DATABASE IF NOT EXISTS testsystemdbcrest;

-- Use the newly created database
USE testsystemdbcrest;

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
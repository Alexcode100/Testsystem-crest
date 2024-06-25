-- Insert data for teststable
INSERT INTO teststable (TestDate, TestTime, DeviceType) VALUES
('2024-06-25', '10:04:40', 'F4:12:FA:FA:06:48'),
('2024-06-25', '10:50:36', 'F4:12:FA:FA:06:48'),
('2024-06-25', '13:45:18', 'F4:12:FA:FA:06:48');

-- Get the last inserted TestID
SET @last_test_id = (SELECT MAX(TestID) FROM teststable);

-- Seeding data for testresults, ensuring the TestID references are correct
INSERT INTO testresults (TestID, MeasurementType, MeasurementValue) VALUES
(@last_test_id - 2, 'IMEI', '867787050304595'),
(@last_test_id - 2, 'ICCID', '89882280000008996015'),
(@last_test_id - 2, 'Firmware', '21.01'),
(@last_test_id - 2, 'Vbatt', '3594'),
(@last_test_id - 2, 'RTC_con', 'SUCCESS'),
(@last_test_id - 2, 'Flash_con', 'SUCCESS'),
(@last_test_id - 2, 'Network', 'SUCCESS 1 1'),
(@last_test_id - 2, 'Revision', 'BC95GVBAR02A03'),
(@last_test_id - 2, 'IMSI', '901405700026311'),
(@last_test_id - 2, 'RTC_sync', 'SUCCESS'),
(@last_test_id - 2, 'SHT20', 'SUCCESS'),
(@last_test_id - 2, 'VB86', '64487687'),
(@last_test_id - 2, 'Klay', NULL),
(@last_test_id - 1, 'IMEI', '867787050304595'),
(@last_test_id - 1, 'ICCID', '89882280000008996015'),
(@last_test_id - 1, 'Firmware', '21.01'),
(@last_test_id - 1, 'Vbatt', '3594'),
(@last_test_id - 1, 'RTC_con', 'SUCCESS!!!!'),
(@last_test_id - 1, 'Flash_con', 'SUCCESS'),
(@last_test_id - 1, 'Network', 'SUCCESS 1 1'),
(@last_test_id - 1, 'Revision', 'BC95GVBAR02A03'),
(@last_test_id - 1, 'IMSI', '901405700026311'),
(@last_test_id - 1, 'RTC_sync', 'SUCCESS'),
(@last_test_id - 1, 'SHT20', 'SUCCESS'),
(@last_test_id - 1, 'VB86', '64487687'),
(@last_test_id - 1, 'Klay', NULL),
(@last_test_id, 'IMEI', '867787050314595'),
(@last_test_id, 'ICCID', '89882280000008996015'),
(@last_test_id, 'Firmware', '21.01'),
(@last_test_id, 'Vbatt', '3594'),
(@last_test_id, 'RTC_con', 'SUCCESS'),
(@last_test_id, 'Flash_con', 'SUCCESS'),
(@last_test_id, 'Network', 'SUCCESS 1 1'),
(@last_test_id, 'Revision', 'BC95GVBAR02A03'),
(@last_test_id, 'IMSI', '901405700026311'),
(@last_test_id, 'RTC_sync', 'SUCCESS'),
(@last_test_id, 'SHT20', 'SUCCESS'),
(@last_test_id, 'VB86', '64487687'),
(@last_test_id, 'Klay', NULL);
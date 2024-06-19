-- V2__Remove_IDSensor_and_Result.sql

USE TestSystemDBCrest;

ALTER TABLE TestsTable DROP COLUMN IDSensor;
ALTER TABLE TestResults DROP COLUMN Result;
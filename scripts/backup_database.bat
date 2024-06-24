@echo off
set db_name=testsystemdbcrest
set backup_path=C:\Users\barde\OneDrive\Bureaublad\TestOmgevingProjectCRESTStage\S.P.I.T.S. 2\Code\db\Backups
set cnf_path=C:\Users\barde\OneDrive\Bureaublad\TestOmgevingProjectCRESTStage\S.P.I.T.S. 2\Code\Secure_directory\my.cnf

set year=%date:~10,4%
set month=%date:~4,2%
set day=%date:~7,2%
set hour=%time:~0,2%
set minute=%time:~3,2%
set second=%time:~6,2%

REM Remove spaces from time
if "%hour:~0,1%" == " " set hour=0%hour:~1,1%

set datetime=%year%%month%%day%%hour%%minute%%second%

mysqldump --defaults-extra-file=%cnf_path% %db_name% > "%backup_path%\%db_name%-%datetime%.sql"

echo Backup completed: %backup_path%\%db_name%-%datetime%.sql

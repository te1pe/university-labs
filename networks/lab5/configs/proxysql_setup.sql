DELETE FROM mysql_servers;
INSERT INTO mysql_servers(hostgroup_id, hostname, port, weight) VALUES
    (20, '192.168.64.11', 3306, 100),
    (20, '192.168.64.12', 3306, 100),
    (20, '192.168.64.13', 3306, 100);
INSERT INTO mysql_servers(hostgroup_id, hostname, port, weight) VALUES
    (10, '192.168.64.11', 3306, 1000);
INSERT INTO mysql_servers(hostgroup_id, hostname, port, weight) VALUES
    (30, '192.168.64.12', 3306, 1000),
    (30, '192.168.64.13', 3306, 1000);

UPDATE global_variables SET variable_value='monitor' WHERE variable_name='mysql-monitor_username';
UPDATE global_variables SET variable_value='monitor_pass' WHERE variable_name='mysql-monitor_password';

DELETE FROM mysql_users;
INSERT INTO mysql_users(username, password, default_hostgroup, active) VALUES
    ('app', 'app_pass', 10, 1);

DELETE FROM mysql_galera_hostgroups;
INSERT INTO mysql_galera_hostgroups (writer_hostgroup, backup_writer_hostgroup, reader_hostgroup, offline_hostgroup, active, max_writers, writer_is_also_reader, max_transactions_behind)
VALUES (10, 30, 20, 9999, 1, 1, 2, 100);

LOAD MYSQL SERVERS TO RUNTIME;
LOAD MYSQL USERS TO RUNTIME;
LOAD MYSQL VARIABLES TO RUNTIME;
SAVE MYSQL SERVERS TO DISK;
SAVE MYSQL USERS TO DISK;
SAVE MYSQL VARIABLES TO DISK;

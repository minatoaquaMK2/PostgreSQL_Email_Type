
# User defined e-mail type
A new data type for dealing with email addresses. 

## RUN
```shell
$ cd postgresql-11.3/src/tutorial
$make
```
Once you have made the email while still in the src/tutorial directory, start a psql session on your test database and run the email.sql file as follows:
```shell
$ psql test
psql (11.3)
Type "help" for help.

\i email.sql
```
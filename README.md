MySQL UDF plugin for decoding JWT
===
mysql_jwt is a MySQL UDF plugin for decoding JSON Web Token(JWT).

Decode JWT in the query
---

```sql
> SET @token = 'eyJhbGciOiJub25lIn0.eyJzdWIiOjEyMzQ1Njc4OTAsImlzcyI6ImV4YW1wbGUuY29tIn0.';
> SELECT decode_jwt(@token);
*************************** 1. row ***************************
decode_jwt(@token): {
    "iss": example.com
    "sub": 1234567890
}
1 row in set (0.00 sec)
```

How to install
---

```sh
$ git submodule init

# on OSX
$ g++ -o mysql_jwt.so mysql_jwt.cc `mysql_config --cflags` -bundle
# on Linux
$ g++ -o mysql_jwt.so mysql_jwt.cc `mysql_config --cflags` -shared -fPIC

$ cp mysql_jwt.so `mysql_config --plugindir`/
$ mysql -u root -e "CREATE FUNCTION decode_jwt RETURNS STRING SONAME 'mysql_jwt.so'"
```

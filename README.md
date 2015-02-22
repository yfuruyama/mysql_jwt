mysql_jwt
===
mysql_jwt is a MySQL UDF plugin for decoding JSON Web Token(JWT).

Decode JWT in the query
---
After installing this plugin, you can use `decode_jwt` function.

```
> SET @token = 'eyJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJodHRwOi8vZXhhbXBsZS5jb20iLCJzdWIiOiIxMjM0NTY3ODkwIiwiYXVkIjoiaHR0cDovL2FwcC5leGFtcGxlLmNvbSIsImV4cCI6MTQyNDQzNzQ5MSwiaWF0IjoxNDI0NDM2NTkxLCJqdGkiOiIxMjM0NTY3ODkwIn0.V0SEo1Y1kurWp2bSYU9gEQ2K9nweII_RNIlYEBRHdWY';
> SELECT decode_jwt(@token)\G
*************************** 1. row ***************************
decode_jwt(@token): {"iss":"http://example.com","sub":"1234567890","aud":"http://app.example.com","exp":1424437491,"iat":1424436591,"jti":"1234567890"}
1 row in set (0.00 sec)
```

The claim keyword can be specified at 2nd argument of `decode_jwt()`.
```
> SELECT decode_jwt(@token, 'iss');
+---------------------------+
| decode_jwt(@token, 'iss') |
+---------------------------+
| http://example.com        |
+---------------------------+
1 row in set (0.00 sec)

> SELECT FROM_UNIXTIME(decode_jwt(@token, 'exp'));
+------------------------------------------+
| FROM_UNIXTIME(decode_jwt(@token, 'exp')) |
+------------------------------------------+
| 2015-02-20 22:04:51                      |
+------------------------------------------+
1 row in set (0.00 sec)
```

How to install
---

```sh
$ git clone git@github.com:addsict/mysql_jwt.git && cd mysql_jwt
$ git submodule init && git submodule update

# on OSX
$ g++ -o mysql_jwt.so mysql_jwt.cc `mysql_config --cflags` -bundle
# on Linux
$ g++ -o mysql_jwt.so mysql_jwt.cc `mysql_config --cflags` -shared -fPIC

$ cp mysql_jwt.so `mysql_config --plugindir`/
$ mysql -u root -e "CREATE FUNCTION decode_jwt RETURNS STRING SONAME 'mysql_jwt.so'"
```

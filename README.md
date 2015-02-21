mysql_jwt - MySQL UDF for decoding JWT
===
mysql_jwt is a MySQL UDF plugin for decoding JSON Web Token(JWT).

How to install
---

```sh
$ make
$ make install
$ mysql -u root -e "CREATE FUNCTION decode_jwt RETURNS STRING SONAME 'mysql_jwt.so'"
```

Decode JWT in a query
---

```sql
> SET @token = 'eyJhbGciOiJub25lIn0.eyJzdWIiOjEyMzQ1Njc4OTAsImlzcyI6Imh0dHA6Ly9leGFtcGxlLmNvbSJ9.';
> SELECT decode_jwt(@token);
*************************** 1. row ***************************
decode_jwt(@token): {
    "iss": http://example.com
    "sub": 1234567890
}
1 row in set (0.00 sec)
```

extern "C" {
#include <mysql/mysql.h>
}
#include <string>
#include <sstream>
#include "base64/base64.h"
#include "picojson/picojson.h"

extern "C" {
char *decode_jwt(UDF_INIT *initid, UDF_ARGS *args,
                 char *result, unsigned long *res_length,
                 char *null_value, char *error);
my_bool decode_jwt_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void decode_jwt_deinit(UDF_INIT *initid);
}

std::string get_claim_part(std::string jwt)
{
  std::stringstream ss(jwt);
  std::string header;
  std::string claim;

  // ignore header part
  std::getline(ss, header, '.');

  std::getline(ss, claim, '.');
  return claim;
}

int format_pretty(picojson::object obj, std::string *out)
{
  (*out) += "{\n";

  const char* keys[] = {"iss", "sub", "aud", "exp", "iat", "jti"};
  for (int i = 0, l = sizeof(keys) / sizeof(char*); i < l; ++i) {
    const char* key = keys[i];
    if (!obj[key].is<picojson::null>()) {
      char buf[1024];
      if (obj[key].is<std::string>()) {
        sprintf(buf, "    \"%s\": \"%s\",\n", key, obj[key].to_str().c_str());
      } else {
        sprintf(buf, "    \"%s\": %s,\n", key, obj[key].to_str().c_str());
      }
      (*out) += buf;
    }
  }

  (*out) += "}";

  return out->size();
}

my_bool decode_jwt_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count != 1) {
    strcpy(message, "Incorrect parameter count in the call to function 'decode_jwt'");
    return 1;
  }
  if (args->arg_type[0] != STRING_RESULT) {
    strcpy(message, "1st argument of 'decode_jwt' should be string");
    return 1;
  }

  initid->const_item = 0;
  initid->maybe_null = 1;

  return 0;
}

void decode_jwt_deinit(UDF_INIT *initid)
{
}

char *decode_jwt(UDF_INIT *initid, UDF_ARGS *args,
                 char *result, unsigned long *res_length,
                 char *is_null, char *error)
{
  std::string jwt = (char*)args->args[0];

  const std::string claim_part = get_claim_part(jwt);
  if (claim_part.empty()) {
    *is_null = 1;
    return NULL;
  }

  std::string claim;
  bool is_success = Base64::Decode(claim_part, &claim);
  if (!is_success) {
    *is_null = 1;
    return NULL;
  }

  picojson::value v;
  std::string err = picojson::parse(v, claim);
  if (!err.empty()) {
    *is_null = 1;
    return NULL;
  }

  picojson::object obj = v.get<picojson::object>();
  std::string out; 
  int out_len = format_pretty(obj, &out);

  strcpy(result, out.c_str());
  *res_length = out_len;

  return result;
}


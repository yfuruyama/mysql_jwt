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

my_bool decode_jwt_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count != 1 && args->arg_count != 2) {
    strcpy(message, "decode_jwt() can accept one or two arguments");
    return 1;
  }
  if (args->arg_type[0] != STRING_RESULT) {
    strcpy(message, "1st argument of decode_jwt() should be string");
    return 1;
  } else if (args->arg_count == 2 && args->arg_type[1] != STRING_RESULT) {
    strcpy(message, "2nd argument of decode_jwt() should be string");
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

  std::string decoded;
  bool is_success = Base64::Decode(claim_part, &decoded);
  if (!is_success) {
    *is_null = 1;
    return NULL;
  }

  if (args->arg_count == 2) {
    std::string claim_key = (char*)args->args[1];

    picojson::value v;
    std::string err = picojson::parse(v, decoded);
    if (!err.empty()) {
      *is_null = 1;
      return NULL;
    }

    picojson::object obj = v.get<picojson::object>();
    std::string claim = obj[claim_key].to_str();
    strcpy(result, claim.c_str());
    *res_length = claim.size();
  } else {
    strcpy(result, decoded.c_str());
    *res_length = decoded.size();
  }

  return result;
}


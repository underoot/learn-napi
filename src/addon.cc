#include "addon.h"

#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      const char* err_message = error_info->error_message;        \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (err_message == NULL)               \
            ? "empty error message"                               \
            : err_message;                                        \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while(0)

#include <stdio.h>

int64_t do_factorial(int64_t a) {
  if (a == 1) return a;
  return a * do_factorial(a - 1);
}

static napi_value
factorial(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  int64_t *arg = new int64_t;
  int64_t value;
  napi_value result;
  napi_value argv[1];
  void **data;
  napi_valuetype type;

  
  napi_get_cb_info(env, info, &argc, argv, NULL, data);
  napi_get_value_int64(env, argv[0], arg);
  napi_typeof(env, argv[0], &type);

  value = do_factorial(*arg);

  napi_create_int64(env, value, &result);

  return result;
}

napi_value create_addon(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  napi_value exported_function;
  NAPI_CALL(env, napi_create_function(env,
                                      "factorial",
                                      NAPI_AUTO_LENGTH,
                                      factorial,
                                      NULL,
                                      &exported_function));

  NAPI_CALL(env, napi_set_named_property(env,
                                         result,
                                         "factorial",
                                         exported_function));

  return result;
}

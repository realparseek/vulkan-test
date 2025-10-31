#include <kd/context.h>

kd_context kd_context_create() {
  return (kd_context) {
    .window_api_initialized = 0
  };
}

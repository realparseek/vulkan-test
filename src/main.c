#include <kd/window.h>

int main() { 
  kd_context kctx = kd_context_create();
  kd_window* kwin = kd_window_create(&kctx, 1024, 768, "title");
  
  while (!kd_window_closed(&kctx, kwin)) {
    kd_window_update(&kctx, kwin);
  }

  kd_window_destroy(&kctx, kwin);
  return 0;
}

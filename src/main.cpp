#include "app.h"
#include "commondefs.h"
#include "config.h"
#include "emulatorstate.h"
#include "filedatastorage.h"

App app;

int main(int, char**) {
  return app.run();
}

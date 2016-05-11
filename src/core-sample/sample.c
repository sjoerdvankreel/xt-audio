#include <stdio.h>

extern int PrintDevicesMain(int argc, char** argv);
extern int PrintInfoMain(int argc, char** argv);
extern int CaptureMain(int argc, char** argv);
extern int RenderMain(int argc, char** argv);

int main(int argc, char** argv) {
  printf("PrintDevicesMain:\n");
  PrintDevicesMain(argc, argv);
  printf("PrintInfoMain:\n");
  PrintInfoMain(argc, argv);
  printf("CaptureMain:\n");
  CaptureMain(argc, argv);
  printf("RenderMain:\n");
  RenderMain(argc, argv);
  return 0;
}

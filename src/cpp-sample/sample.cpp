#include <iostream>

extern int PrintDevicesMain(int argc, char** argv);
extern int PrintInfoMain(int argc, char** argv);
extern int CaptureMain(int argc, char** argv);
extern int RenderMain(int argc, char** argv);

int main(int argc, char** argv) {
  std::cout << "PrintDevicesMain:\n";
  PrintDevicesMain(argc, argv);
  std::cout << "PrintInfoMain:\n";
  PrintInfoMain(argc, argv);
  std::cout << "CaptureMain:\n";
  CaptureMain(argc, argv);
  std::cout << "RenderMain:\n";
  RenderMain(argc, argv);
  return 0;
}

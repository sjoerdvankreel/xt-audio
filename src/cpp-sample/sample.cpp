#include <iostream>

extern int PrintSimpleMain(int argc, char** argv);
extern int PrintDetailedMain(int argc, char** argv);
extern int CaptureSimpleMain(int argc, char** argv);
extern int RenderSimpleMain(int argc, char** argv);
extern int CaptureAdvancedMain(int argc, char** argv);
extern int RenderAdvancedMain(int argc, char** argv);
extern int FullDuplexMain(int argc, char** argv);

int main(int argc, char** argv) {
  std::cout << "PrintSimpleMain:\n";
  PrintSimpleMain(argc, argv);
  std::cout << "PrintDetailedMain:\n";
  PrintDetailedMain(argc, argv);
  std::cout << "CaptureSimpleMain:\n";
  CaptureSimpleMain(argc, argv);
  std::cout << "RenderSimpleMain:\n";
  RenderSimpleMain(argc, argv);
  std::cout << "CaptureAdvancedMain:\n";
  CaptureAdvancedMain(argc, argv);
  std::cout << "RenderAdvancedMain:\n";
  RenderAdvancedMain(argc, argv);
  std::cout << "FullDuplexMain:\n";
  FullDuplexMain(argc, argv);
  return 0;
}

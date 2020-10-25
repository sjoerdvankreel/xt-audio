#include <xt-cpp.hpp>
#include <iostream>
#include <string>

extern int PrintSimpleMain(int argc, char** argv);
extern int PrintDetailedMain(int argc, char** argv);
extern int CaptureSimpleMain(int argc, char** argv);
extern int RenderSimpleMain(int argc, char** argv);
extern int CaptureAdvancedMain(int argc, char** argv);
extern int RenderAdvancedMain(int argc, char** argv);
extern int FullDuplexMain(int argc, char** argv);
extern int AggregateMain(int argc, char** argv);

int main(int argc, char** argv) {
  int index = -1;
  if(argc == 2)
    index = std::stoi(std::string(argv[1]));
  try {
    if(index == -1 || index == 0) {
      std::cout << "PrintSimpleMain:\n";
      PrintSimpleMain(argc, argv);
    }
    if(index == -1 || index == 1) {
      std::cout << "PrintDetailedMain:\n";
      PrintDetailedMain(argc, argv);
    }
    if(index == -1 || index == 2) {
      std::cout << "CaptureSimpleMain:\n";
      CaptureSimpleMain(argc, argv);
    }
    if(index == -1 || index == 3) {
      std::cout << "RenderSimpleMain:\n";
      RenderSimpleMain(argc, argv);
    }
    if(index == -1 || index == 4) {
      std::cout << "CaptureAdvancedMain:\n";
      CaptureAdvancedMain(argc, argv);
    }
    if(index == -1 || index == 5) {
      std::cout << "RenderAdvancedMain:\n";
      RenderAdvancedMain(argc, argv);
    }
    if(index == -1 || index == 6) {
      std::cout << "FullDuplexMain:\n";
      FullDuplexMain(argc, argv);
    }
    if(index == -1 || index == 7) {
      std::cout << "AggregateMain:\n";
      AggregateMain(argc, argv);
    }
    return 0;
  } catch(Xt::Exception const& e) {
    auto info = Xt::Audio::GetErrorInfo(e.GetError());
    std::cout << "Error:\n";
    std::cout << "\tSystem: " << info.system << "\n";
    std::cout << "\tCause: " << info.cause << "\n";
    std::cout << "\tFault: " << info.fault << "\n";
    std::cout << "\tText: " << info.text << "\n";
  }
}
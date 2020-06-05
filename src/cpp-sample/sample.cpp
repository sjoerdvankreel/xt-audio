#include <xt-cpp.hpp>
#include <iostream>

extern int PrintSimpleMain(int argc, char** argv);
extern int PrintDetailedMain(int argc, char** argv);
extern int CaptureSimpleMain(int argc, char** argv);
extern int RenderSimpleMain(int argc, char** argv);
extern int CaptureAdvancedMain(int argc, char** argv);
extern int RenderAdvancedMain(int argc, char** argv);
extern int FullDuplexMain(int argc, char** argv);
extern int AggregateMain(int argc, char** argv);

int main(int argc, char** argv) {
  try {
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
    std::cout << "AggregateMain:\n";
    AggregateMain(argc, argv);
    return 0;
  } catch(Xt::Exception const& e) {
    std::cout << "Error:\n";
    std::cout << "\tSystem: " << Xt::Print::SystemToString(Xt::Exception::GetSystem(e.GetError())) << "\n";
    std::cout << "\tCause: " << Xt::Print::CauseToString(Xt::Exception::GetCause(e.GetError())) << "\n";
    std::cout << "\tFault: " << Xt::Exception::GetFault(e.GetError()) << "\n";
    std::cout << "\tText: " << Xt::Exception::GetText(e.GetError()) << "\n";
  }
}
#include <xt/XtAudio.hpp>
#include <string>
#include <cstdlib>
#include <iostream>

extern int AggregateMain();
extern int FullDuplexMain();
extern int PrintSimpleMain();
extern int RenderSimpleMain();
extern int PrintDetailedMain();
extern int CaptureSimpleMain();
extern int RenderAdvancedMain();
extern int CaptureAdvancedMain();

static char const* 
Names[] =
{
  "PrintSimple", "PrintDetailed", "CaptureSimple", "RenderSimple",
  "CaptureAdvanced", "RenderAdvanced", "FullDuplex", "Aggregate"
};

static int(*Samples[])() = 
{
  PrintSimpleMain, PrintDetailedMain, CaptureSimpleMain, RenderSimpleMain,
  CaptureAdvancedMain, RenderAdvancedMain, FullDuplexMain, AggregateMain, 
};

static void 
RunSample(int32_t index)
{
  std::cout << Names[index] << ":\n";
  Samples[index]();
}

int 
main(int argc, char** argv) 
{
  int32_t index = argc == 2? std::stoi(std::string(argv[1])): -1;
  try
  {
    if (index >= 0) RunSample(index);
    else for (int32_t i = 0; i < sizeof(Samples)/sizeof(Samples[0]); i++) RunSample(i);
    return EXIT_SUCCESS;
  } catch (Xt::Exception const& e)
  { 
    std::cout << Xt::Audio::GetErrorInfo(e.GetError()) << "\n"; 
    return EXIT_FAILURE;
  }
}
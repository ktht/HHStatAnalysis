#include "hh-limits/Run2Models/interface/program_main.h"
#include "hh-limits/Run2Models/interface/exception.h"
#include "hh-limits/Run2Models/interface/HH_ModelFactory.h"

namespace {

struct Arguments {
    run::Argument<std::string> uncModelName{"unc-model", "name of the unc model to run"};
    run::Argument<std::string> shapesPathName{"shapes-path", "path where shape files are located"};
    run::Argument<std::string> outputPathName{"output-path", "path where to store created datacards"};
};

} // anonymous namespace

namespace hh_limits {

class CreateWorkspaceProgram {
public:
    CreateWorkspaceProgram(const Arguments& _args) : args(_args) {}

    void Run()
    {
        auto model = HH_ModelFactory::Make(args.uncModelName());
        std::cout << boost::format("Creating datacards for %1% unc model using %2% shapes...")
                     % args.uncModelName() % args.shapesPathName() << std::endl;
        model->CreateDatacards(args.shapesPathName(), args.outputPathName());
        std::cout << boost::format("Datacards are successfully created into '%1%'.") % args.outputPathName()
                  << std::endl;
    }

private:
    Arguments args;
};

} // namespace hh_limits

PROGRAM_MAIN(hh_limits::CreateWorkspaceProgram, Arguments)

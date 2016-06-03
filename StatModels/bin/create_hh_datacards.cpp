/*! Tool to create HH datacards.
This file is part of https://github.com/cms-hh/StatAnalysis. */

#include "HHStatAnalysis/Core/interface/program_main.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/StatModels/interface/StatModelFactory.h"

namespace {

struct Arguments {
    run::Argument<std::string> uncModelName{"stat-model", "name of the stat model to run"};
    run::Argument<std::string> shapesPathName{"shapes-path", "path where shape files are located"};
    run::Argument<std::string> outputPathName{"output-path", "path where to store created datacards"};
};

} // anonymous namespace

namespace hh_analysis {

class CreateHHDatacards {
public:
    CreateHHDatacards(const Arguments& _args) : args(_args) {}

    void Run()
    {
        auto model = stat_models::StatModelFactory::Make(args.uncModelName());
        std::cout << boost::format("Creating datacards for %1% unc model using %2% shapes...")
                     % args.uncModelName() % args.shapesPathName() << std::endl;
        model->CreateDatacards(args.shapesPathName(), args.outputPathName());
        std::cout << boost::format("Datacards are successfully created into '%1%'.") % args.outputPathName()
                  << std::endl;
    }

private:
    Arguments args;
};

} // namespace hh_analysis

PROGRAM_MAIN(hh_analysis::CreateHHDatacards, Arguments)

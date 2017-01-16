/*! Tool to create HH datacards.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include "HHStatAnalysis/Core/interface/program_main.h"
#include "HHStatAnalysis/Core/interface/exception.h"
#include "HHStatAnalysis/StatModels/interface/StatModelFactory.h"
#include "HHStatAnalysis/StatModels/interface/Config.h"

namespace {

struct Arguments {
    run::Argument<std::string> cfg{"cfg", "configuration file"};
    run::Argument<std::string> model_desc{"model-desc", "name of the stat model descriptor in the config"};
    run::Argument<std::string> shapes{"shapes", "file with input shapes"};
    run::Argument<std::string> output_path{"output", "path where to store created datacards"};
};

} // anonymous namespace

namespace hh_analysis {

class CreateHHDatacards {
public:
    CreateHHDatacards(const Arguments& _args) : args(_args) {}

    void Run()
    {
        const StatModelDescriptor model_desc = LoadDescriptor(args.cfg(), args.model_desc());
        auto model = stat_models::StatModelFactory::Make(model_desc);
        std::cout << boost::format("Creating datacards for %1% unc model using %2% shapes...")
                     % model_desc.stat_model % args.shapes() << std::endl;
        model->CreateDatacards(args.shapes(), args.output_path());
        std::cout << boost::format("Datacards are successfully created into '%1%'.") % args.output_path() << std::endl;
    }

private:
    Arguments args;
};

} // namespace hh_analysis

PROGRAM_MAIN(hh_analysis::CreateHHDatacards, Arguments)

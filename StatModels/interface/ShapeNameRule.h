/*! Definition of the class that represents shape name rule.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "Uncertainty.h"

namespace hh_analysis {

class ShapeNameRule {
public:
    static const std::string Analysis, Channel, Bin, Process, Point, Mass, Era, Systematic, Category, Region, Prefix;
    static const std::set<std::string> AllVariables;

    static std::string NumToName(double x);
    static std::string BinName(const std::string& channel, const std::string& category, const std::string& region = "");
    static std::string AddDimSuffix(const std::string& variable, size_t dim = 0);

    ShapeNameRule() {}
    ShapeNameRule(const char* _rule) : rule(_rule) {}
    ShapeNameRule(const std::string& _rule) : rule(_rule) {}
    operator std::string() const { return rule; }
    const std::string& GetRule() const { return rule; }

    bool HasVariables() const { return rule.find('$') != std::string::npos; }
    ShapeNameRule SetVariable(const std::string& variable, const std::string& value) const;

    ShapeNameRule SetAnalysis(const std::string& analysis) const { return SetVariable(Analysis, analysis); }
    ShapeNameRule SetChannel(const std::string& channel) const { return SetVariable(Channel, channel); }
    ShapeNameRule SetProcess(const std::string& process) const { return SetVariable(Process, process); }
    ShapeNameRule SetEra(const std::string& era) const { return SetVariable(Era, era); }
    ShapeNameRule SetCategory(const std::string& category) const { return SetVariable(Category, category); }
    ShapeNameRule SetRegion(const std::string& region) const { return SetVariable(Region, region); }

    ShapeNameRule SetBin(const std::string& bin) const { return SetVariable(Bin, bin); }
    ShapeNameRule SetBin(const std::string& channel, const std::string& category, const std::string& region = "") const;

    ShapeNameRule SetPrefix(const std::string& prefix, size_t dim = 0) const;
    ShapeNameRule SetPoint(double point, size_t dim = 0) const;
    ShapeNameRule SetMass(double mass) const { return SetVariable(Mass, NumToName(mass)); }

    ShapeNameRule AddSystematicVariable() const;
    ShapeNameRule SetSystematic(const std::string& systematic, UncVariation variation) const;

private:
    std::string rule;
};

} // namespace hh_analysis

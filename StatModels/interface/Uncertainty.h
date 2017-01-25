/*! Definition of uncertainty descriptor.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#pragma once

#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "HHStatAnalysis/Core/interface/EnumNameMap.h"

namespace hh_analysis {

ENUM_OSTREAM_OPERATORS()

enum class CorrelationRange { LHC = 4, Experiment = 3, Analysis = 2, Channel = 1, Category = 0 };

inline bool operator<=(CorrelationRange a, CorrelationRange b)
{
    return static_cast<int>(a) <= static_cast<int>(b);
}

enum class UncDistributionType { lnN, shape };
ENUM_NAMES(UncDistributionType) = {
    { UncDistributionType::lnN, "lnN" },
    { UncDistributionType::shape, "shape" }
};

struct Uncertainty {
    CorrelationRange correlation_range = CorrelationRange::Experiment;
    UncDistributionType distr_type = UncDistributionType::lnN;
    std::string name;
    std::vector<std::string> analysis_names, channel_names, category_names;

    Uncertainty() {}
    Uncertainty(CorrelationRange _correlation_range, UncDistributionType _distr_type, const std::string& _name,
                const std::vector<std::string>& _analysis_names = {},
                const std::vector<std::string>& _channel_names = {},
                const std::vector<std::string>& _category_names = {}) :
        correlation_range(_correlation_range), distr_type(_distr_type), name(_name), analysis_names(_analysis_names),
        channel_names(_channel_names), category_names(_category_names)
    {}

    virtual ~Uncertainty() {}

    Uncertainty DistrType(UncDistributionType _distr_type) const
    {
        Uncertainty u(*this);
        u.distr_type = _distr_type;
        return u;
    }

    Uncertainty Analysis(const std::string& analysis) const
    {
        Uncertainty u(*this);
        u.analysis_names = { analysis };
        return u;
    }

    Uncertainty Analyses(const std::vector<std::string>& analyses) const
    {
        Uncertainty u(*this);
        u.analysis_names = analyses;
        return u;
    }

    Uncertainty Channel(const std::string& channel) const
    {
        Uncertainty u(*this);
        u.channel_names = { channel };
        return u;
    }

    Uncertainty Channels(const std::vector<std::string>& channels) const
    {
        Uncertainty u(*this);
        u.channel_names = channels;
        return u;
    }

    Uncertainty Category(const std::string& category) const
    {
        Uncertainty u(*this);
        u.category_names = { category };
        return u;
    }

    Uncertainty Categories(const std::vector<std::string>& categories) const
    {
        Uncertainty u(*this);
        u.category_names = categories;
        return u;
    }

    std::string FullName() const
    {
        static const std::string exp_name = "CMS";
        static const std::string era = "$ERA";
        static const std::string analysis = "$ANALYSIS";
        static const std::string channel = "$CHANNEL";
        static const std::string category = "$BIN";
        static const std::string sep = "_";
        std::ostringstream ss;
        if(correlation_range <= CorrelationRange::Experiment)
            ss << exp_name << sep;
        if(distr_type == UncDistributionType::shape)
            ss << distr_type << sep;
        ss << name << sep;
        if(correlation_range <= CorrelationRange::Analysis)
            ss << analysis << sep;
//        if(correlation_range <= CorrelationRange::Channel)
//            ss << channel << sep;
        if(correlation_range <= CorrelationRange::Category)
            ss << category << sep;
        ss << era;
        return ss.str();
    }

    bool operator<(const Uncertainty& other) const
    {
        if(correlation_range != other.correlation_range) return correlation_range < other.correlation_range;
        if(name != other.name) return name < other.name;
        return FullName() < other.FullName();
    }

    template<typename ...T>
    void Apply(ch::CombineHarvester& cb, const ch::syst::SystMap<T...>& syst_map,
               const std::vector<std::string>& processes, const std::vector<std::string>& processes_2 = {},
               const std::vector<std::string>& processes_3 = {}) const
    {
        const auto all_processes = ch::JoinStr({processes, processes_2, processes_3});
        auto cb_copy = cb.cp().process(all_processes);
        if(analysis_names.size())
            cb_copy = cb_copy.analysis(analysis_names);
        if(channel_names.size())
            cb_copy = cb_copy.channel(channel_names);
        if(category_names.size())
            cb_copy = cb_copy.bin(category_names);
        std::ostringstream ss_distr;
        ss_distr << distr_type;
        cb_copy.AddSyst(cb, FullName(), ss_distr.str(), syst_map);
    }

    void Apply(ch::CombineHarvester& cb, double value, const std::vector<std::string>& processes,
               const std::vector<std::string>& processes_2 = {}, const std::vector<std::string>& processes_3 = {}) const
    {
        Apply(cb, ch::syst::SystMap<>::init(value), processes, processes_2, processes_3);
    }

    void Apply(ch::CombineHarvester& cb, const std::vector<std::string>& processes,
               const std::vector<std::string>& processes_2 = {}, const std::vector<std::string>& processes_3 = {}) const
    {
        Apply(cb, 1.0, processes, processes_2, processes_3);
    }

    void ApplyGlobal(ch::CombineHarvester& cb, const std::vector<std::string>& processes,
                     const std::vector<std::string>& processes_2 = {},
                     const std::vector<std::string>& processes_3 = {}) const;
};

} // namespace hh_analysis

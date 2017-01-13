/*! Python interface for HH stat models.
This file is part of https://github.com/cms-hh/HHStatAnalysis. */

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include "HHStatAnalysis/StatModels/interface/Config.h"

namespace {
template <typename T>
boost::python::list ToPythonList(const std::vector<T>& v)
{
    boost::python::list l;
    for(const auto& item : v)
        l.append(item);
    return l;
}

template <typename Key, typename Value>
boost::python::dict ToPythonDict(const std::map<Key, Value>& m)
{
    boost::python::dict d;
    for(const auto& item : m)
        d[item.first] = item.second;
    return d;
}
} // anonymous namespace

BOOST_PYTHON_MODULE(libHHStatAnalysisStatModels)
{
    using StrVector = std::vector<std::string>;
    using StrStrMap = std::map<std::string, std::string>;
    using StrVectMap = std::map<std::string, StrVector>;
    using namespace boost::python;
    using namespace hh_analysis;
    class_<StrVector>("StrVector")
        .def(vector_indexing_suite<StrVector>());
    class_<StrStrMap>("StrStrMap")
        .def(map_indexing_suite<StrStrMap>());
    class_<StrVectMap>("StrVectMap")
        .def(map_indexing_suite<StrVectMap>());
    enum_<LimitType>("LimitType")
        .value("model_independent", LimitType::ModelIndependent)
        .value("MSSM", LimitType::MSSM);
    class_<RangeWithStep<double>>("RangeWithStep", init<>())
        .def(init<double, double, double>())
        .def("min", &RangeWithStep<double>::min)
        .def("max", &RangeWithStep<double>::max)
        .def("step", &RangeWithStep<double>::step);
    class_<StatModelDescriptor>("StatModelDescriptor")
        .def_readwrite("name", &StatModelDescriptor::name)
        .def_readwrite("channels", &StatModelDescriptor::channels)
        .def_readwrite("limit_type", &StatModelDescriptor::limit_type)
        .def_readwrite("blind", &StatModelDescriptor::blind)
        .def_readwrite("grid_x", &StatModelDescriptor::grid_x)
        .def_readwrite("grid_y", &StatModelDescriptor::grid_y)
        .def_readwrite("th_model_file", &StatModelDescriptor::th_model_file);
    def("LoadDescriptor", LoadDescriptor);
    def("ToList", ToPythonList<std::string>);
    def("ToDict", ToPythonDict<std::string, std::string>);
}

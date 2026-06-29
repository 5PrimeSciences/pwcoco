#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "data.h"
#include "options.h"
#include "pwcoco_run.h"

namespace nb = nanobind;

NB_MODULE(_pwcoco, m)
{
	m.doc() = "PWCoCo Python bindings";

	nb::class_<PwCoCoConfig>(m, "PwCoCoConfig")
	    .def(nb::init<>())
	    .def_rw("bfile", &PwCoCoConfig::bfile)
	    .def_rw("sum_stats1", &PwCoCoConfig::sum_stats1)
	    .def_rw("sum_stats2", &PwCoCoConfig::sum_stats2)
	    .def_rw("bim_file", &PwCoCoConfig::bim_file)
	    .def_rw("fam_file", &PwCoCoConfig::fam_file)
	    .def_rw("bed_file", &PwCoCoConfig::bed_file)
	    .def_rw("out", &PwCoCoConfig::out)
	    .def_rw("log", &PwCoCoConfig::log)
	    .def_rw("pve_file1", &PwCoCoConfig::pve_file1)
	    .def_rw("pve_file2", &PwCoCoConfig::pve_file2)
	    .def_rw("chr", &PwCoCoConfig::chr)
	    .def_rw("threads", &PwCoCoConfig::threads)
	    .def_rw("p_cutoff1", &PwCoCoConfig::p_cutoff1)
	    .def_rw("p_cutoff2", &PwCoCoConfig::p_cutoff2)
	    .def_rw("collinear", &PwCoCoConfig::collinear)
	    .def_rw("maf", &PwCoCoConfig::maf)
	    .def_rw("ld_window", &PwCoCoConfig::ld_window)
	    .def_rw("freq_threshold", &PwCoCoConfig::freq_threshold)
	    .def_rw("init_h4", &PwCoCoConfig::init_h4)
	    .def_rw("top_snp", &PwCoCoConfig::top_snp)
	    .def_rw("p1", &PwCoCoConfig::p1)
	    .def_rw("p2", &PwCoCoConfig::p2)
	    .def_rw("p3", &PwCoCoConfig::p3)
	    .def_rw("n1", &PwCoCoConfig::n1)
	    .def_rw("n2", &PwCoCoConfig::n2)
	    .def_rw("n1_case", &PwCoCoConfig::n1_case)
	    .def_rw("n2_case", &PwCoCoConfig::n2_case)
	    .def_rw("pve1", &PwCoCoConfig::pve1)
	    .def_rw("pve2", &PwCoCoConfig::pve2)
	    .def_rw("out_cond", &PwCoCoConfig::out_cond)
	    .def_rw("cond_ssize", &PwCoCoConfig::cond_ssize)
	    .def_rw("verbose", &PwCoCoConfig::verbose)
	    .def_rw("pairwise", &PwCoCoConfig::pairwise);

	nb::class_<phenotype>(m, "Phenotype")
	    .def(nb::init<>())
	    .def(nb::init<const std::string &, double, double, double, const std::string &>(),
	         nb::arg("name"),
	         nb::arg("n") = 0,
	         nb::arg("n_case") = 0,
	         nb::arg("pve") = -1,
	         nb::arg("pve_file") = "")
	    .def("read", &phenotype::read_phenofile, nb::arg("filename"))
	    .def_prop_ro("name", &phenotype::get_phenoname)
	    .def_prop_ro("snp_names", &phenotype::get_snp_names)
	    .def_prop_ro("failed", &phenotype::has_failed);

	nb::class_<reference>(m, "Reference")
	    .def(nb::init<>())
	    .def(nb::init<const std::string &, unsigned short>(), nb::arg("out"), nb::arg("chr") = 0)
	    .def("read_bim", &reference::read_bimfile, nb::arg("bimfile"))
	    .def("read_fam", &reference::read_famfile, nb::arg("famfile"))
	    .def("read_bed", &reference::read_bedfile, nb::arg("bedfile"))
	    .def_prop_ro("failed", &reference::has_failed)
	    .def_prop_ro("ready", &reference::is_ready);

	m.def(
	    "config_from_cli_args",
	    [](const std::vector<std::string> &args) {
		    PwCoCoConfig cfg;
		    std::vector<std::string> storage(args.begin(), args.end());
		    std::vector<char *> argv;
		    argv.reserve(storage.size() + 1);
		    argv.push_back(const_cast<char *>("pwcoco"));
		    for (auto &s : storage)
			    argv.push_back(s.data());
		    parse_cli(static_cast<int>(argv.size()), argv.data(), cfg);
		    return cfg;
	    },
	    nb::arg("args"),
	    "Build PwCoCoConfig from CLI-style flag arguments.");

	m.def(
	    "run_core",
	    [](PwCoCoConfig &cfg) {
		    setup_logger(cfg);
		    return run_pwcoco(cfg);
	    },
	    nb::arg("config"),
	    "Run PWCoCo analysis. Returns 0 on success.");

	m.def("setup_logger", &setup_logger, nb::arg("config"));
	m.def("initial_coloc", &initial_coloc, nb::arg("exposure"), nb::arg("outcome"), nb::arg("out"), nb::arg("p1"), nb::arg("p2"), nb::arg("p3"), nb::arg("init_h4"));
	m.def("pwcoco_sub", &pwcoco_sub, nb::arg("exposure"), nb::arg("outcome"), nb::arg("ref"), nb::arg("p_cutoff1"), nb::arg("p_cutoff2"), nb::arg("collinear"), nb::arg("ld_window"),
	          nb::arg("out"), nb::arg("top_snp"), nb::arg("freq_threshold"), nb::arg("cond_ssize"), nb::arg("out_cond"), nb::arg("p1"), nb::arg("p2"), nb::arg("p3"), nb::arg("verbose"));
}

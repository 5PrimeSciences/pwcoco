#include "pwcoco_run.h"

#include <chrono>
#include <filesystem>
#include <memory>
#include <omp.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <vector>

#ifdef _WIN32
#include <spdlog/sinks/wincolor_sink.h>
#else
#include <spdlog/sinks/ansicolor_sink.h>
#endif

#include "coloc.h"
#include "conditional.h"
#include "data.h"

using namespace std;
namespace fs = std::filesystem;

void setup_logger(const PwCoCoConfig &cfg)
{
	vector<spdlog::sink_ptr> sinks;
#ifdef _WIN32
	sinks.push_back(make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
#else
	sinks.push_back(make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
#endif
	try {
		sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(cfg.log + ".txt"));
		auto logger = make_shared<spdlog::logger>("pwcoco_log", begin(sinks), end(sinks));
		spdlog::set_default_logger(logger);
	} catch (const spdlog::spdlog_ex &ex) {
		cout << "Log setup failed: " << ex.what() << " - attempting to create default log \"pwcoco_log.txt\"." << endl;
		sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>("pwcoco_log.txt"));
		auto logger = make_shared<spdlog::logger>("pwcoco_log", begin(sinks), end(sinks));
		spdlog::set_default_logger(logger);
	}
	spdlog::flush_on(spdlog::level::info);
}

int run_pwcoco(const PwCoCoConfig &cfg)
{
	chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	if (cfg.bim_file.empty()) {
		spdlog::critical("No .bim file found; a .bim file MUST be supplied!");
		return 0;
	}
	if (cfg.fam_file.empty()) {
		spdlog::critical("No .fam file found; a .fam file MUST be supplied!");
		return 0;
	}

	const fs::path path(cfg.sum_stats1);
	error_code ec;
	bool data_folder = false;

	if (fs::is_directory(path, ec)) {
		data_folder = true;
		spdlog::info("Summary stats 1 is treated as a folder.");
		spdlog::warn("Passing folders as the summary statistics arguments is a beta feature and may be buggy. Please do not use this feature to inform on active analyses!");
	} else if (fs::is_regular_file(path, ec)) {
		data_folder = false;
		spdlog::info("Summary stats 1 is treated as a file.");
	}

	const fs::path path2(cfg.sum_stats2);
	error_code ec2;

	if (data_folder && fs::is_directory(path2, ec2)) {
		spdlog::info("Summary stats 2 is treated as a folder.");
	} else if (data_folder && !fs::is_directory(path2, ec2)) {
		spdlog::info("Summary stats 2 expected to be a folder but is not. Please fix this before continuing.");
		return 0;
	} else if (!data_folder && fs::is_regular_file(path2, ec2)) {
		spdlog::info("Summary stats 2 is treated as a file.");
	} else if (!data_folder && !fs::is_regular_file(path2, ec2)) {
		spdlog::info("Summary stats 2 expected to be a file but is not. Please fix this before continuing.");
		return 0;
	}

#if defined(_OPENMP)
	omp_set_dynamic(0);
	omp_set_num_threads(cfg.threads);
	spdlog::info("OpenMP will attempt to use up to {} threads.", cfg.threads);
#endif

	auto ref = make_unique<reference>(cfg.out, cfg.chr);
	double init_h4 = cfg.init_h4 / 100.0;

	if (data_folder) {
		using recursive_directory_iterator = fs::recursive_directory_iterator;
		for (const auto &dir_entry : recursive_directory_iterator(cfg.sum_stats1)) {
			string filename{dir_entry.path().filename().u8string()},
				path_to_file1{dir_entry.path().u8string()};

			for (const auto &dir_entry2 : recursive_directory_iterator(cfg.sum_stats2)) {
				string filename2{dir_entry2.path().filename().u8string()},
					path_to_file2{dir_entry2.path().u8string()};

				if (!cfg.pairwise && !path_to_file1.compare(path_to_file2)) {
					continue;
				}

				unique_ptr<phenotype> exposure(init_pheno(path_to_file1, filename + (cfg.pairwise ? "" : ".exp"), cfg.n1, cfg.n1_case, cfg.pve1, cfg.pve_file1));
				unique_ptr<phenotype> outcome(init_pheno(path_to_file2, filename2 + (cfg.pairwise ? "" : ".out"), cfg.n2, cfg.n2_case, cfg.pve2, cfg.pve_file2));
				if (exposure->has_failed() || outcome->has_failed()) {
					spdlog::error("Reading of either summary statistic files has failed; have these been moved or altered?");
					spdlog::error("File 1: {}", path_to_file1);
					spdlog::error("File 2: {}", path_to_file2);
					continue;
				}

				if (initial_coloc(exposure.get(), outcome.get(), cfg.out, cfg.p1, cfg.p2, cfg.p3, init_h4)) {
					continue;
				}

				if (!ref->is_ready()) {
					if (ref->read_bimfile(cfg.bim_file) == 0) {
						return 0;
					}
					ref->whole_bim();
					ref->sanitise_list();

					if (ref->read_famfile(cfg.fam_file) == 0) {
						return 0;
					}
					if (ref->read_bedfile(cfg.bed_file) == 0) {
						return 0;
					}
				} else {
					ref->reset_vectors();
				}

				ref->match_bim(exposure->get_snp_names(), outcome->get_snp_names(), true);
				ref->sanitise_list();

				if (cfg.maf > 0.0) {
					if (ref->filter_snp_maf(cfg.maf) == 0)
						return 0;
				}

				if (pwcoco_sub(exposure.get(), outcome.get(), ref.get(), cfg.p_cutoff1, cfg.p_cutoff2, cfg.collinear, cfg.ld_window, cfg.out, cfg.top_snp, cfg.freq_threshold, cfg.cond_ssize, cfg.out_cond, cfg.p1, cfg.p2, cfg.p3, cfg.verbose)) {
					continue;
				}
			}
		}
	} else {
		unique_ptr<phenotype> exposure(init_pheno(cfg.sum_stats1, fs::path(cfg.sum_stats1).filename().string(), cfg.n1, cfg.n1_case, cfg.pve1, cfg.pve_file1));
		unique_ptr<phenotype> outcome(init_pheno(cfg.sum_stats2, fs::path(cfg.sum_stats2).filename().string(), cfg.n2, cfg.n2_case, cfg.pve2, cfg.pve_file2));
		if (exposure->has_failed() || outcome->has_failed()) {
			spdlog::critical("Reading of either summary statistic files has failed; have these been moved or altered?");
			return 1;
		}

		if (initial_coloc(exposure.get(), outcome.get(), cfg.out, cfg.p1, cfg.p2, cfg.p3, init_h4)) {
			return 0;
		}

		if (ref->read_bimfile(cfg.bim_file) == 0) {
			return 0;
		}
		ref->match_bim(exposure->get_snp_names(), outcome->get_snp_names(), false);
		ref->sanitise_list();

		if (ref->read_famfile(cfg.fam_file) == 0) {
			return 0;
		}
		if (ref->read_bedfile(cfg.bed_file) == 0) {
			return 0;
		}
		if (cfg.maf > 0.0) {
			if (ref->filter_snp_maf(cfg.maf) == 0)
				return 0;
		}

		if (pwcoco_sub(exposure.get(), outcome.get(), ref.get(), cfg.p_cutoff1, cfg.p_cutoff2, cfg.collinear, cfg.ld_window, cfg.out, cfg.top_snp, cfg.freq_threshold, cfg.cond_ssize, cfg.out_cond, cfg.p1, cfg.p2, cfg.p3, cfg.verbose)) {
			return 0;
		}
	}

	chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	spdlog::info("Analysis finished. Computational time: {} secs", (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0);
	return 0;
}

int initial_coloc(phenotype *exposure, phenotype *outcome, string out, double p1, double p2, double p3, double init_h4)
{
	mdata *matched = new mdata(exposure, outcome);
	coloc_analysis *initial = new coloc_analysis(matched, out, p1, p2, p3);

	if (initial->num_snps() == 0) {
		spdlog::info("Stopping algorthim as no SNPs included in initial colocalisation analysis.");
		delete initial;
		return 1;
	}

	initial->init_coloc(exposure->get_phenoname(), outcome->get_phenoname());

	if (initial->pp_abf[H4] > init_h4) {
		spdlog::info("Stopping algorthim as H4 for initial colocalisation analysis is already at or above threshold ({}%).", init_h4 * 100);
		delete initial;
		return 1;
	}

	delete initial;
	return 0;
}

int pwcoco_sub(phenotype *exposure, phenotype *outcome, reference *ref, double p_cutoff1, double p_cutoff2, double collinear, double ld_window, string out, double top_snp,
	double freq_threshold, double cond_ssize, bool out_cond, double p1, double p2, double p3, bool verbose)
{
	conditional_dat *exp_cdat = new conditional_dat();
	conditional_dat *out_cdat = new conditional_dat();

	cond_analysis *exp_analysis = new cond_analysis(p_cutoff1, collinear, ld_window, out, top_snp, freq_threshold, exposure->get_phenoname(), cond_ssize, verbose);
	exp_analysis->init_conditional(exposure, ref);
	exp_analysis->find_independent_snps(exp_cdat, ref);

	cond_analysis *out_analysis = new cond_analysis(p_cutoff2, collinear, ld_window, out, top_snp, freq_threshold, outcome->get_phenoname(), cond_ssize, verbose);
	out_analysis->init_conditional(outcome, ref);
	out_analysis->find_independent_snps(out_cdat, ref);

	if (exp_analysis->get_num_ind() == 0 && out_analysis->get_num_ind() == 0) {
		spdlog::warn("Both conditional analyses failed to run or find any conditionally independednt signals, and so no colocalisation will be run.");
		delete exp_analysis;
		delete out_analysis;
		delete exp_cdat;
		delete out_cdat;
		return 1;
	}

	spdlog::info("There are {} selected SNPs in the exposure dataset and {} in the outcome dataset.", exp_analysis->get_num_ind(), out_analysis->get_num_ind());
	spdlog::info("Performing {} conditional and colocalisation analyses.", (exp_analysis->get_num_ind() == 0 ? 1 : exp_analysis->get_num_ind()) * (out_analysis->get_num_ind() == 0 ? 1 : out_analysis->get_num_ind()));

	for (int i = 0; i < exp_analysis->get_num_ind() + 1; i++) {
		string exp_snp_name = "", out_snp_name = "";

		if (i < exp_analysis->get_num_ind()) {
			conditional_dat *par_exp_cdat = new conditional_dat(*exp_cdat);

			exp_analysis->pw_conditional(exp_analysis->get_num_ind() > 1 ? i : -1, out_cond, par_exp_cdat, ref);
			exp_snp_name = exp_analysis->get_ind_snp_name(i);

			delete par_exp_cdat;
		} else if (i >= exp_analysis->get_num_ind()) {
			exp_snp_name = "unconditioned";
		}

		for (int j = 0; j < out_analysis->get_num_ind() + 1; j++) {
			if (j < out_analysis->get_num_ind()) {
				conditional_dat *par_out_cdat = new conditional_dat(*out_cdat);

				out_analysis->pw_conditional(out_analysis->get_num_ind() > 1 ? j : -1, out_cond, par_out_cdat, ref);
				out_snp_name = out_analysis->get_ind_snp_name(j);

				delete par_out_cdat;
			} else if (j >= out_analysis->get_num_ind()) {
				out_snp_name = "unconditioned";
			}

			if (exp_snp_name == "unconditioned" && out_snp_name == "unconditioned")
				continue;

			mdata *matched_conditional;
			if (exp_snp_name == "unconditioned")
				matched_conditional = new mdata(out_analysis, exposure);
			else if (out_snp_name == "unconditioned")
				matched_conditional = new mdata(exp_analysis, outcome);
			else
				matched_conditional = new mdata(exp_analysis, out_analysis);

			coloc_analysis *conditional_coloc = new coloc_analysis(matched_conditional, out, p1, p2, p3);
			conditional_coloc->init_coloc(exp_snp_name, out_snp_name, exp_analysis->get_cond_name(), out_analysis->get_cond_name());

			delete matched_conditional;
			delete conditional_coloc;
		}
	}

	delete exp_analysis;
	delete out_analysis;
	delete exp_cdat;
	delete out_cdat;
	return 0;
}

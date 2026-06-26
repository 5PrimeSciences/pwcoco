#pragma once

#include <string>

struct PwCoCoConfig {
	// Required
	std::string bfile;
	std::string sum_stats1;
	std::string sum_stats2;

	// Derived from bfile
	std::string bim_file;
	std::string fam_file;
	std::string bed_file;

	// Optional — defaults match CLI
	std::string out = "pwcoco_out";
	std::string log = "pwcoco_log";
	std::string pve_file1;
	std::string pve_file2;
	unsigned short chr = 0;
	int threads = 8;
	double p_cutoff1 = 5e-8, p_cutoff2 = 5e-8;
	double collinear = 0.9, maf = 0.1, ld_window = 1.0e7;
	double freq_threshold = 0.2, init_h4 = 80, top_snp = 1e10;
	double p1 = 1e-4, p2 = 1e-4, p3 = 1e-5;
	double n1 = 0.0, n2 = 0.0, n1_case = 0.0, n2_case = 0.0;
	double pve1 = -1.0, pve2 = -1.0;
	bool out_cond = false, cond_ssize = false, verbose = false, pairwise = false;
};

void setup_logger(const PwCoCoConfig &cfg);
int run_pwcoco(const PwCoCoConfig &cfg);

class phenotype;
class reference;

int initial_coloc(phenotype *exposure, phenotype *outcome, std::string out, double p1, double p2, double p3, double init_h4);
int pwcoco_sub(phenotype *exposure, phenotype *outcome, reference *ref, double p_cutoff1, double p_cutoff2, double collinear, double ld_window, std::string out, double top_snp,
	double freq_threshold, double cond_ssize, bool out_cond, double p1, double p2, double p3, bool verbose);

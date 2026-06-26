#include "options.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

using namespace std;

namespace {
constexpr const char *PWCOCO_VERSION = "1.2.0";
}

const char *pwcoco_version_string() {
    return PWCOCO_VERSION;
}

void print_version() {
    cout << "pwcoco " << PWCOCO_VERSION << endl;
}

void print_help() {
    spdlog::info("Usage: pwcoco [options]");
    spdlog::info("");
    spdlog::info("Please remember to periodically check for new updates using git pull!");
    spdlog::info("More information about these flags and how to use PWCoCo can be found on GitHub:");
    spdlog::info("https://github.com/jwr-git/pwcoco");
    spdlog::info("");
    spdlog::info("Required flags:");
    spdlog::info("	--bfile                    Location to the reference data in Plink (bed/bim/fam) "
                 "format.");
    spdlog::info("	                           Do not include the file ending name.");
    spdlog::info("	                           Each file requires the same name and be in the same "
                 "directory.");
    spdlog::info("");
    spdlog::info("	--sum_stats1, sum_stats2   Location to the first file or folder containing summary "
                 "statistics.");
    spdlog::info("");
    spdlog::info("Optional flags:");
    spdlog::info("	--pve_file1, pve_file2     Files from which to calculate the phenotypic variance for "
                 "the summary statistics.");
    spdlog::info("");
    spdlog::info("	--pve1, pve2               If the phenotypic variance has already been calculated, you "
                 "can specify it directly here.");
    spdlog::info("");
    spdlog::info("	--log                      Specify log name; default is 'pwcoco_log.txt' and will save "
                 "in the current directory.");
    spdlog::info("");
    spdlog::info("	--out                      Prefix for all output files for this analysis.");
    spdlog::info("");
    spdlog::info("	--p_cutoff                 P value cutoff for SNPs to be selected by the stepwise "
                 "selection process; default is 5e-8.");
    spdlog::info("	                           Alternatively, --p_cutoff1 and --p_cutoff2 can specify "
                 "dataset-specific P value cutoffs.");
    spdlog::info("");
    spdlog::info("	--chr                      Limit the reference data to reading only this chromosome.");
    spdlog::info("");
    spdlog::info("	--top_snp                  Maximum number of SNPs that can be selected by the stepwise "
                 "selection process; default 1e10.");
    spdlog::info("");
    spdlog::info("	--ld_window                Distance in kb that is assumed for SNPs to be in total "
                 "linkage equilibrium; default is 1e7.");
    spdlog::info("");
    spdlog::info(
        "	--collinear                Threshold that determines if SNPs are collinear; default is 0.9.");
    spdlog::info("");
    spdlog::info("	--maf                      Filters SNPs from the reference dataset according to this "
                 "threshold; default is 0.1.");
    spdlog::info("");
    spdlog::info("	--freq_threshold           Exclude SNPs with an allele frequency difference between "
                 "the sum stats and the reference data");
    spdlog::info("	                           greater than this threshold; default is 0.2.");
    spdlog::info("");
    spdlog::info("	--init_h4                  Treshold to termine the program early if the initial "
                 "colocalisation H4 is higher than this; default 80.");
    spdlog::info("");
    spdlog::info("	--out_cond                 Flag to turn on extra files to be output corresponding to "
                 "the conditioned data.");
    spdlog::info("");
    spdlog::info("	--coloc_pp                 Specify the three prior probabilities; default 1e-4, 1e-4 "
                 "and 1e-5.");
    spdlog::info("");
    spdlog::info("	--n1, n2                   Specify the sample size for summary statistics.");
    spdlog::info("");
    spdlog::info("	--n1_case, n2_case         Specific the number of cases for summary statistics.");
    spdlog::info("");
    spdlog::info("	--threads                  Number of threads available for OpenMP multi-threaded "
                 "functions; default is 8.");
    spdlog::info("");
    spdlog::info("	--verbose                  Output extra files and messages for debugging purposes.");
    spdlog::info("");
    spdlog::info("	--pairwise                 If using folders as input, will run PWCoCo on the pairwise "
                 "combination of files.");
    spdlog::info("	                           Without this flag, the files must match based on name.");
}

bool cli_requests_early_exit(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        string opt = argv[i];
        if (opt == "--help" || opt == "-h") {
            print_help();
            return true;
        }
        if (opt == "--version") {
            print_version();
            return true;
        }
    }
    return false;
}

void print_banner() {
    spdlog::info(" ****************************************");
    spdlog::info(" *______ _    _ _____       _____       *");
    spdlog::info(" *| ___ \\ |  | /  __ \\     /  __ \\      *");
    spdlog::info(" *| |_/ / |  | | /  \\/ ___ | /  \\/ ___  *");
    spdlog::info(" *|  __/| |/\\| | |    / _ \\| |    / _ \\ *");
    spdlog::info(" *| |   \\  /\\  / \\__/\\ (_) | \\__/\\ (_) |*");
    spdlog::info(" *\\_|    \\/  \\/ \\____/\\___/ \\____/\\___/ *");
    spdlog::info(" *                                      *");
    spdlog::info(" ****************************************");
    spdlog::info("Version: {}", PWCOCO_VERSION);
    spdlog::info("!! Remember to periodically 'git pull' in your PWCoCo directory to obtain the latest updates and bug "
                 "fixes !!");
}

int parse_cli(int argc, char *argv[], PwCoCoConfig &cfg) {
    for (int i = 1; i < argc; i++) {
        string opt = argv[i];

        if (opt == "--bfile") {
            cfg.bfile = argv[++i];
            if (cfg.bfile.size() >= 6 && cfg.bfile.substr(cfg.bfile.length() - 6) == ".bfile")
                cfg.bfile = cfg.bfile.substr(0, cfg.bfile.length() - 6);
            cfg.bim_file = cfg.bfile + ".bim";
            cfg.fam_file = cfg.bfile + ".fam";
            cfg.bed_file = cfg.bfile + ".bed";
            spdlog::info("Using {} as reference files.", cfg.bfile);
        } else if (opt == "--phen1_file" || opt == "--sum_stats1") {
            cfg.sum_stats1 = argv[++i];
            spdlog::info("Using {} summary statistic 1 files or folders.", cfg.sum_stats1);
        } else if (opt == "--phen2_file" || opt == "--sum_stats2") {
            cfg.sum_stats2 = argv[++i];
            spdlog::info("Using {} summary statistic 2 files or folders.", cfg.sum_stats2);
        } else if (opt == "--pve_file1") {
            cfg.pve_file1 = argv[++i];
            spdlog::info("Calculating phenotypic variance from {}.", cfg.pve_file1);
        } else if (opt == "--pve_file2") {
            cfg.pve_file2 = argv[++i];
            spdlog::info("Calculating phenotypic variance from {}.", cfg.pve_file2);
        } else if (opt == "--n1") {
            cfg.n1 = stod(argv[++i]);
            if (cfg.n1 <= 0)
                cfg.n1 = 0;
            spdlog::info("--n1 {}.", cfg.n1);
        } else if (opt == "--n2") {
            cfg.n2 = stod(argv[++i]);
            if (cfg.n2 <= 0)
                cfg.n2 = 0;
            spdlog::info("--n2 {}.", cfg.n2);
        } else if (opt == "--n1_case") {
            cfg.n1_case = stod(argv[++i]);
            if (cfg.n1_case <= 0)
                cfg.n1_case = 0;
            spdlog::info("--n1_case {}.", cfg.n1_case);
        } else if (opt == "--n2_case") {
            cfg.n2_case = stod(argv[++i]);
            if (cfg.n2_case <= 0)
                cfg.n2_case = 0;
            spdlog::info("--n2_case {}.", cfg.n2_case);
        } else if (opt == "--log") {
            cfg.log = argv[++i];
            spdlog::info("--log {}.", cfg.log);
        } else if (opt == "--p_cutoff") {
            cfg.p_cutoff1 = cfg.p_cutoff2 = stod(argv[++i]);
            spdlog::info("--p_cutoff {}.", cfg.p_cutoff1);
        } else if (opt == "--p_cutoff1") {
            cfg.p_cutoff1 = stod(argv[++i]);
            spdlog::info("--p_cutoff1 {}.", cfg.p_cutoff1);
        } else if (opt == "--p_cutoff2") {
            cfg.p_cutoff2 = stod(argv[++i]);
            spdlog::info("--p_cutoff2 {}.", cfg.p_cutoff2);
        } else if (opt == "--out") {
            cfg.out = argv[++i];
            spdlog::info("--out {}.", cfg.out);
        } else if (opt == "--chr") {
            cfg.chr = (unsigned short)stoi(argv[++i]);
            if (cfg.chr < 1 || cfg.chr > 23)
                cfg.chr = (cfg.chr < 1 ? 1 : cfg.chr > 23 ? 23 : cfg.chr);
            spdlog::info("--chr {}.", cfg.chr);
        } else if (opt == "--top_snp") {
            cfg.top_snp = stod(argv[++i]);
            if (cfg.top_snp < 1 || cfg.top_snp > 10000)
                cfg.top_snp = (cfg.top_snp <= 1 ? 1 : cfg.top_snp >= 10000 ? 10000 : cfg.top_snp);
            spdlog::info("--top_snp {}.", cfg.top_snp);
        } else if (opt == "--ld_window") {
            cfg.ld_window = stoi(argv[++i]);
            if (cfg.ld_window > 10000)
                cfg.ld_window = 10000;
            cfg.ld_window *= 1000;
            spdlog::info("--ld_window {}.", cfg.ld_window);
        } else if (opt == "--collinear") {
            cfg.collinear = stod(argv[++i]);
            if (cfg.collinear < 0.01 || cfg.collinear > 0.99)
                cfg.collinear = (cfg.collinear <= 0.01 ? 0.01 : cfg.collinear >= 0.99 ? 0.99 : cfg.collinear);
            spdlog::info("--collinear {}.", cfg.collinear);
        } else if (opt == "--maf") {
            cfg.maf = stod(argv[++i]);
            if (cfg.maf < 0.0 || cfg.maf > 0.5)
                cfg.maf = (cfg.maf < 0.0 ? 0.0 : cfg.maf > 0.5 ? 0.5 : cfg.maf);
            spdlog::info("--maf {}.", cfg.maf);
        } else if (opt == "--freq_threshold") {
            cfg.freq_threshold = stod(argv[++i]);
            if (cfg.freq_threshold < 0.0 || cfg.freq_threshold > 1.0)
                cfg.freq_threshold = (cfg.freq_threshold < 0.0   ? 0.0
                                      : cfg.freq_threshold > 1.0 ? 1.0
                                                                 : cfg.freq_threshold);
            spdlog::info("--freq_threshold {}.", cfg.freq_threshold);
        } else if (opt == "--init_h4") {
            cfg.init_h4 = stod(argv[++i]);
            if (cfg.init_h4 < 0.0 || cfg.init_h4 > 100.0)
                cfg.init_h4 = (cfg.init_h4 < 0.0 ? 0.0 : cfg.init_h4 > 100.0 ? 100.0 : cfg.init_h4);
            spdlog::info("--init_h4 {}.", cfg.init_h4);
        } else if (opt == "--out_cond") {
            cfg.out_cond = true;
            spdlog::info("--out_cond.");
        } else if (opt == "--coloc_pp") {
            cfg.p1 = stod(argv[++i]);
            cfg.p2 = stod(argv[++i]);
            cfg.p3 = stod(argv[++i]);
            cfg.p1 = (cfg.p1 > 1.0 ? 1.0 : cfg.p1 < 1e-50 ? 1e-50 : cfg.p1);
            cfg.p2 = (cfg.p2 > 1.0 ? 1.0 : cfg.p2 < 1e-50 ? 1e-50 : cfg.p2);
            cfg.p3 = (cfg.p3 > 1.0 ? 1.0 : cfg.p3 < 1e-50 ? 1e-50 : cfg.p3);
            spdlog::info("--coloc_pp p1 {} p2 {} p3 {}.", cfg.p1, cfg.p2, cfg.p3);
        } else if (opt == "--cond_ssize") {
            cfg.cond_ssize = true;
            spdlog::info("--cond_ssize.");
        } else if (opt == "--threads") {
            cfg.threads = stoi(argv[++i]);
            spdlog::info("--threads {}.", cfg.threads);
        } else if (opt == "--verbose") {
            cfg.verbose = true;
            cfg.out_cond = true;
            spdlog::info("--verbose.");
        } else if (opt == "--pve1") {
            cfg.pve1 = stod(argv[++i]);
            spdlog::info("--pve1 {}.", cfg.pve1);
        } else if (opt == "--pve2") {
            cfg.pve2 = stod(argv[++i]);
            spdlog::info("--pve2 {}.", cfg.pve2);
        } else if (opt == "--pairwise") {
            cfg.pairwise = true;
            spdlog::info("--pairwise.");
        }
    }
    return 0;
}

#include "options.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

namespace {

class Argv {
public:
    explicit Argv(std::initializer_list<std::string> args) {
        storage_.emplace_back("pwcoco");
        storage_.insert(storage_.end(), args.begin(), args.end());
        for (auto &arg : storage_) {
            ptrs_.push_back(arg.data());
        }
    }

    int argc() const {
        return static_cast<int>(ptrs_.size());
    }

    char **argv() {
        return ptrs_.data();
    }

private:
    std::vector<std::string> storage_;
    std::vector<char *> ptrs_;
};

std::string capture_stdout(void (*fn)()) {
    std::ostringstream buffer;
    auto *previous = std::cout.rdbuf(buffer.rdbuf());
    fn();
    std::cout.rdbuf(previous);
    return buffer.str();
}

} // namespace

TEST(PwCoCoVersion, VersionString) {
    EXPECT_STREQ(pwcoco_version_string(), "1.2.0");
}

TEST(PwCoCoVersion, PrintVersion) {
    EXPECT_EQ(capture_stdout(print_version), "pwcoco 1.2.0\n");
}

TEST(PwCoCoCliEarlyExit, HelpAndVersionRequestExit) {
    Argv help_argv{"--help"};
    Argv h_argv{"-h"};
    Argv version_argv{"--version"};
    Argv run_argv{"--bfile", "ref", "--sum_stats1", "a.txt", "--sum_stats2", "b.txt"};

    EXPECT_TRUE(cli_requests_early_exit(help_argv.argc(), help_argv.argv()));
    EXPECT_TRUE(cli_requests_early_exit(h_argv.argc(), h_argv.argv()));
    EXPECT_TRUE(cli_requests_early_exit(version_argv.argc(), version_argv.argv()));
    EXPECT_FALSE(cli_requests_early_exit(run_argv.argc(), run_argv.argv()));
}

TEST(PwCoCoParseCli, BfileSuffixAndDerivedPaths) {
    PwCoCoConfig cfg;
    Argv argv{"--bfile", "panel.bfile", "--sum_stats1", "exp.txt", "--sum_stats2", "out.txt"};

    parse_cli(argv.argc(), argv.argv(), cfg);

    EXPECT_EQ(cfg.bfile, "panel");
    EXPECT_EQ(cfg.bim_file, "panel.bim");
    EXPECT_EQ(cfg.fam_file, "panel.fam");
    EXPECT_EQ(cfg.bed_file, "panel.bed");
    EXPECT_EQ(cfg.sum_stats1, "exp.txt");
    EXPECT_EQ(cfg.sum_stats2, "out.txt");
}

TEST(PwCoCoParseCli, LdWindowConvertsKbToBp) {
    PwCoCoConfig cfg;
    Argv argv{"--bfile", "ref", "--sum_stats1", "a.txt", "--sum_stats2", "b.txt", "--ld_window", "500"};

    parse_cli(argv.argc(), argv.argv(), cfg);

    EXPECT_DOUBLE_EQ(cfg.ld_window, 500000.0);
}

TEST(PwCoCoParseCli, VerboseEnablesOutCond) {
    PwCoCoConfig cfg;
    Argv argv{"--bfile", "ref", "--sum_stats1", "a.txt", "--sum_stats2", "b.txt", "--verbose"};

    parse_cli(argv.argc(), argv.argv(), cfg);

    EXPECT_TRUE(cfg.verbose);
    EXPECT_TRUE(cfg.out_cond);
}

TEST(PwCoCoParseCli, SharedPCutoffAppliesToBothDatasets) {
    PwCoCoConfig cfg;
    Argv argv{"--bfile", "ref", "--sum_stats1", "a.txt", "--sum_stats2", "b.txt", "--p_cutoff", "1e-6"};

    parse_cli(argv.argc(), argv.argv(), cfg);

    EXPECT_DOUBLE_EQ(cfg.p_cutoff1, 1e-6);
    EXPECT_DOUBLE_EQ(cfg.p_cutoff2, 1e-6);
}

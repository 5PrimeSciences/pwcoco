#include "options.h"

#include "pwcoco_run.h"

int main(int argc, char *argv[]) {
    if (cli_requests_early_exit(argc, argv))
        return 0;

    print_banner();

    PwCoCoConfig cfg;
    parse_cli(argc, argv, cfg);
    setup_logger(cfg);
    return run_pwcoco(cfg);
}

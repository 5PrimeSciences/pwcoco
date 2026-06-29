#pragma once

#include "pwcoco_run.h"

int parse_cli(int argc, char *argv[], PwCoCoConfig &cfg);
void print_banner();
void print_help();
void print_version();
const char *pwcoco_version_string();
bool cli_requests_early_exit(int argc, char *argv[]);

//
// Hobo File System
//
// (C) Copyright 2019 Tony Mason
//
// Licensed under the Aranae project license
// https://github.com/fsgeek/Araneae/blob/master/LICENSE
//

#include <hobofs.h>

static const char *hobofs = "HoboFS";

int main(int argc, char **argv)
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_cmdline_opts fuse_opts;
    int code = 1;
    int se_signal_handlers_installed = 0;
    int se_session_created = 0;
    int se_mounted = 0;
    int opt_args_created = 0;
    struct fuse_loop_config fl_config;
    struct fuse_session *session = NULL;
    const struct fuse_lowlevel_ops *hobo_ops;

    printf("%s: starting...\n", hobofs);
    hobo_ops = hobo_getops();

    memset(&fuse_opts, 0, sizeof(fuse_opts));

    // handle the command line options
    while (0 == fuse_parse_cmdline(&args, &fuse_opts)) {
        opt_args_created = 1;

        if (fuse_opts.show_help || (NULL == fuse_opts.mountpoint)) {
            printf("usage: %s [options] <mount point>\n", argv[0]);
            if (fuse_opts.show_help) {
                fuse_cmdline_help();
                fuse_lowlevel_help();
                code = 0;
            }
            else {
                printf("       %s --help\n", argv[0]);
                code = 1;
            }
            break;
        }

        if (fuse_opts.show_version) {
            printf("FUSE library version %s\n", fuse_pkgversion());
            fuse_lowlevel_version();
            code = 0;
            break;
        }

        // start a new session
        session = fuse_session_new(&args, hobo_ops, sizeof(struct fuse_lowlevel_ops), NULL);

        if (NULL == session) {
            fprintf(stderr, "%s: fuse_session_new failed\n", hobofs);
            code = 1;
            break;
        }
        se_session_created = 1;

        if (fuse_set_signal_handlers(session)) {
            fprintf(stderr, "%s: fuse_set_signal_handlers failed\n", hobofs);
            code = 1;
            break;
        }
        se_signal_handlers_installed = 1;

        if (fuse_session_mount(session, fuse_opts.mountpoint)) {
            fprintf(stderr, "%s: fuse_session_mount failed\n", hobofs);
            code = 1;
            break;
        }
        se_mounted = 1;

        fuse_daemonize(fuse_opts.foreground);

        if (fuse_opts.singlethread) {
            code = fuse_session_loop(session);
           fprintf(stderr, "%s: fuse_session_loop returned %d\n", hobofs, code);
        }
        else {
            memset(&fl_config, 0, sizeof(fl_config));
            fl_config.clone_fd = fuse_opts.clone_fd;
            fl_config.max_idle_threads = 10; // default - should this be based on the core count?
            code = fuse_session_loop_mt(session, &fl_config);
           fprintf(stderr, "%s: fuse_session_loop_mt returned %d\n", hobofs, code);
        }

        // done with loop
        break;
    } 

    if (!opt_args_created) {

    }

    // cleanup
    if (se_mounted) {
        fuse_session_unmount(session);
        se_mounted = 0;
    }

    if (se_signal_handlers_installed) {
        fuse_remove_signal_handlers(session);
        se_signal_handlers_installed = 0;
    }

    if (se_session_created) {
        fuse_session_destroy(session);
        se_session_created = 0;
    }

    if (fuse_opts.mountpoint) {
        free(fuse_opts.mountpoint);
        fuse_opts.mountpoint = NULL;
    }

    if (opt_args_created) {
        fuse_opt_free_args(&args);
        opt_args_created = 0;
    }

    return code;

}
#include "clash.h"

LogType LOG_LEVEL = INFO;

int main(int argc, char* argv[]) {
    Arguments args(INTRO_TEXT);
    args.RegisterBool("help", "Print help message");
    args.RegisterAlias('h', "help");

    args.RegisterBool("verbose", "Show debug logs");
    args.RegisterAlias('v', "verbose");

    args.RegisterBool("quiet", "Hide all logs except errors");
    args.RegisterAlias('q', "quiet");

    args.RegisterString("command", "Run command");
    args.RegisterAlias('c', "command");

    try {
        args.Parse(argc, argv);
    } catch (ArgumentsException& err) {
        error("%s", err.what());
        return 2;
    }

    if (args.GetBool("quiet")) {
        LOG_LEVEL = ERROR;
    } else if (args.GetBool("verbose")) {
        LOG_LEVEL = DEBUG;
    }

    if (args.GetBool("help")) {
        printf("%s\n", args.GetHelpText().c_str());
        return EXIT_SUCCESS;
    }

    Shell shell(argc, argv);

    string command = args.GetString("command");
    if (!command.empty()) {
        if (!shell.ParseString(command)) {
            return -1;
        }
        return shell.RunJobsAndWait() ? EXIT_SUCCESS : -1;
    }

    vector<string> unnamed_args = args.GetUnnamed();
    if (unnamed_args.size() > 0) {
        const string& file_path = unnamed_args[0];
        if (!shell.ParseFile(file_path)) {
            return 127;
        }
        return shell.RunJobsAndWait() ? EXIT_SUCCESS : -1;
    }

    return shell.StartRepl();
}

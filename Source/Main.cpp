#include "Help.h"
#include "Build.h"
#include "Extract.h"
#include "Init.h"
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

static void printBanner()
{
    for (const auto& [name, text] : map::banner)
        printf ("%s\n", text.toRawUTF8());
}

static void printBannerAndHelp()
{
    static const auto newlineText { juce::String::charToString (Chars::newline) };
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    printBanner();
    printf ("%s", newlineText.toRawUTF8());
    printHelp (BinaryData::getString (files::ggwpHelp));
}

static bool isTerminalOutput() noexcept
{
#ifdef _WIN32
    return _isatty (_fileno (stdout)) != 0;
#else
    return isatty (fileno (stdout)) != 0;
#endif
}

int main (int argc, char* argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP (CP_UTF8);
#endif

    if (isTerminalOutput())
    {
        juce::MemoryOutputStream erase;
        jam::TerminalEncoder::writeEraseScreen (erase);
        fwrite (erase.getData(), sizeof (char), erase.getDataSize(), stdout);
        fflush (stdout);
    }

    juce::ConsoleApplication app;

    static const juce::String helpOption { "--help|-h" };
    static const juce::String versionOption { "--version|-v" };
    app.addDefaultCommand ({ helpOption, helpOption, text::Commands::helpShortDescription, text::Commands::helpLongDescription,
                             [] (const juce::ArgumentList&) { printBannerAndHelp(); } });
    app.addVersionCommand (versionOption,
                          juce::String (ProjectInfo::projectName) + " version " + ProjectInfo::versionString);

    app.addCommand ({ getBuildOption(), text::Commands::buildArgumentDescription, text::Commands::buildShortDescription,
                      text::Commands::buildLongDescription, runBuildCommand });
    app.addCommand ({ getExtractOption(), text::Commands::extractArgumentDescription, text::Commands::extractShortDescription,
                      text::Commands::extractLongDescription, runExtractCommand });
    app.addCommand ({ getInitOption(), text::Commands::initArgumentDescription, text::Commands::initShortDescription,
                      text::Commands::initLongDescription, runInitCommand });

    return app.findAndRunCommand (argc, argv);
}

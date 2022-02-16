#ifndef CLI_ARGUMENT_PROCESSOR_H
#define CLI_ARGUMENT_PROCESSOR_H

#include "cli_usage_menu_printable.h"
#include "cloneable.h"
#include "custom_locale.h"
#include "dictionary.h"
#include "loggable.h"
#include "reflective.h"
#include "string_utilities.h"
#include "toggleable_stream.h"
#include <iomanip>
#include <iostream>
#include <iterator>

namespace utilities
{

/**
 * This class facilitates processing and storage of command-line arguments
 */
class CLI_ArgumentProcessor final
: public attributes::interfaces::CLI_UsageMenuPrintable,
  public attributes::interfaces::Cloneable<CLI_ArgumentProcessor>,
  public attributes::concrete::Loggable<std::string, ToggleableStream>,
  private virtual attributes::abstract::Reflective
{
public:

    /**
     * Typedef declarations
     */
    typedef std::pair<std::string, std::string> tTokenPair;
    typedef std::map<std::string, std::vector<std::string>> tTokenVectorPair;

    /**
     * Constructor
     * @param toggleableStreamMap a map of name-ToggleableStream object pairs
     */
    CLI_ArgumentProcessor(std::map<std::string, ToggleableStream *> &toggleableStreamMap)
    : Loggable<std::string, ToggleableStream>(&toggleableStreamMap),
      m_bRemoveProcessedVariablesFromRegistry(false)
    {

    }

    /**
     * Copy constructor
     */
    CLI_ArgumentProcessor(const CLI_ArgumentProcessor &cliArgumentProcessor)
    {
        operator = (cliArgumentProcessor);
    }

    /**
     * Move constructor
     */
    CLI_ArgumentProcessor(CLI_ArgumentProcessor &&cliArgumentProcessor)
    {
        operator = (std::move(cliArgumentProcessor));
    }

    /**
     * Destructor
     */
    virtual ~CLI_ArgumentProcessor(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    CLI_ArgumentProcessor &operator = (const CLI_ArgumentProcessor &cliArgumentProcessor)
    {
        if (&cliArgumentProcessor != this)
        {
            Loggable<std::string, ToggleableStream>::operator = (cliArgumentProcessor);

            m_bRemoveProcessedVariablesFromRegistry = cliArgumentProcessor.
                                                      m_bRemoveProcessedVariablesFromRegistry;
            m_optionsUsageMap = cliArgumentProcessor.m_optionsUsageMap;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    CLI_ArgumentProcessor &operator = (CLI_ArgumentProcessor &&cliArgumentProcessor)
    {
        if (&cliArgumentProcessor != this)
        {
            Loggable<std::string, ToggleableStream>::operator = (std::move(cliArgumentProcessor));

            m_bRemoveProcessedVariablesFromRegistry = std::move(cliArgumentProcessor.
                                                                m_bRemoveProcessedVariablesFromRegistry);
            m_optionsUsageMap = std::move(cliArgumentProcessor.m_optionsUsageMap);
        }

        return *this;
    }

    /**
     * Add an option with corresponding arguments and description
     * @param option         a string of the form "-option" that specifies a program option
     * @param argumentFormat a string that defines the required format for accompanying arguments, if any,
     *                       supplied along with the option. The argument format specification is tokenized by
     *                       whitespace, and is interpreted as follows: each token not enclosed within brackets
     *                       is counted as a required argument for the given option; each token enclosed within
     *                       brackets is counted as an optional argument. If an ellipses (...) is encountered
     *                       between two bracketed tokens, such as [token_1] ... [token_n], the option will be
     *                       considered to have an indeterminate maximum number of arguments (although the
     *                       option may still have a required minimum number of arguments). Tokens not enclosed
     *                       within brackets that appear after (to the right of) tokens enclosed within
     *                       brackets are ignored altogether.
     * @param description    a short summary of the option and any corresponding arguments
     */
    inline virtual void addOption(const std::string &option, const std::string &argumentFormat,
                                  const std::string &description) final
    {
        m_optionsUsageMap[option] = tTokenPair(argumentFormat, description);
    }

private:

    /**
     * Verify that the argument format string for the supplied option is valid; also verify that the user
     * supplied a valid number of arguments for this option
     */
    virtual bool checkOptionArguments(const std::string &option, size_t numArgs) const final
    {
        bool bSuccess = false;
        auto &&itOption = m_optionsUsageMap.find(option);
        if (itOption != m_optionsUsageMap.cend())
        {
            bSuccess = true;
            long numOptionalArgs = 0;
            size_t numRequiredArgs = 0;
            bool bBracketsNotBalanced = false, bRequiredArgumentsProcessed = false;
            Tokenizer tokenizer(" ", "\t");

            const std::string &argumentFormat = itOption->second.first;
            auto &&itPreviousClosedBracket = argumentFormat.cend();
            auto &&itArgumentFormat = argumentFormat.cbegin();
            while (itArgumentFormat != argumentFormat.cend())
            {
                auto &&itOpenBracket = std::find(itArgumentFormat, argumentFormat.cend(), '[');
                if (itPreviousClosedBracket != argumentFormat.cend())
                {
                    // check for an ellipses between brackets
                    if (itOpenBracket != argumentFormat.cend())
                    {
                        std::string string(itArgumentFormat, itOpenBracket);
                        string.erase(std::remove_if(string.begin(), string.end(), isspace), string.end());
                        if (string.find("...") != std::string::npos) // does resulting string contain ellipses?
                            numOptionalArgs = -1;
                    }
                }

                if (!bRequiredArgumentsProcessed)
                {
                    std::string requiredArguments(argumentFormat.cbegin(), itOpenBracket);
                    auto &&tokens = tokenizer.parse<std::string>(requiredArguments);
                    numRequiredArgs = tokens.size();
                    bRequiredArgumentsProcessed = true;
                }

                if (itOpenBracket != argumentFormat.cend()) // had better find a closing bracket...
                {
                    auto &&itClosedBracket = std::find(itOpenBracket, argumentFormat.cend(), ']');
                    itOpenBracket = std::find(itOpenBracket + 1, argumentFormat.cend(), '[');
                    if (itClosedBracket != argumentFormat.cend() &&
                        std::distance(itClosedBracket, itOpenBracket) > 0)
                    {
                        if (numOptionalArgs >= 0)
                            ++numOptionalArgs;
                    }
                    else
                    {
                        bBracketsNotBalanced = true;
                        break;
                    }

                    itPreviousClosedBracket = itClosedBracket;
                    itArgumentFormat = itClosedBracket + 1;
                }
                else
                    break;
            }

            if (bBracketsNotBalanced)
            {
                logMsg("usage", LoggingLevel::Enum::Usage,
                       "The argument format string for the \"" + option + "\" option is invalid; "
                       "the opening and closing brackets must be balanced.\n\n",
                       getQualifiedMethodName(__func__));

                bSuccess = false;
            }

            // if the argument format vector is empty, then it's a boolean
            // option which is assigned a "true" value in the parse function
            if (argumentFormat.empty())
                numRequiredArgs = 1;

            if (bSuccess)
            {
                size_t maxNumArguments = numOptionalArgs + numRequiredArgs;
                if (numArgs < numRequiredArgs || (numOptionalArgs >= 0 && numArgs > maxNumArguments))
                {
                    auto *pLogger = const_cast<CLI_ArgumentProcessor *>(this)->getLoggingStream("usage");
                    if (pLogger != nullptr)
                        printUsage(*pLogger);

                    bSuccess = false;
                }
            }
        }

        return bSuccess;
    }

public:

    /**
     * Function to clear the CLI processor's options-usage map
     */
    inline virtual void clear(void) final
    {
        m_optionsUsageMap.clear();
    }

    /**
     * clone() function
     */
    inline virtual CLI_ArgumentProcessor *clone(void) const override
    {
        return new CLI_ArgumentProcessor(*this);
    }

    /**
     * Function to create a dynamically-allocated command line argument list from the input vector of strings;
     * upon success, the function returns a non-null 2-d dynamically-allocated argument vector of character
     * strings
     */
    template<typename Container> inline static char **createDynamicArgumentVector(Container &&arguments)
    {
        char **argv = nullptr;
        if (!arguments.empty())
        {
            argv = new char * [arguments.size()];
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                auto &&size = arguments[i].size() + 1;
                argv[i] = new char [size];
                std::memset(argv[i], 0, size * sizeof(char));
                std::strncpy(argv[i], arguments[i].c_str(), size - 1);
            }
        }

        return argv;
    }

    /**
     * Return whether or not the options-usage map is empty
     */
    inline virtual bool empty(void) final
    {
        return m_optionsUsageMap.empty();
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "CLI_ArgumentProcessor";
    }

    /**
     * Function to parse the argument vector, populating a map of options with corresponding arguments
     * @param argc               the argument count
     * @param argv               an array of command-line arguments
     * @param optionArgumentsMap a map of options with corresponding arguments
     */
    virtual bool parse(int argc, char **argv, tTokenVectorPair &optionArgumentsMap) const final
    {
        auto &&itCurrentOptionUsagePair = m_optionsUsageMap.cend();
        for (int i = 1; i < argc; ++i)
        {
            auto &&argument = utilities::string::StringUtilities::removeOuterQuotes(argv[i]);
            auto &&itOptionUsagePair = m_optionsUsageMap.find(argument);
            if (itOptionUsagePair != m_optionsUsageMap.cend())
            {
                if (i + 1 == argc)
                    itCurrentOptionUsagePair = itOptionUsagePair;

                if (itCurrentOptionUsagePair != m_optionsUsageMap.cend() &&
                    optionArgumentsMap[itCurrentOptionUsagePair->first].empty())
                {
                    // if the option has no arguments, then it must be a flag
                    optionArgumentsMap[itCurrentOptionUsagePair->first] = { "true" };
                }

                if (optionArgumentsMap[itOptionUsagePair->first].empty())
                    itCurrentOptionUsagePair = itOptionUsagePair;
            }
            else if (itCurrentOptionUsagePair != m_optionsUsageMap.cend())
                optionArgumentsMap[itCurrentOptionUsagePair->first].push_back(argument);
            else
                return false;
        }

        return true;
    }

    /**
     * Function to print CLI argument usage to the specified stream
     * @param stream a reference to an std::ostream object
     */
    virtual void printUsage(std::ostream &stream) const override final
    {
        stream << "Usage:" << std::endl << std::endl;

        // determine field widths before printing...
        int argWidth = 0, optWidth = 0;
        for (auto &&option : m_optionsUsageMap)
        {
            argWidth = std::max(argWidth, int(option.second.first.size()));
            optWidth = std::max(optWidth, int(option.first.size()));
        }

        Tokenizer tokenizer("\r", "\n");
        for (auto &&option : m_optionsUsageMap)
        {
            stream << " " << std::left << std::setw(optWidth) << option.first;
            stream << " " << std::left << std::setw(argWidth) << option.second.first << " : ";

            tokenizer.initialize(option.second.second);
            for (auto &&itToken = tokenizer.cbegin(); itToken != tokenizer.cend(); ++itToken)
            {
                if (itToken == tokenizer.cbegin())
                    stream << *itToken << std::endl;
                else
                {
                    int width = optWidth + argWidth + int(itToken->size()) + 5;
                    stream << std::right << std::setw(width) << *itToken << std::endl;
                }
            }
        }

        stream << std::endl;
    }

    /**
     * Function to process an options-arguments token map and store the results in the variable registry
     */
    virtual bool process(const tTokenVectorPair &optionArgumentsMap,
                         utilities::Dictionary &dictionary) const final
    {
        auto *pRegistry = dictionary.getVariableRegistry();
        bool bSuccess = (pRegistry != nullptr);
        if (bSuccess)
        {
            std::stringstream stream;
            stream.imbue(std::locale(std::locale(), CustomLocale::create()));
            for (auto &&itOption : optionArgumentsMap)
            {
                auto &&itRegistryEntry = pRegistry->findByName(itOption.first);
                if (itRegistryEntry != pRegistry->cend())
                {
                    auto &&numArgs = itOption.second.size();
                    bSuccess &= checkOptionArguments(itOption.first, numArgs);
                    if (bSuccess)
                    {
                        stream.clear();
                        stream.str("");
                        std::copy(itOption.second.cbegin(), itOption.second.cend(),
                                  std::ostream_iterator<std::string>(stream, numArgs > 1 ? "," : ""));

                        bSuccess &= itRegistryEntry->second.assign(stream);
                        if (!bSuccess)
                        {
                            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                                      << "Error interpreting program option \"" << itOption.first
                                      << "\" with the following argument(s):";

                            for (auto &&argument : itOption.second)
                                std::cout << " \"" << argument << "\"";

                            std::cout << std::endl << std::endl;
                        }
                        else if (m_bRemoveProcessedVariablesFromRegistry)
                            bSuccess = pRegistry->removeVariables(itRegistryEntry->second);
                    }
                }
            }
        }

        return bSuccess;
    }

    /**
     * Function to process the command-line input and store the results in the variable registry
     * @param argc       the argument count
     * @param argv       an array of command-line arguments
     * @param dictionary a reference to a Dictionary object
     */
    virtual bool process(int argc, char **argv, utilities::Dictionary &dictionary) const final
    {
        bool bSuccess = true;

        int numArgs = 0;
        std::string arguments;
        auto &&itOption = m_optionsUsageMap.cend();
        auto *pRegistry = dictionary.getVariableRegistry();
        for (int i = 1; i <= argc; ++i)
        {
            auto &&argument = utilities::string::StringUtilities::removeOuterQuotes(argv[i % argc]);
            auto &&itNextOption = m_optionsUsageMap.find(argument);
            if (itNextOption != m_optionsUsageMap.cend() || i == argc)
            {
                if (itOption != m_optionsUsageMap.cend())
                {
                    bSuccess &= checkOptionArguments(itOption->first, numArgs);
                    if (bSuccess)
                    {
                        std::istringstream stream(arguments);
                        if (pRegistry->contains(itOption->first))
                            pRegistry->assign(itOption->first, stream);
                    }

                    arguments.clear();
                }

                if (bSuccess)
                {
                    numArgs = 0;
                    if (i != argc)
                        itOption = itNextOption;
                }
            }
            else if (itOption != m_optionsUsageMap.cend())
            {
                argument = utilities::string::StringUtilities::removeOuterQuotes(argv[i]);
                arguments += argument + " ";
                ++numArgs;
            }
        }

        return bSuccess;
    }

    /**
     * Remove an option from the argument processor
     * @param option a string of the form "-option" that specifies the program option to be removed
     */
    inline virtual bool removeOption(const std::string &option) final
    {
        auto &&itOption = m_optionsUsageMap.find(option);
        bool bSuccess = (itOption != m_optionsUsageMap.end());
        if (bSuccess)
            m_optionsUsageMap.erase(itOption);

        return bSuccess;
    }

    /**
     * Query whether or not variable entries associated with successfully processed options will be removed
     * from the corresponding dictionary's variable registry
     */
    inline virtual bool removeProcessedVariablesFromRegistry(void) const final
    {
        return m_bRemoveProcessedVariablesFromRegistry;
    }

    /**
     * Enable/disable removal of variable entries associated with successfully processed options from the
     * corresponding dictionary's variable registry
     */
    inline virtual void removeProcessedVariablesFromRegistry(bool bEnabled) final
    {
        m_bRemoveProcessedVariablesFromRegistry = bEnabled;
    }

private:

    /**
     * flag to indicate removal of entries associated with successfully processed options from the
     * corresponding dictionary's variable registry
     */
    bool m_bRemoveProcessedVariablesFromRegistry;

    /**
     * map of CLI options with corresponding usage & help
     */
    std::map<std::string, tTokenPair> m_optionsUsageMap;
};

}

#endif

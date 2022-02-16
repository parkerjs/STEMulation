#include "dictionary.h"
#include "prefix_tree.h"
#include "testPrefixTree.h"
#include "unitTestManager.h"
#include <chrono>
#include <fstream>
#include <iomanip>

// using namespace declarations
using namespace attributes::abstract;
using namespace containers::tree;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testPrefixTree", &PrefixTreeUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
PrefixTreeUnitTest::PrefixTreeUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
PrefixTreeUnitTest *PrefixTreeUnitTest::create(UnitTestManager *pUnitTestManager)
{
    PrefixTreeUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new PrefixTreeUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * unit test for prefix tree
 */
bool PrefixTreeUnitTest::execute(void)
{
    std::cout << "Starting unit test for PrefixTree..." << std::endl << std::endl;

    bool bSuccess = dependenciesInitialized();
    if (bSuccess)
    {
        size_t max_word_length = 0;
        const char nullTerminator(' '); // string null terminator
        const char wildcard('*');

        std::ifstream inFile("bin/inputs/prefixTreeTestInput.dat");
        std::ofstream outFile("bin/outputs/prefixTreeTestOutput.dat");

        std::string str;
        std::vector<std::string> dictionary; // vector of strings
        std::vector<std::string>::iterator itString;
        std::vector<std::vector<size_t>> wildcardLocations;
        PrefixTree<char> tree;

        // read in test file
        while (inFile)
        {
            if (inFile >> str)
            {
                // store strings in dictionary in randomized order
                max_word_length = (str.size() > max_word_length) ? str.size() : max_word_length;

                // must add a specific null terminator since
                // some words will be subsets of other words
                dictionary.push_back(str + nullTerminator);
            }
        }

        // insert shuffled words into the tree
        auto &&begin = std::chrono::system_clock::now();
        for (size_t i = 0; i < dictionary.size(); ++i)
            tree.insert(dictionary[i]);

        auto &&end = std::chrono::system_clock::now();
        std::cout << "Total insertion time elapsed in seconds: "
                  << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()
                  << std::endl;

        // create a copy of the tree
        PrefixTree<char> copyOfTree(tree);

        // print the contents of the tree to file
        outFile << "Contents of the tree:" << std::endl;
        outFile << copyOfTree << std::endl << std::endl;

        // replace some of the characters in the dictionary with wild-cards
        // to see if the tree can successfully retrieve a string that matches
        // the pattern
        wildcardLocations.resize(max_word_length);
        for (size_t i = 0; i < wildcardLocations.size(); ++i)
        {
            auto &&k = rand() % (i + 1);
            k = (k > 0) ? k : 1;
            size_t j = 0;
            while (j < k)
            {
                auto &&l = rand() % (i + 1);
                if (std::find(wildcardLocations[i].begin(), wildcardLocations[i].end(), l) == wildcardLocations[i].end())
                {
                    wildcardLocations[i].push_back(l);
                    ++j;
                }
            }
        }

        outFile << "Contents of dictionary with wild-cards introduced:" << std::endl;

        for (size_t i = 0; i < dictionary.size(); ++i)
        {
            outFile << std::setw(20) << dictionary[i] << " -> ";
            auto &&j = dictionary[i].size() - 1; // subtract one for the null terminator
            auto &&k = wildcardLocations[j - 1].size();
            for (size_t l = 0; l < k - 1; ++l)
                dictionary[i][wildcardLocations[j - 1][l]] = '*';

            outFile << dictionary[i] << std::endl;
        }

        outFile << std::endl << std::endl;

        // search the tree for the strings with wild-cards
        outFile << std::setw(20) << "Search input";
        outFile << std::setw(10) << "Success";
        outFile << std::setw(20) << "Search output" << std::endl;

        // define the node retrieval functor
        auto &&retrievalFunc = [=] (const char &left, const char &right)
        {
            bool bLeftIsNull = (left == nullTerminator);
            bool bRightIsNull = (right == nullTerminator);
            if (bLeftIsNull ^ bRightIsNull)
                return false;

            return (left == right || right == wildcard);
        };

        for (size_t i = 0; i < dictionary.size(); ++i)
        {
            outFile << std::setw(20) << dictionary[i];
            if (copyOfTree.remove(dictionary[i], std::bind(retrievalFunc, std::placeholders::_1,
                                                           std::placeholders::_2), true))
                outFile << std::setw(10) << "Passed";
            else
                outFile << std::setw(10) << "Failed";

            outFile << std::setw(20) << dictionary[i] << std::endl;
        }

        bool bCopyOfTreeIsEmpty = copyOfTree.empty();
        bSuccess &= bCopyOfTreeIsEmpty;

        outFile << std::endl << "All keys were" << (bCopyOfTreeIsEmpty ? "" : " not ")
                << " successfully found and deleted from tree." << std::endl << std::endl;

        // search the tree for strings that are not in the dictionary
        std::vector<std::string> wordsNotInDictionary;

        wordsNotInDictionary.push_back("");
        wordsNotInDictionary.push_back("woohoot");
        wordsNotInDictionary.push_back("brouhaha");
        wordsNotInDictionary.push_back("suoicodilaipxecitsiligarfilacrepus");
        wordsNotInDictionary.push_back("dociousaliexpiisticfragicalirupus");
        wordsNotInDictionary.push_back("supercalifragilisticexpialidocious");
        wordsNotInDictionary.push_back("zatooba");
        wordsNotInDictionary.push_back("zzz");

        outFile << "Words that are not in the dictionary" << std::endl;
        outFile << std::setw(20) << "Search input";
        outFile << std::setw(10) << "Success";
        outFile << std::setw(20) << "Search output" << std::endl;

        for (size_t i = 0; i < wordsNotInDictionary.size(); ++i)
        {
            outFile << std::setw(20) << wordsNotInDictionary[i];
            if (tree.retrieve(wordsNotInDictionary[i]))
            {
                outFile << std::setw(10) << "Failed";
                tree.remove(wordsNotInDictionary[i]);
                bSuccess &= false;
            }
            else
                outFile << std::setw(10) << "Passed";

            outFile << std::setw(20) << wordsNotInDictionary[i] << std::endl;
        }

        outFile << std::endl;

        // test to see if copyOfTree is empty
        bSuccess &= copyOfTree.empty();

        // now destroy the original tree
        tree.destroy();

        // test to see if tree is empty
        bool bTreeIsEmpty = tree.empty();

        bSuccess &= bTreeIsEmpty;

        outFile << std::endl << "All keys were" << (bTreeIsEmpty ? " " : " not ")
                << "successfully found and deleted from tree." << std::endl << std::endl;

        bSuccess &= bCopyOfTreeIsEmpty;

        // close files;
        inFile.close();
        outFile.close();
    }

    std::cout << "Test " << (bSuccess ? "PASSED" : "FALIED") << "." << std::endl << std::endl;

    return bSuccess;
}

}

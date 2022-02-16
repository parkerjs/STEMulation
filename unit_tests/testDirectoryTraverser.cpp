#include "directory_traverser.h"
#include "testDirectoryTraverser.h"
#include "unitTestManager.h"
#include <fstream>

// using namespace declarations
using namespace attributes::abstract;
using namespace messaging;
using namespace utilities::file_system;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testDirectoryTraverser",
                                                    &DirectoryTraverserUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
DirectoryTraverserUnitTest::DirectoryTraverserUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
DirectoryTraverserUnitTest *DirectoryTraverserUnitTest::create(UnitTestManager *pUnitTestManager)
{
    DirectoryTraverserUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new DirectoryTraverserUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool DirectoryTraverserUnitTest::execute(void)
{
    std::cout << "Starting unit test for DirectoryTraverser..." << std::endl << std::endl;

    size_t numFiles = 0;
    bool bSuccess = dependenciesInitialized();
    if (bSuccess)
    {
        std::unique_ptr<DirectoryTraverser> pTraverser(DirectoryTraverser::create());
        bSuccess = (pTraverser != nullptr);
        if (bSuccess)
        {
            auto &&outputFile = "bin/outputs/dirctoryTraversal.txt";
            std::ofstream stream(outputFile);

            pTraverser->setPath("./");
            auto &&itFolder = pTraverser->cbegin();
            while (itFolder != pTraverser->cend())
            {
                stream << itFolder->c_str() << std::endl;
                ++itFolder;
                ++numFiles;
            }

            std::set<std::string> &&files = pTraverser->findFiles();

            bSuccess = (numFiles == files.size());
        }
    }

    if (bSuccess)
        std::cout << "Test PASSED, " << numFiles << " file(s) discovered." << std::endl << std::endl;
    else
        std::cout << "Test FAILED." << std::endl << std::endl;

    return bSuccess;
}

}

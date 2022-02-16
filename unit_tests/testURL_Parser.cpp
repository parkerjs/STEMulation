#include "testURL_Parser.h"
#include "unitTestManager.h"
#include "URL_Parser.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace messaging;
using namespace networking;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testURL_Parser", &URL_ParserUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
URL_ParserUnitTest::URL_ParserUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
URL_ParserUnitTest *URL_ParserUnitTest::create(UnitTestManager *pUnitTestManager)
{
    URL_ParserUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new URL_ParserUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool URL_ParserUnitTest::execute(void)
{
    std::vector<std::string> exampleURLs =
    { "abc://username:password@example.com:123/path/data?key=value&key2=value2#fragid1",
      "http://www.google.co.kr:8080/testurl/depth1/depth2/depth3?name=james&id=100",
      "https://example.org/absolute/URI/with/absolute/path/to/resource.txt",
      "https://example.org/absolute/URI/with/absolute/path/to/resource",
      "ftp://example.org/resource.txt",
      "example.org/scheme-relative/URI/with/absolute/path/to/resource.txt",
      "example.org/scheme-relative/URI/with/absolute/path/to/resource",
      "/relative/URI/with/absolute/path/to/resource.txt",
      "relative/path/to/resource.txt",
      "../../../resource.txt",
      "./resource.txt#frag01",
      "/resource.txt",
      "#frag01",
      "mailto:someone@example.com:443?subject=This%20is%20the%20subject&cc=someone_else@example.com",
      "someone@aol.com",
      "firstname.lastname:asdf@host.net",
      "/quote/SPY?p=SPY",
      "resource_with_no_dot_is_interpreted_as_path"
    };

    std::vector<std::vector<std::string>> outputURL_Components =
    { { "fragid1", "example.com", "/path/data", "123", "key=value&key2=value2", "abc", "username", "password" },
      { "", "www.google.co.kr", "/testurl/depth1/depth2/depth3", "8080", "name=james&id=100", "http", "", "" },
      { "", "example.org", "/absolute/URI/with/absolute/path/to/resource.txt", "", "", "https", "", "" },
      { "", "example.org", "/absolute/URI/with/absolute/path/to/resource", "", "", "https", "", "" },
      { "", "example.org", "/resource.txt", "", "", "ftp", "", "" },
      { "", "example.org", "/scheme-relative/URI/with/absolute/path/to/resource.txt", "", "", "", "", "" },
      { "", "example.org", "/scheme-relative/URI/with/absolute/path/to/resource", "", "", "", "", "" },
      { "", "", "/relative/URI/with/absolute/path/to/resource.txt", "", "", "", "", "" },
      { "", "", "relative/path/to/resource.txt", "", "", "", "", "" },
      { "", "", "../../../resource.txt", "", "", "", "", "" },
      { "frag01", "", "./resource.txt", "", "", "", "", "" },
      { "", "", "/resource.txt", "", "", "", "", "" },
      { "frag01", "", "", "", "", "", "", "" },
      { "", "example.com", "", "443", "subject=This%20is%20the%20subject&cc=someone_else@example.com", "",
        "mailto", "someone"},
      { "", "aol.com", "", "", "", "", "someone", ""},
      { "", "host.net", "", "", "", "", "firstname.lastname", "asdf"},
      { "", "", "/quote/SPY", "", "p=SPY", "", "", ""},
      { "", "", "resource_with_no_dot_is_interpreted_as_path", "", "", "", "", ""}
    };

    std::cout << "Starting unit test for URL_Parser..." << std::endl << std::endl;

    enum URL_Component { Fragment, Host, Path, Port, Query, Scheme, UserName, UserPassword };
    std::map<URL_Component, std::string> componentsMap;

    bool bSuccess = true; // assume success
    URL_Parser parser;
    for (std::size_t example = 0; bSuccess && example < exampleURLs.size(); ++example)
    {
        parser.parse(exampleURLs[example]);
        componentsMap[Fragment] = parser.getFragment();
        componentsMap[Host] = parser.getHost();
        componentsMap[Path] = parser.getPath();
        componentsMap[Port] = parser.getPort();
        componentsMap[Query] = parser.getQuery();
        componentsMap[Scheme] = parser.getScheme();
        componentsMap[UserName] = parser.getUserName();
        componentsMap[UserPassword] = parser.getUserPassword();

        bSuccess = (outputURL_Components[example][0] == componentsMap[Fragment] &&
                    outputURL_Components[example][1] == componentsMap[Host] &&
                    outputURL_Components[example][2] == componentsMap[Path] &&
                    outputURL_Components[example][3] == componentsMap[Port] &&
                    outputURL_Components[example][4] == componentsMap[Query] &&
                    outputURL_Components[example][5] == componentsMap[Scheme] &&
                    outputURL_Components[example][6] == componentsMap[UserName] &&
                    outputURL_Components[example][7] == componentsMap[UserPassword]);
    }

    std::cout << std::endl << (bSuccess ? "Test PASSED." : "Test FAILED.") << std::endl << std::endl;

    return bSuccess;
}

}

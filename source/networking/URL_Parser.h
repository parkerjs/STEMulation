#ifndef URL_PARSER_H
#define URL_PARSER_H

#include "URL.h"

namespace networking
{

/**
 * Uniform Resource Layer (URL) parsing utility class
 */
class URL_Parser
: public URL
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM URL_Parser(void);

    /**
     * Constructor
     * @param url the web address to be parsed
     */
    EXPORT_STEM URL_Parser(const std::string &url);

    /**
     * Copy constructor
     */
    EXPORT_STEM URL_Parser(const URL_Parser &parser);

    /**
     * Move constructor
     */
    EXPORT_STEM URL_Parser(URL_Parser &&parser);

    /**
     * Destructor
     */
    EXPORT_STEM ~URL_Parser(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM URL_Parser &operator = (const URL_Parser &parser);

    /**
     * Move assignment operator
     */
    EXPORT_STEM URL_Parser &operator = (URL_Parser &&parser);

    /**
     * clone() function
     */
    EXPORT_STEM virtual URL_Parser *clone(void) const override;

    /**
     * Parse the specified URL; sets this object's current url to the input argument
     * @param url the web address to be parsed
     */
    EXPORT_STEM virtual bool parse(const std::string &url) final;

private:

    /**
     * Parse the fragment identifier
     * @param url the web address to be parsed
     * @param pos the position within the URL string currently under examination
     */
    EXPORT_STEM virtual void parseFragment(const std::string &url, std::size_t &pos) final;

    /**
     * Parse the host name
     * @param url the web address to be parsed
     * @param pos the position within the URL string currently under examination
     */
    EXPORT_STEM virtual void parseHost(const std::string &url, std::size_t &pos) final;

    /**
     * Parse the path component
     * @param url the web address to be parsed
     * @param pos the position within the URL string currently under examination
     */
    EXPORT_STEM virtual void parsePath(const std::string &url, std::size_t &pos) final;

    /**
     * Parse the port number
     * @param url the web address to be parsed
     * @param pos the position within the URL string currently under examination
     */
    EXPORT_STEM virtual void parsePort(const std::string &url, std::size_t &pos) final;

    /**
     * Parse the query string
     * @param url the web address to be parsed
     * @param pos the position within the URL string currently under examination
     */
    EXPORT_STEM virtual void parseQuery(const std::string &url, std::size_t &pos) final;

public:

    /**
     * Parse query parameter from query string
     * @param query the query string
     */
    EXPORT_STEM virtual void parseQueryParameters(const std::string &query) final;

private:

    /**
     * Parse the protocol scheme
     * @param url the web address to be parsed
     * @param pos the position within the URL string currently under examination
     */
    EXPORT_STEM virtual void parseScheme(const std::string &url, std::size_t &pos) final;

    /**
     * Parse the user information
     * @param url the web address to be parsed
     * @param pos the position within the URL string currently under examination
     */
    EXPORT_STEM virtual void parseUserInformation(const std::string &url, std::size_t &pos) final;
};

}

#endif

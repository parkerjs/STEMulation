#ifndef JSON_TO_CSV_CONVERTER_H
#define JSON_TO_CSV_CONVERTER_H

#include "json_to_prefix_tree_converter.h"
#include <sstream>

namespace utilities
{

namespace json
{

/**
 * This class implements code to convert json-formatted data into csv format
 */
class JsonToCsvConverter final
{
public:

    /**
     * Constructor
     */
    JsonToCsvConverter(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    JsonToCsvConverter(const JsonToCsvConverter &converter) = delete;

    /**
     * Move constructor
     */
    JsonToCsvConverter(JsonToCsvConverter &&converter) = delete;

public:

    /**
     * Destructor
     */
    ~JsonToCsvConverter(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    JsonToCsvConverter &operator = (const JsonToCsvConverter &converter) = delete;

    /**
     * Move assignment operator
     */
    JsonToCsvConverter &operator = (JsonToCsvConverter &&converter) = delete;

public:

    /**
     * Function to convert a json node object and its corresponding subtree to csv-format
     * @param value             a Json::Value node object
     * @param stream            a reference to an std::ostream object
     * @param bRemoveSingletons flag indicating that singleton nodes will not be included in the output
     */
    static bool convert(Json::Value &value, std::ostream &stream, bool bRemoveSingletons = false)
    {
        JsonToPrefixTreeConverter::PrefixTree tree;
        bool bSuccess = JsonToPrefixTreeConverter::convert(value, tree, bRemoveSingletons);
        if (bSuccess)
        {
            // singletons should already be gone, but call the following for redundancy
            if (bRemoveSingletons)
                bSuccess = tree.removeSingletons();

            if (bSuccess)
                tree.print(stream, ",");
        }

        return bSuccess;
    }
};

}

}

#endif

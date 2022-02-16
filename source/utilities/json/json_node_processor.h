#ifndef JSON_NODE_PROCESSOR_H
#define JSON_NODE_PROCESSOR_H

#include <jsoncpp/json.h>

namespace utilities
{

namespace json
{

/**
 * This class implements a json object node processing utility
 */
class JsonNodeProcessor final
{
public:

    /**
     * Constructor
     */
    JsonNodeProcessor(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    JsonNodeProcessor(const JsonNodeProcessor &processor) = delete;

    /**
     * Move constructor
     */
    JsonNodeProcessor(JsonNodeProcessor &&processor) = delete;

public:

    /**
     * Destructor
     */
    ~JsonNodeProcessor(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    JsonNodeProcessor &operator = (const JsonNodeProcessor &processor) = delete;

    /**
     * Move assignment operator
     */
    JsonNodeProcessor &operator = (JsonNodeProcessor &&processor) = delete;

public:

    /**
     * Function to process a Json node and its corresponding subtree
     * @param node     a Json::Value node object
     * @param function the node processing function
     */
    template<typename Function>
    static bool process(Json::Value &node, Function &&function)
    {
        bool bSuccess = true;
        if (node.isArray())
        {
            for (Json::ArrayIndex i = 0; bSuccess && i < node.size(); ++i)
            {
                bSuccess = function(std::to_string(i), node[i]);
                if (bSuccess)
                    bSuccess = process(node[i], std::forward<Function>(function));
            }
        }
        else if (node.isObject())
        {
            auto &&keys = node.getMemberNames();
            for (size_t i = 0; bSuccess && i < keys.size(); ++i)
            {
                auto &&key = keys[i];
                bSuccess = function(key, node[key]);
            }
        }

        return bSuccess;
    }
};

}

}

#endif

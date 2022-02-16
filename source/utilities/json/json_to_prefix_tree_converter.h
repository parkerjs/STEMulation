#ifndef JSON_TO_PREFIX_TREE_CONVERTER_H
#define JSON_TO_PREFIX_TREE_CONVERTER_H

#include "any.h"
#include "prefix_tree.h"
#include <functional>
#include <jsoncpp/json.h>

namespace utilities
{

namespace json
{

/**
 * This class implements code to convert json-formatted data into an equivalent prefix tree data structure
 */
class JsonToPrefixTreeConverter final
{
public:

    /**
     * Typedef declarations
     */
    using PrefixTree = containers::tree::PrefixTree<functional::Any>;
    using TreeNode = containers::tree::TreeNode<functional::Any>;

    /**
     * Constructor
     */
    JsonToPrefixTreeConverter(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    JsonToPrefixTreeConverter(const JsonToPrefixTreeConverter &converter) = delete;

    /**
     * Move constructor
     */
    JsonToPrefixTreeConverter(JsonToPrefixTreeConverter &&converter) = delete;

public:

    /**
     * Destructor
     */
    ~JsonToPrefixTreeConverter(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    JsonToPrefixTreeConverter &operator = (const JsonToPrefixTreeConverter &converter) = delete;

    /**
     * Move assignment operator
     */
    JsonToPrefixTreeConverter &operator = (JsonToPrefixTreeConverter &&converter) = delete;

public:

    /**
     * Function to convert json-formatted data into an equivalent prefix tree data structure
     * @param value             a Json::Value node object
     * @param tree              upon success, will be populated
     * @param bRemoveSingletons flag indicating that singleton nodes will not be included in the output
     */
    inline static bool convert(Json::Value &value, PrefixTree &tree, bool bRemoveSingletons = false)
    {
        auto *pRoot = tree.getRoot();
        bool bSuccess = (pRoot != nullptr);
        if (bSuccess)
            bSuccess = convert(value, pRoot, bRemoveSingletons);

        return bSuccess;
    }

protected:

    /**
     * Function to convert json-formatted data into an equivalent prefix tree data structure
     * @param value             a Json::Value node object
     * @param pNode             a pointer to the current tree node
     * @param bRemoveSingletons flag indicating that singleton nodes will not be included in the output
     */
    static bool convert(Json::Value &value, TreeNode *pNode, bool bRemoveSingletons)
    {
        bool bSuccess = (pNode != nullptr);
        if (bSuccess)
        {
            switch (value.type())
            {
                default: // not a valid enumeration
                bSuccess = false;
                break;

                case Json::nullValue:
                pNode->addChild(new TreeNode(std::string("null"), pNode));
                break;

                case Json::booleanValue:
                pNode->addChild(new TreeNode(std::string(value.asBool() ? "true" : "false"), pNode));
                break;

                case Json::intValue:
                pNode->addChild(new TreeNode(value.asLargestInt(), pNode));
                break;

                case Json::uintValue:
                pNode->addChild(new TreeNode(value.asLargestUInt(), pNode));
                break;

                case Json::realValue:
                pNode->addChild(new TreeNode(value.asDouble(), pNode));
                break;

                case Json::stringValue:
                pNode->addChild(new TreeNode(value.asString(), pNode));
                break;

                case Json::arrayValue:
                for (Json::ArrayIndex i = 0; bSuccess && i < value.size(); ++i)
                {
                    TreeNode *pChild = pNode;
                    if (!bRemoveSingletons || (bRemoveSingletons && value.size() > 1))
                    {
                        pChild = new TreeNode(i, pNode);
                        pNode->addChild(pChild);
                    }

                    bSuccess = convert(value[i], pChild, bRemoveSingletons);
                }

                break;

                case Json::objectValue:
                {
                    auto &&keys = value.getMemberNames();
                    for (size_t i = 0; bSuccess && i < keys.size(); ++i)
                    {
                        std::string key(keys[i]);
                        TreeNode *pChild = pNode;
                        if (!bRemoveSingletons || (bRemoveSingletons && value.size() > 1))
                        {
                            pChild = new TreeNode(key, pNode);
                            pNode->addChild(pChild);
                        }

                        bSuccess = convert(value[key], pChild, bRemoveSingletons);
                    }
                }
            }
        }

        return bSuccess;
    }
};

}

}

#endif

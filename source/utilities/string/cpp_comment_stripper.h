#ifndef CPP_COMMENT_STRIPPER_H
#define CPP_COMMENT_STRIPPER_H

#include <string>

namespace utilities
{

namespace string
{

/**
 * Class that contains code to strip C/C++ style comments from a strip
 */
class CppCommentStripper final
{
private:

    /**
     * Enumerations
     */
    enum class Status
    {
        EscapeAfterSlash,       // /\        (`/' followed by `\')
        EscapeAfterStar,        // /*...*\   (`*\' in C style comment)
        EscapeNext,             // "string\  (`\' in string literal)
        EscapeWithinCppComment, // //...\    (`\' in C++ style comment)
        FirstSlash,             // '/'       (seen `/')
        Normal,                 //           (normal)
        StarWithinC_Comment,    //           (`*' in C style comment)
        WithinC_Comment,        //           (in C style comment)
        WithinCppComment,       //           (in C++ style comment)
        WithinQuotes            // "string   (literal string)
    };

    /**
     * Constructor
     */
    CppCommentStripper(void)
    {

    }

public:

    /**
     * Destructor
     */
    ~CppCommentStripper(void)
    {

    }

    /**
     * Function to strip comments (C/C++ style comments)
     */
    static bool strip(std::string &string)
    {
        int numNewlineEscapes = 0;
        Status status = Status::Normal;
        auto &&itInput = string.begin(), &&itOutput = string.begin();
        while (itInput != string.end())
        {
            auto &&ch = *itInput++;
            switch (status)
            {
                case Status::Normal:
                if ('"' == ch)
                {
                    status = Status::WithinQuotes;
                    *itOutput++ = ch;
                }
                else if ('/' == ch)
                {
                    status = Status::FirstSlash;
                    numNewlineEscapes = 0;
                }
                else
                    *itOutput++ = ch;

                break;

                case Status::WithinQuotes:
                if ('\\' == ch)
                    status = Status::EscapeNext;
                else if ('"' == ch)
                    status = Status::Normal;

                *itOutput++ = ch;
                break;

                case Status::EscapeNext:
                status = Status::WithinQuotes;
                *itOutput++ = ch;
                break;

                case Status::FirstSlash:
                if ('/' == ch)
                {
                    status = Status::WithinCppComment;
                    numNewlineEscapes = 0;
                }
                else if ('*' == ch)
                {
                    status = Status::WithinC_Comment;
                    numNewlineEscapes = 0;
                }
                else if ('\\' == ch)
                    status = Status::EscapeAfterSlash;
                else
                {
                    status = Status::Normal;
                    *itOutput++ = '/';
                    for (; numNewlineEscapes > 0; --numNewlineEscapes)
                    {
                        *itOutput++ = '\\';
                        *itOutput++ = '\n';
                    }

                    *itOutput++ = ch;
                }

                break;

                case Status::WithinC_Comment:
                if ('*' == ch)
                    status = Status::StarWithinC_Comment;

                break;

                case Status::StarWithinC_Comment:
                if ('/' == ch)
                    status = Status::Normal;
                else if ('\\' == ch)
                    status = Status::EscapeAfterStar;
                else if ('*' != ch)
                    status = Status::WithinC_Comment;

                break;

                case Status::WithinCppComment:
                if ('\n' == ch)
                {
                    status = Status::Normal;
                    *itOutput++ = ch;
                }
                else if ('\\' == ch)
                    status = Status::EscapeWithinCppComment;

                break;

                case Status::EscapeAfterSlash:
                if ('\n' == ch) // until now we are not sure if this newline would be part of a comment
                {
                    status = Status::FirstSlash;
                    ++numNewlineEscapes;
                }
                else if ('/' == ch)
                {
                    status = Status::FirstSlash;
                    *itOutput++ = '/';
                    for (; numNewlineEscapes > 0; --numNewlineEscapes)
                    {
                        *itOutput++ = '\\';
                        *itOutput++ = '\n';
                    }

                    *itOutput++ = '\\';
                }
                else
                {
                    status = Status::Normal;

                    *itOutput++ = '/';
                    for (; numNewlineEscapes > 0; --numNewlineEscapes)
                    {
                        *itOutput++ = '\\';
                        *itOutput++ = '\n';
                    }

                    *itOutput++ = '\\';
                    *itOutput++ = ch;
                }

                break;

                case Status::EscapeAfterStar:
                if ('\n' == ch)
                    status = Status::StarWithinC_Comment;
                else if ('*' == ch)
                    status = Status::StarWithinC_Comment;
                else
                    status = Status::WithinC_Comment;

                break;

                case Status::EscapeWithinCppComment:
                if ('\n' == ch)
                    status = Status::WithinCppComment;
                else
                    status = Status::WithinCppComment;

                break;
            }
        }

        string.erase(itOutput, itInput);

        return true;
    }
};

}

}

#endif

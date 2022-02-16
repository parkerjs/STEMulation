#ifndef LINE_GETTER_H
#define LINE_GETTER_H

#include <algorithm>
#include <fstream>
#include <string>

namespace utilities
{

namespace string
{

/**
 * This class contains methods to retrieve lines from an input stream
 */
class LineGetter final
{
public:

    /**
     * Constructor
     */
    LineGetter(char delimiter = '\n')
    : m_delimiter(delimiter)
    {

    }

    /**
     * Destructor
     */
    ~LineGetter(void)
    {

    }

    /**
     * Function to retrieve the line between the previous and next delimiters. If the stream currently points
     * to a delimiter, then the next line is extracted
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getCurrent(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getCurrent(stream, line, position);
    }

    /**
     * Function to retrieve the line between the previous and next delimiters. If the stream currently points
     * to a delimiter, then the next line is extracted.
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getCurrent(std::istream &stream, std::string &line, std::streamoff &position) final
    {
        position = std::string::npos;
        bool bSuccess = false;
        if (stream)
        {
            if (stream.peek() == m_delimiter)
            {
                stream.ignore(1);
                bSuccess = getUntilNextDelimiter(stream, line, position);
            }
            else
            {
                std::string temp;
                std::streamoff previousPosition = stream.tellg();
                bSuccess = getUntilPreviousDelimiter(stream, temp, position);
                stream.seekg(previousPosition + 1LL, std::ios_base::beg);
                bSuccess &= getUntilNextDelimiter(stream, line);

                line = temp + line;
            }
        }

        return bSuccess;
    }

    /**
     * Get the current delimiter
     */
    inline virtual char getDelimiter(void) const final
    {
        return m_delimiter;
    }

    /**
     * Function to retrieve first line in the input stream up to the first delimiter (delimiter is consumed)
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getFirst(std::istream &stream, std::string &line) final
    {
        if (stream)
        {
            stream.seekg(0, std::ios_base::beg);

            return getUntilNextDelimiter(stream, line);
        }

        return false;
    }

    /**
     * Function to retrieve first non-blank line in the input stream up to the first delimiter (delimiter is
     * consumed)
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getFirstNonBlank(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getFirstNonBlank(stream, line, position);
    }

    /**
     * Function to retrieve first non-blank line in the input stream up to the first delimiter (delimiter is
     * consumed)
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getFirstNonBlank(std::istream &stream, std::string &line,
                                         std::streamoff &position) final
    {
        if (stream)
        {
            stream.seekg(0, std::ios_base::beg);

            return getNextNonBlank(stream, line, position);
        }

        return false;
    }

    /**
     * Function to retrieve the last line in the input stream
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getLast(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getLast(stream, line, position);
    }

    /**
     * Function to retrieve the last line in the input stream
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getLast(std::istream &stream, std::string &line, std::streamoff &position) final
    {
        if (stream)
        {
            stream.seekg(-1, std::ios_base::end);

            return getUntilPreviousDelimiter(stream, line, position);
        }

        return false;
    }

    /**
     * Function to retrieve last non-blank line in the input stream back to the previous delimiter (delimiter
     * is consumed)
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getLastNonBlank(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getLastNonBlank(stream, line, position);
    }

    /**
     * Function to retrieve last non-blank line in the input stream back to the previous delimiter (delimiter
     * is consumed)
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getLastNonBlank(std::istream &stream, std::string &line,
                                        std::streamoff &position) final
    {
        if (stream)
        {
            stream.seekg(-1, std::ios_base::end);

            return getPreviousNonBlank(stream, line, position);
        }

        return false;
    }

    /**
     * Function to retrieve the next non-blank line in the input stream
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getNextNonBlank(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getNextNonBlank(stream, line, position);
    }

    /**
     * Function to retrieve the next non-blank line in the input stream
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getNextNonBlank(std::istream &stream, std::string &line,
                                        std::streamoff &position) final
    {
        if (stream)
        {
            if (stream.peek() != m_delimiter && stream.tellg() > 0)
                ignoreUntilNextDelimiter(stream);

            bool bSuccess = true;
            do
            {
                bSuccess &= getUntilNextDelimiter(stream, line, position);
                if (!bSuccess || !std::all_of(line.cbegin(), line.cend(), isspace))
                    break;
            }
            while (stream);

            return bSuccess;
        }

        return false;
    }

    /**
     * Function to retrieve the previous non-blank line in the input stream
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getPreviousNonBlank(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getPreviousNonBlank(stream, line, position);
    }

    /**
     * Function to retrieve the previous non-blank line in the input stream
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getPreviousNonBlank(std::istream &stream, std::string &line,
                                            std::streamoff &position) final
    {
        if (stream)
        {
            if (stream.peek() != m_delimiter)
                ignoreUntilPreviousDelimiter(stream);

            bool bSuccess = true;
            do
            {
                bSuccess &= getUntilPreviousDelimiter(stream, line, position);
                if (!bSuccess || !std::all_of(line.cbegin(), line.cend(), isspace))
                    break;
            }
            while (stream && stream.tellg() > 0);

            return bSuccess;
        }

        return false;
    }

    /**
     * Function to retrieve characters up until the next delimiter (delimiter is consumed)
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getUntilNextDelimiter(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getUntilNextDelimiter(stream, line, position);
    }

    /**
     * Function to retrieve characters up until the next delimiter (delimiter is consumed)
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getUntilNextDelimiter(std::istream &stream, std::string &line,
                                              std::streamoff &position) final
    {
        position = std::string::npos;
        line.clear();
        bool bSuccess = stream.good();
        while (stream)
        {
            std::streamoff currentPosition = stream.tellg();
            auto &&ch = stream.get();
            if (ch == m_delimiter || ch == EOF)
                break;
            else if (position == (std::streamoff)std::string::npos)
                position = currentPosition;

            line += (char)ch;
        }

        return bSuccess;
    }

    /**
     * Function to retrieve characters back to the previous delimiter (delimiter is consumed)
     * @param stream a reference to an std::istream object
     * @param line   a reference to an std::string that will contain the line extracted from the input
     *               stream
     */
    inline virtual bool getUntilPreviousDelimiter(std::istream &stream, std::string &line) final
    {
        std::streamoff position = 0;

        return getUntilPreviousDelimiter(stream, line, position);
    }

    /**
     * Function to retrieve characters back to the previous delimiter (delimiter is consumed)
     * @param stream   a reference to an std::istream object
     * @param line     a reference to an std::string that will contain the line extracted from the input
     *                 stream
     * @param[out] position the starting position of the line with respect to the beginning of the stream
     */
    inline virtual bool getUntilPreviousDelimiter(std::istream &stream, std::string &line,
                                                  std::streamoff &position) final
    {
        position = std::string::npos;
        line.clear();
        bool BOF = false, bSuccess = stream.good();
        while (stream && !BOF)
        {
            auto &&ch = stream.peek();
            if (stream.tellg() > 0LL)
                stream.seekg(-1, std::ios_base::cur);
            else
                BOF = true; // stream is at beginning of file

            if (!BOF && ch == m_delimiter)
            {
                position = stream.tellg() + static_cast<std::streamoff>(2LL);
                break;
            }

            line = (char)ch + line;
        }

        if (BOF)
            position = 0LL;

        return bSuccess;
    }

    /**
     * Function to ignore characters in the stream up to and including the next delimiter
     */
    inline virtual void ignoreUntilNextDelimiter(std::istream &stream) final
    {
        if (stream)
            stream.ignore(std::numeric_limits<std::streamsize>::max(), m_delimiter);
    }

    /**
     * Function to ignore characters in the stream back to and including the previous delimiter
     */
    inline virtual void ignoreUntilPreviousDelimiter(std::istream &stream) final
    {
        while (stream && stream.tellg() > 0LL)
        {
            auto &&ch = stream.peek();
            stream.seekg(-1, std::ios_base::cur);
            if (ch == m_delimiter)
                break;
        }
    }

    /**
     * Set the current delimiter
     */
    inline virtual void setDelimiter(char delimiter) final
    {
        m_delimiter = delimiter;
    }

private:

    /**
     * the character used to delimit lines from the input
     */
    char m_delimiter;
};

}

}

#endif

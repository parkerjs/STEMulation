#ifndef URL_FIELD_H
#define URL_FIELD_H

namespace networking
{

/**
 * Enumerated type to represent the various components of a URL web address
 */
enum URL_Field { Fragment = 1, // fragment identifier
                 Host = 2, // host name
                 Path = 4, // path component
                 Port = 8, // port
                 Query = 16, // query string
                 Scheme = 32, // scheme name
                 UserName = 64, // username
                 UserNameAndUserPassword = 128 // username and password
};

}

#endif

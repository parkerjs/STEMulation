#ifndef CERTIFICATE_DOWNLOADER_H
#define CERTIFICATE_DOWNLOADER_H

#include "downloader.h"

namespace networking
{

namespace sockets
{

// forward declarations
class SSL_Socket;

namespace ssl
{

/**
 * This class retrieves certificate info from the server for secured connections
 */
class CertificateDownloader
: public Downloader,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     * @param url          the web address from which the certificate(s) will be downloaded
     */
    EXPORT_STEM CertificateDownloader(const tDependencies &dependencies,
                                      const std::string &url);

    /**
     * Copy constructor
     */
    EXPORT_STEM CertificateDownloader(const CertificateDownloader &downloader);

    /**
     * Move constructor
     */
    EXPORT_STEM CertificateDownloader(CertificateDownloader &&downloader);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~CertificateDownloader(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM CertificateDownloader &operator = (const CertificateDownloader &downloader);

    /**
     * Move assignment operator
     */
    EXPORT_STEM CertificateDownloader &operator = (CertificateDownloader &&downloader);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual CertificateDownloader *clone(void) const override;

    /**
     * create() function
     * @param dependencies a tuple of this object's required injection dependencies
     * @param url          the web address from which the certificate(s) will be downloaded
     */
    static EXPORT_STEM CertificateDownloader *create(const tDependencies &dependencies,
                                                     const std::string &url);

    /**
     * Default function implementation to download data from a server. Function returns true if data was
     * successfully read from the server
     * @param data an std::string that will be populated with bytes read from the server.
     */
    EXPORT_STEM virtual bool download(std::string &data) override;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "CertificateDownloader";
    }

private:

    /**
     * Function to send request to the server, if connected. Upon success, returns true.
     */
    EXPORT_STEM virtual bool request(void) override;
};

}

}

}

#endif

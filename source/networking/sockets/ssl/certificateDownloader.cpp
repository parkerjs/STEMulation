#include "certificateDownloader.h"
#include "SSL_Socket.h"

namespace networking
{

namespace sockets
{

namespace ssl
{

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 * @param url          the web address from which the certificate(s) will be downloaded
 */
CertificateDownloader::CertificateDownloader(const tDependencies &dependencies,
                                             const std::string &url)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  Downloader(dependencies, url)
{

}

/**
 * Copy constructor
 */
CertificateDownloader::CertificateDownloader(const CertificateDownloader &downloader)
: DependencyInjectableVirtualBaseInitializer(1, downloader),
  Downloader(downloader)
{
    operator = (downloader);
}

/**
 * Move constructor
 */
CertificateDownloader::CertificateDownloader(CertificateDownloader &&downloader)
: DependencyInjectableVirtualBaseInitializer(1, std::move(downloader)),
  Downloader(std::move(downloader))
{
    operator = (std::move(downloader));
}

/**
 * Destructor
 */
CertificateDownloader::~CertificateDownloader(void)
{

}

/**
 * Copy assignment operator
 */
CertificateDownloader &CertificateDownloader::operator = (const CertificateDownloader &downloader)
{
    if (&downloader != this)
    {
        Downloader::operator = (downloader);
    }

    return *this;
}

/**
 * Move assignment operator
 */
CertificateDownloader &CertificateDownloader::operator = (CertificateDownloader &&downloader)
{
    if (&downloader != this)
    {
        Downloader::operator = (std::move(downloader));
    }

    return *this;
}

/**
 * clone() function
 */
CertificateDownloader *CertificateDownloader::clone(void) const
{
    auto *pDownloader = new CertificateDownloader(*this);
    if (pDownloader != nullptr)
        pDownloader->setup();

    return pDownloader;
}

/**
 * create() function
 * @param dependencies a tuple of this object's required injection dependencies
 * @param url          the web address from which the certificate(s) will be downloaded
 */
CertificateDownloader *CertificateDownloader::create(const tDependencies &dependencies,
                                                     const std::string &url)
{
    CertificateDownloader *pDownloader = nullptr;
    if (CertificateDownloader::dependenciesInitialized(dependencies))
    {
        pDownloader = new CertificateDownloader(dependencies, url);
        pDownloader->setup();
    }

    return pDownloader;
}

/**
 * Default function implementation to download data from a server. Function returns true if data was
 * successfully read from the server
 * @param data an std::string that will be populated with bytes read from the server.
 */
bool CertificateDownloader::download(std::string &data)
{
    bool bSuccess = (m_pURL != nullptr);
    if (bSuccess)
    {
        if (m_pSocket == nullptr)
            m_pSocket = createSocket();

        auto *pSocket = dynamic_cast<SSL_Socket *>(m_pSocket);
        bSuccess = (pSocket != nullptr);
        if (bSuccess)
        {
            if (!pSocket->initialized())
                bSuccess = pSocket->initialize();

            if (bSuccess)
                bSuccess = pSocket->connect();

            if (bSuccess)
            {
                data.clear();
                auto *pHandle = pSocket->getHandle();
                bSuccess = (pHandle != nullptr);
                if (bSuccess)
                {
                    auto *pCertificate = SSL_get_peer_certificate(pHandle); // get the server's certificate
                    if (pCertificate != nullptr)
                    {
                        data = "Server certificates:\n";
                        auto *pLine = X509_NAME_oneline(X509_get_subject_name(pCertificate), nullptr, 0);
                        data += "Subject: " + std::string(pLine) + "\n";
                        OPENSSL_free(pLine);
                        pLine = X509_NAME_oneline(X509_get_issuer_name(pCertificate), nullptr, 0);
                        data += "Issuer: " + std::string(pLine) + "\n";
                        OPENSSL_free(pLine);
                        X509_free(pCertificate);
                    }
                    else
                        data = "No certificates.\n";
                }
            }

            pSocket->disconnect();
        }
    }

    return bSuccess;
}

/**
 * Function to send request to the server, if connected. Upon success, returns true.
 */
inline bool CertificateDownloader::request(void)
{
    return true;
}

}

}

}
